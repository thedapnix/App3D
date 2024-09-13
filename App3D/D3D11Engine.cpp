#include "ImGuiHelper.h" //Which in turn includes D3D11Engine.h

#include <dxgi.h>			//directx driver interface
#include <d3dcompiler.h>	//compiling shaders

//#include <algorithm> //min and max

/*
It is useful to re-think the terminology of matrices I think:
World matrix is more like "localToWorld"
View matrix is more like "worldToView"
Proj matrix is more like "viewToClip" (this is where window dimensions come into play and cull anything outside of it because we're preparing for this to be the final frame)
The viewport then transforms from clip space into screen space (which is literally just post-cull, not changing positions) and that's our finished frame

We want to add lighting to the gun (which is drawn in view-space (doesn't perform the view transformation and thus stays in frame at all times in relation to camera position)
Lights calculate whether they hit something in the pixel shader based off of an objects position in world-space (because the lights are also in world-space)
Make a separate pixel shader that transforms the lights to be in view-space, and to check against objects positions in view-space as well?

Another idea what I cooked up while in bed thinking about this stuff because programming has become my life:
Make separate m_drawables and m_spotlights along with this, and make a render-pass that uses the new shaders on only things that are supposed to be in this "pov" render
This makes sense because when the lights calculate if they hit something, they're calculating against the drawables world position
	This won't work, since the position of the gun in the world is not where it's expected to be (its world coordinates are written more like an offset from the camera)
	Thus, we need lights that do the same thing, I just hope this won't fuck with other parts of the light (sure i can fix their position, but what will happen to the cones of the spotlight? i hope it'll be fine)

	
Optimal would be if I could perform lighting calculations separately, that way I can have different vertex positions depending on what we're doing
	Placement in the final image? Proj only
	Performing lighting calculations? Properly transformed



New: Reworked drawables to pass a complete world-transform to shaders, and have it multiply things differently if it's supposed to orbit around a point or not
To-do: Make the orbitting better, example formula from rasmus:
Scale * (offset from center) * Rotation X * Rotation Y * (position of center)

*/

#include "Interact.h"

using namespace DirectX;

D3D11Engine::D3D11Engine(const HWND& hWnd, const UINT& width, const UINT& height)
{
	m_windowWidth = width;
	m_windowHeight = height;

	//Base d3d setup
	InitInterfaces(hWnd);
	InitViewport(viewport);
	InitViewport(povViewport);
	InitRTV();
	InitDepthStencil();
	InitUAV(); //deferred

	//Render setup
	InitShadersAndInputLayout();

	//Deferred setup
	InitGraphicsBuffer(m_gBuffers);

	//Camera setup (matrices and constant buffer)
	InitCamera();

	//Tesselation helper setup (allow for different rasterizer states to show tessellation works through wireframe rendering)
	InitRasterizerStates();

	//Billboarding setup
	m_particles = ParticleSystem(device.Get());

	//Cube environment mapping setup
	m_cubeMap = CubeMap(device.Get(), true);

	//Sampler setup for texture access in shaders
	InitSampler();

	//ImGui setup
	SetupImGui(hWnd, device.Get(), context.Get());
}

D3D11Engine::~D3D11Engine()
{
	ClearImGui();
}

void D3D11Engine::Update(float dt)
{
	//When we pass constant buffers to shaders, it's important that the matrices in them are transposed. They should NOT be transposed before that though
	//Make the UpdateConstantBuffer() functions transpose and pass?

	//UPDATE CONSTANT BUFFERS
	m_camera->UpdateConstantBuffer(context.Get());

	//m_drawables.at(11).Rotate(0.0f, 0.0005f * dt, 0.001f * dt);
	for (auto& drawable : m_drawables)
	{
		drawable.UpdateConstantBuffer(context.Get());
	}
	for (auto& mirrorCube : m_reflectiveDrawables)
	{
		mirrorCube.Rotate(0.0f, 0.0003f * dt, 0.0f);
		mirrorCube.UpdateConstantBuffer(context.Get());
	}
	for (auto& drawable : m_povDrawables)
	{
		drawable.UpdateConstantBuffer(context.Get(), true, m_camera.get()); 
	}

	m_particleVar += dt;
	m_particles.UpdateConstantBuffer(context.Get(), m_particleVar);

	//RENDER
	RenderDepth(dt);
	Render(dt, rtv.Get(), dsv.Get(), &viewport, m_camera.get(), CLEAR_COLOR);
	if (billboardingIsEnabled) RenderParticles(m_camera.get());
	if (cubemapIsEnabled) RenderReflectiveObject(dt);

	//PRESENT
	swapChain->Present(1, 0); //vSync, 1 = enabled, 0 = disabled (or in other terms, fps limit to screen hz or uncap)
}

void D3D11Engine::ImGuiSceneData(D3D11Engine* d3d11engine, bool shouldUpdateFps, int state, int rawX, int rawY)
{
	StartImGuiFrame(); //Begin

	fpsCounter++; //FPS counter, input from app-class
	if (shouldUpdateFps)
	{
		fpsString = std::to_string(fpsCounter);
		fpsCounter = 0;
	}


	ImGuiEngineWindow(
		m_camera.get(), fpsString, state,
		deferredIsEnabled, cullingIsEnabled, billboardingIsEnabled, lodIsEnabled, cubemapIsEnabled,
		drawablesBeingRendered, selectableDrawables, 
		rawX, rawY
	);

	EndImGuiFrame(); //End
}

void D3D11Engine::MovePlayerX(float speed)
{
	//Store a temporary position instead of modifying the original
	XMFLOAT3 tempPos = m_camera->GetPosition();
	
	//Identical to the camera Strafe() but we're doing it on tempPos
	XMVECTOR s = XMVectorReplicate(speed);
	XMVECTOR r = m_camera->GetRight();
	XMVECTOR p = XMLoadFloat3(&tempPos);
	XMStoreFloat3(&tempPos, XMVectorMultiplyAdd(s, r, p));

	//Update position of camera and its bounding volume (Also does collision testing)
	MovePlayer(tempPos);
}

void D3D11Engine::MovePlayerZ(float speed)
{
	//Store a temporary position instead of modifying the original
	XMFLOAT3 tempPos = m_camera->GetPosition();

	//Identical to the camera PlayerWalk() but we're doing it on tempPos
	XMMATRIX temp = XMMatrixRotationY(m_camera->GetYaw());
	XMVECTOR newLook = XMVector3TransformNormal(m_camera->GetDefaultLook(), temp);
	XMVECTOR pos = XMLoadFloat3(&tempPos);
	pos += speed * newLook;
	XMStoreFloat3(&tempPos, pos);

	//Update position of camera and its bounding volume (Also does collision testing)
	MovePlayer(tempPos);
}

Camera& D3D11Engine::GetCamera() const noexcept
{
	return *m_camera;
}

void D3D11Engine::PlayerInteract()
{
	int n = 0;

	for (auto& drawable : m_drawables)
	{
		if (m_camera->GetSelectionFrustum().Intersects(drawable.GetBoundingBox()))
		{
			if (drawable.GetInteractID() > 0)
			{
				drawable.Interact(InteractionBaseFunc, m_drawables);
				break; //Only interact with one object at a time
			}
		}
	}

	selectableDrawables = n;

	//Also for funsies: Change views?
	//m_camera->ChangeFrustum();
}

bool D3D11Engine::CreateDrawable(std::string objFileName, DirectX::XMFLOAT3 translate, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotate, int interact, std::vector<int> interactsWith)
{
	InitDrawableFromFile(objFileName, m_drawables, scale, rotate, translate, m_textures, device.Get(), interact, interactsWith);

	return true;
}

bool D3D11Engine::CreateReflectiveDrawable(std::string objFileName, DirectX::XMFLOAT3 translate, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotate, int interact, std::vector<int> interactsWith)
{
	InitDrawableFromFile(objFileName, m_reflectiveDrawables, scale, rotate, translate, m_textures, device.Get(), interact, interactsWith); //temp: just put false as interactible here bleh

	return true;
}

bool D3D11Engine::CreatePovDrawable(std::string objFileName, DirectX::XMFLOAT3 translate, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotate, int interact, std::vector<int> interactsWith)
{
	InitDrawableFromFile(objFileName, m_povDrawables, scale, rotate, translate, m_textures, device.Get(), interact, interactsWith);

	return true;
}

bool D3D11Engine::MoveDrawable(int i, DirectX::XMFLOAT3 dist)
{
	m_drawables.at(i).EditTranslation(dist.x, dist.y, dist.z);

	return true;
}

bool D3D11Engine::SetupQT()
{
	for (const auto& drawable : m_drawables)
		m_quadTree.AddElement(&drawable, drawable.GetBoundingBox());

	return true;
}

void D3D11Engine::RemoveDrawableInteraction(int id)
{
	for (auto& drawable : m_drawables)
	{
		if (drawable.GetInteractID() == id)
		{
			drawable.RemoveInteraction();
		}
	}
}

void D3D11Engine::DestroyDrawable(int id)
{
	m_drawables.at(id).Destroy();
}

int D3D11Engine::GetDrawableIndexFromInteraction(int interactId)
{
	for (int i = 0; i < m_drawables.size(); i++)
	{
		if (m_drawables.at(i).GetInteractID() == interactId)
		{
			return i;
		}
	}

	return -1; //As well as some error message if you wanna
}

std::vector<Drawable>& D3D11Engine::GetDrawables()
{
	return m_drawables;
}

std::vector<Drawable>& D3D11Engine::GetPovDrawables()
{
	return m_povDrawables;
}

bool D3D11Engine::CreateLightSpot(DirectX::XMFLOAT3 position, float fov, float rotX, float rotY, DirectX::XMFLOAT3 color)
{
	LightData data;

	//Pass in fov as a float between 0 and 1. Bigger number = Wider cone. Default 0.25f
	//Pass in rotX as a float between -1 to +1. Negative values angle the light to the left, positive values angle the light to the right. Default to 0
	//Pass in rotY as a float between -1 to +1. Negative values angle the light upwards, positive values angle the light downwards. Default to 0

	data.pos = position;
	data.fovY = XM_PI * fov;	//Example: Passing in  0.25f would be the same as the previous  " XM_PI / 4.0f "
	data.rotX = XM_PI * rotX;	//Example: Passing in -0.25f would be the same as the previous  "-XM_PI / 4.0f "
	data.rotY = XM_PI * rotY;	//Example: Passing in 0.125f would be the same as the previous  " XM_PI / 8.0f "
	data.col = color;			//Default value {1.0f, 1.0f, 1.0f}

	m_lightDataVec.push_back(data);

	return true;
}

bool D3D11Engine::CreatePovLightSpot(DirectX::XMFLOAT3 position, float fov, float rotX, float rotY, DirectX::XMFLOAT3 color)
{
	LightData data;

	//Pass in fov as a float between 0 and 1. Bigger number = Wider cone. Default 0.25f
	//Pass in rotX as a float between -1 to +1. Negative values angle the light to the left, positive values angle the light to the right. Default to 0
	//Pass in rotY as a float between -1 to +1. Negative values angle the light upwards, positive values angle the light downwards. Default to 0

	data.pos = position;
	data.fovY = XM_PI * fov;	//Example: Passing in  0.25f would be the same as the previous  " XM_PI / 4.0f "
	data.rotX = XM_PI * rotX;	//Example: Passing in -0.25f would be the same as the previous  "-XM_PI / 4.0f "
	data.rotY = XM_PI * rotY;	//Example: Passing in 0.125f would be the same as the previous  " XM_PI / 8.0f "
	data.col = color;			//Default value {1.0f, 1.0f, 1.0f}

	m_povLightDataVec.push_back(data);

	return true;
}

bool D3D11Engine::CreateLightDir(DirectX::XMFLOAT3 position, float rotX, float rotY, DirectX::XMFLOAT3 color)
{
	//Directional light
	LightData data;

	//PREVIOUSLY
	//data.pos = XMFLOAT3(-80.0f, 1.0f, -6.0f);
	//data.fovY = 0.0f;
	//data.rotX = XM_PIDIV2;
	//data.rotY = XM_PI / 8.0f;
	//data.col = DirectX::XMFLOAT3(0.25f, 0.25f, 0.25f);

	//NEW
	data.pos = position;
	data.fovY = 0.0f;
	data.rotX = XM_PI * rotX; //For XM_PI / 2 --> 0.5f
	data.rotY = XM_PI * rotY; //For XM_PI / 8 --> 0.125f
	data.col = color; //Since directional lights don't fall off with distance, we manually lower the strength of the light here (got bright white walls earlier)

	m_lightDataVec.push_back(data);

	return true;
}

bool D3D11Engine::SetupLights()
{
	m_spotlights = SpotLights(device.Get(), m_lightDataVec);
	m_shadowMap = ShadowMap(device.Get(), &m_drawables, &m_spotlights); //"SetupLights()", shadows too while we're at it, but don't tell anyone :3

	//new (dont bother making pov things cast shadows yet)
	m_povSpotlights = SpotLights(device.Get(), m_povLightDataVec);
	m_povShadowMap = ShadowMap(device.Get(), &m_povDrawables, &m_povSpotlights);

	return true;
}

/*RENDER FUNCTIONS*/
void D3D11Engine::Render(float dt, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv, D3D11_VIEWPORT* viewport, Camera* cam, const float clear[4])
{
	/*Update buffers and camera frustum here*/
	if (deferredIsEnabled)
	{
		DefPassOne(cam); //Does the same as what's in the else-statement, except to several rendertargets
	}
	else
	{
		// Clear the back buffer and depth stencil, as well as set viewport and render target (viewport only really needed to set after a resize, and that's disabled so uh)
		context->ClearRenderTargetView(rtv, clear);
		context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		context->RSSetViewports(1, viewport);
		context->OMSetRenderTargets(1, &rtv, dsv);

		//INPUT ASSEMBLER STAGE
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST); //farewell D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
		context->IASetInputLayout(inputLayout.Get());

		//SHADER STAGE
		context->VSSetShader(vertexShader.Get(), NULL, 0);
		context->PSSetShader(pixelShader.Get(), NULL, 0);
		context->PSSetSamplers(0, 1, samplerState.GetAddressOf());
		
		//SHADOWS AND LIGHTING STUFF
		ID3D11ShaderResourceView* shadowViews[] = { m_spotlights.GetStructuredBufferSRV() , m_spotlights.GetDepthBufferSRV() };
		context->PSSetShaderResources(3, 2, shadowViews);
		context->PSSetConstantBuffers(0, 1, cam->GetConstantBuffer().GetBufferAddress());
		ID3D11SamplerState* shadowSampler = m_shadowMap.GetSampler();
		context->PSSetSamplers(1, 1, &shadowSampler);

		//TESSELLATION
		if(lodIsEnabled)context->RSSetState(wireframeRS.Get());
		else			context->RSSetState(regularRS.Get());
		context->HSSetShader(hullShader.Get(), NULL, 0);
		context->DSSetShader(domainShader.Get(), NULL, 0);
		context->DSSetConstantBuffers(0, 1, cam->GetConstantBuffer().GetBufferAddress());
		context->HSSetConstantBuffers(0, 1, cam->GetConstantBuffer().GetBufferAddress());

		//DRAW (AND CULL IF ENABLED)
		if (cullingIsEnabled)
		{
			int visibleDrawables = 0;
			for (const auto& drawable : m_quadTree.CheckTree(cam->GetFrustum()))
			{
				drawable->Bind(context.Get());
				visibleDrawables++;
			}
			drawablesBeingRendered = visibleDrawables;
		}
		else
		{
			for (auto& drawable : m_drawables)
			{
				drawable.Bind(context.Get());
			}
			drawablesBeingRendered = (int)m_drawables.size();
		}

		//Pov drawables are never culled so we do this outside the if-else check
		for (auto& drawable : m_povDrawables)
		{
			drawable.Bind(context.Get());
		}

		//UNBIND THINGS FOR SANITY REASONS
		context->VSSetShader(NULL, NULL, 0);
		context->PSSetShader(NULL, NULL, 0);
		context->HSSetShader(NULL, NULL, 0);
		context->DSSetShader(NULL, NULL, 0);

		context->HSSetConstantBuffers(0, 0, NULL);
		context->DSSetConstantBuffers(0, 0, NULL);

		ID3D11RenderTargetView* nullRTV = NULL;
		context->OMSetRenderTargets(1, &nullRTV, NULL);

		ID3D11ShaderResourceView* nullSRVs[] = {NULL, NULL, NULL, NULL, NULL};
		context->PSSetShaderResources(0, 5, nullSRVs);

		ID3D11SamplerState* nullSamplers[] = {NULL, NULL};
		context->PSSetSamplers(0, 2, nullSamplers);
	}

	if (deferredIsEnabled)
	{
		DefPassTwo(cam); //Lighting pass, editing the backbuffer using a compute shader
	}
}

void D3D11Engine::RenderParticles(Camera* cam)
{
	context->RSSetViewports(1, &viewport);
	context->OMSetRenderTargets(1, rtv.GetAddressOf(), dsv.Get());

	/*Input Assembler Stage*/
	context->IASetInputLayout(NULL); //Done in cookbook, feels weird
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	/*Shader Stage*/
	context->VSSetShader(m_particles.GetVertexShader(), NULL, 0);
	ID3D11ShaderResourceView* view = m_particles.GetSRV();
	context->VSSetShaderResources(0, 1, &view);
	context->GSSetShader(m_particles.GetGeometryShader(), NULL, 0);
	context->GSSetConstantBuffers(0, 1, cam->GetConstantBuffer().GetBufferAddress());
	context->PSSetShader(m_particles.GetPixelShader(), NULL, 0);

	//Draw
	context->Draw(m_particles.GetElementCount(), 0);

	//Unbind shaders and constant buffer
	context->VSSetShader(NULL, NULL, 0);
	ID3D11ShaderResourceView* nullSRV = NULL;
	context->VSSetShaderResources(0, 1, &nullSRV);
	context->GSSetShader(NULL, NULL, 0);
	context->GSSetConstantBuffers(0, 0, NULL);
	context->PSSetShader(NULL, NULL, 0);
	ID3D11RenderTargetView* nullRTV = NULL;
	context->OMSetRenderTargets(0, &nullRTV, NULL);

	//Use compute shader and uav to edit stuff
	context->CSSetShader(m_particles.GetComputeShader(), NULL, 0);
	context->CSSetConstantBuffers(0, 1, m_particles.GetConstantBuffer().GetBufferAddress());
	context->CSSetUnorderedAccessViews(0, 1, m_particles.GetUAVAddress(), NULL);
	context->Dispatch(m_windowWidth / 32, m_windowHeight, 1); //Match the compute shader numthreads, [32, 1, 1]

	//Unbind uav
	ID3D11UnorderedAccessView* nullUAV = NULL;
	context->CSSetUnorderedAccessViews(0, 1, &nullUAV, NULL);

	//Unbind shader and constant buffer
	context->CSSetShader(NULL, NULL, 0);
	context->CSSetConstantBuffers(0, 0, NULL);
}

void D3D11Engine::RenderReflectiveObject(float dt)
{
	//Exit out if there are no reflective objects
	if (m_reflectiveDrawables.size() == 0)
	{
		return;
	}

	//Get the reflective object (This whole stuff is hardcoded right now but bear with me
	const Drawable* cube = &m_reflectiveDrawables.at(0);
	std::vector<const Drawable*> visibleDrawables = m_quadTree.CheckTree(m_camera->GetFrustum());
	if(cullingIsEnabled && std::find(visibleDrawables.begin(), visibleDrawables.end(), cube) == visibleDrawables.end())
	{
		return;
	}
	else
	{
		for (int i = 0; i < 6; i++)
		{
			/*Cookbook notes
			Perform your normal rendering logic here, rendering each relevant(from the reflective object's perspective) object in the scene
			Instead of the "normal" render target (potentially the back buffer), render to the one associated with the texture cube side currently being processed
			Instead of the "normal" depth stencil, use the one associated with the reflective object's texture cube
			Instead of the "normal" viewport, use the one associated with the reflective object's texture cube
			Instead of the "normal" camera information, use the one associated with the texture cube side currently being processed
			*/
			Render(dt, m_cubeMap.GetRenderTargetViewAt(i), m_cubeMap.GetDepthStencilView(), m_cubeMap.GetViewport(), m_cubeMap.GetCameraAt(i), CLEAR_COLOR);
		}

		/*Copy the Render() function but remove stuff we're not interested in here*/
		context->RSSetViewports(1, &viewport);
		context->OMSetRenderTargets(1, rtv.GetAddressOf(), dsv.Get());

		/*Input Assembler Stage*/
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //return of trianglelist, because we're just doing old-school vertex+pixel shader, no other fancy shaders
		context->IASetInputLayout(inputLayout.Get());

		/*Shader Stage*/
		context->VSSetShader(m_cubeMap.GetVertexShader(), NULL, 0);
		context->VSSetConstantBuffers(1, 1, m_camera->GetConstantBuffer().GetBufferAddress());

		context->PSSetShader(m_cubeMap.GetPixelShader(), NULL, 0);
		context->PSSetShaderResources(0, 1, m_cubeMap.GetShaderResourceViewAddress());
		context->PSSetSamplers(0, 1, samplerState.GetAddressOf());
		context->PSSetConstantBuffers(0, 1, m_camera->GetConstantBuffer().GetBufferAddress());

		for (auto& drawable : m_reflectiveDrawables)
		{
			drawable.Bind(context.Get(), true);
		}

		/*Unbind shaders*/
		//Vertex shader
		context->VSSetShader(NULL, NULL, 0);
		context->VSSetConstantBuffers(0, 0, NULL);
		//Pixel shader
		context->PSSetShader(NULL, NULL, 0);
		context->PSSetConstantBuffers(0, 0, NULL);
		ID3D11ShaderResourceView* nullSRV = NULL;
		context->PSSetShaderResources(0, 0, &nullSRV);
		ID3D11SamplerState* nullSampler = NULL;
		context->PSSetSamplers(0, 0, &nullSampler);

		/*Unbind rtv*/
		ID3D11RenderTargetView* nullRTV = NULL;
		context->OMSetRenderTargets(1, &nullRTV, NULL);
	}
}

void D3D11Engine::RenderDepth(float dt)
{
	/*
	For future reference: If we want pov-stuff to cast shadows we gotta change VSSetShader to m_shadowMap.GetPovVertexShader() when rendering for all m_povDrawables
	Probably do more shader stuff than that but I'm not the biggest shader fan, me C++
	*/

	/*Bind stuff*/
	context->IASetInputLayout(inputLayout.Get());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->VSSetShader(m_shadowMap.GetVertexShader(), NULL, 0);
	context->RSSetViewports(1, m_shadowMap.GetViewport());

	/*Render depth from the perspective of every light*/
	for (UINT i = 0; i < m_spotlights.GetLightCount(); i++)
	{
		ID3D11DepthStencilView* dsView = m_spotlights.GetDepthStencilViewAt(i);
		context->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH, 1, 0);
		context->OMSetRenderTargets(0, NULL, dsView); //no rtv, only dsv

		//Some way of getting the camera cb of individual lights
		ID3D11Buffer* cameraCB = m_spotlights.GetCameraConstantBufferAt(i).GetBuffer();
		context->VSSetConstantBuffers(1, 1, &cameraCB);

		for (auto& drawable : m_drawables)
		{
			drawable.Bind(context.Get());
		}
	}

	//Change binding for the pov lights
	context->VSSetShader(m_povShadowMap.GetVertexShader(), NULL, 0);
	context->RSSetViewports(1, m_povShadowMap.GetViewport());

	for (UINT i = 0; i < m_povSpotlights.GetLightCount(); i++)
	{
		ID3D11DepthStencilView* dsView = m_povSpotlights.GetDepthStencilViewAt(i);
		context->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH, 1, 0);
		context->OMSetRenderTargets(0, NULL, dsView); //no rtv, only dsv

		//Some way of getting the camera cb of individual lights
		ID3D11Buffer* cameraCB = m_povSpotlights.GetCameraConstantBufferAt(i).GetBuffer();
		context->VSSetConstantBuffers(1, 1, &cameraCB);

		for (auto& drawable : m_povDrawables)
		{
			drawable.Bind(context.Get());
		}
	}

	/*Unbind stuff*/
	context->OMSetRenderTargets(0, NULL, NULL);
	context->VSSetShader(NULL, NULL, 0);
	context->VSSetConstantBuffers(0, 0, NULL);
}

/*DEFERRED RENDERING PASSES*/
void D3D11Engine::DefPassOne(Camera* cam)
{
	//Deferred rendering splits rendering into 3 parts: A geometry pass, a draw pass, and a lighting pass

	///////////////////////////////////////////////////////////////////////////////
	//GEOMETRY PASS, FILL OUR GBUFFERS WITH DATA
	context->ClearRenderTargetView(m_gBuffers[0].rtv.Get(), CLEAR_COLOR);
	context->ClearRenderTargetView(m_gBuffers[1].rtv.Get(), CLEAR_COLOR);
	context->ClearRenderTargetView(m_gBuffers[2].rtv.Get(), CLEAR_COLOR);
	context->ClearRenderTargetView(m_gBuffers[3].rtv.Get(), CLEAR_COLOR);
	context->ClearDepthStencilView(dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);
	context->RSSetViewports(1, &viewport);
	ID3D11RenderTargetView* rtvArr[] = { m_gBuffers[0].rtv.Get(), m_gBuffers[1].rtv.Get(), m_gBuffers[2].rtv.Get(), m_gBuffers[3].rtv.Get() }; //Create an array of render target views and fill it with the rtv's from our gbuffers
	context->OMSetRenderTargets(4, rtvArr, dsv.Get()); //When render targets are bound to the output merger (if I understand correctly), they are sent to the pixel shader where they get filled with data yes?

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	context->IASetInputLayout(inputLayout.Get());

	///////////////////////////////////////////////////////////////////////////////
	context->VSSetShader(vertexShader.Get(), NULL, 0);
	context->PSSetShader(deferredPixelShader.Get(), NULL, 0);
	context->PSSetSamplers(0, 1, samplerState.GetAddressOf());

	//Tessellation
	if (lodIsEnabled)context->RSSetState(wireframeRS.Get());
	else			context->RSSetState(regularRS.Get());
	context->HSSetShader(hullShader.Get(), NULL, 0);
	context->DSSetShader(domainShader.Get(), NULL, 0);
	context->DSSetConstantBuffers(0, 1, cam->GetConstantBuffer().GetBufferAddress());
	context->HSSetConstantBuffers(0, 1, cam->GetConstantBuffer().GetBufferAddress());

	//This right here is why deferred rendering is better with multiple lights, but worse with multiple drawables
	//We're drawing all our drawables onto all 3 render targets
	if (cullingIsEnabled)
	{
		int visibleDrawables = 0;
		for (auto& drawable : m_quadTree.CheckTree(cam->GetFrustum()))
		{
			drawable->Bind(context.Get());
			visibleDrawables++;
		}
		drawablesBeingRendered = visibleDrawables;
	}
	else
	{
		//Per drawable: bind vertex and index buffers, then draw them
		for (auto& drawable : m_drawables)
		{
			drawable.Bind(context.Get());
		}
		drawablesBeingRendered = (int)m_drawables.size();
	}

	//Now that we're done writing data to the render targets, unbind them
	ID3D11RenderTargetView* nullRTVs[] = { NULL, NULL, NULL, NULL };
	context->OMSetRenderTargets(4, nullRTVs, NULL);

	context->VSSetShader(NULL, NULL, 0);
	context->PSSetShader(NULL, NULL, 0);
	context->HSSetShader(NULL, NULL, 0);
	context->DSSetShader(NULL, NULL, 0);

	context->HSSetConstantBuffers(0, 0, NULL);
	context->DSSetConstantBuffers(0, 0, NULL);

	ID3D11ShaderResourceView* nullSRVs[] = { NULL, NULL, NULL };
	context->PSSetShaderResources(0, 3, nullSRVs);

	ID3D11SamplerState* nullSamplers[] = { NULL };
	context->PSSetSamplers(0, 1, nullSamplers);
}

void D3D11Engine::DefPassTwo(Camera* cam)
{
	///////////////////////////////////////////////////////////////////////////////
	//LIGHTING PASS, USE COMPUTE SHADER TO EDIT THE BACKBUFFER AND DO LIGHTING COMPUTATIONS
	ID3D11RenderTargetView* nullRtv = NULL;
	context->OMSetRenderTargets(1, &nullRtv, NULL);

	context->CSSetShader(computeShader.Get(), NULL, 0);

	ID3D11ShaderResourceView* srvArr[] = { m_gBuffers[0].srv.Get(), m_gBuffers[1].srv.Get(), m_gBuffers[2].srv.Get(), m_gBuffers[3].srv.Get(),
	 m_spotlights.GetStructuredBufferSRV() , m_spotlights.GetDepthBufferSRV() }; //shadows
	context->CSSetShaderResources(0, 6, srvArr);

	context->CSSetConstantBuffers(0, 1, cam->GetConstantBuffer().GetBufferAddress());
	ID3D11SamplerState* shadowSampler = m_shadowMap.GetSampler();
	context->CSSetSamplers(0, 1, &shadowSampler);

	//Use unordered access view to edit the backbuffer
	context->ClearUnorderedAccessViewFloat(uav.Get(), CLEAR_COLOR);
	context->CSSetUnorderedAccessViews(0, 1, uav.GetAddressOf(), NULL); //Last value matters in case of "append consume" buffers, but I've only heard of that, I've *no idea* what it means
	context->Dispatch(m_windowWidth / 8, m_windowHeight / 8, 1);		//In order to make our Dispatch cover the entire window, we group threads by the width and height of the 
	//window divided by 8 (as 8x8 is defined in compute shader)

	//UNBIND
	context->CSSetShader(NULL, NULL, 0);
	context->CSSetConstantBuffers(0, 0, NULL);
	ID3D11UnorderedAccessView* nullUav = NULL;
	context->CSSetUnorderedAccessViews(0, 1, &nullUav, NULL);
	ID3D11ShaderResourceView* nullSRVs[] = { NULL, NULL, NULL, NULL, NULL, NULL };
	context->CSSetShaderResources(0, 6, nullSRVs);
	ID3D11SamplerState* nullSamplers[] = { NULL };
	context->PSSetSamplers(0, 1, nullSamplers);
}

void D3D11Engine::RenderPov(float dt, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv, D3D11_VIEWPORT* viewport, Camera* cam, const float clear[4])
{
	//Renders things directly in relation to the camera view, without projection
	/*
	TO-DO:
	1. UPDATE CAMERA (DONE, BUT DOESN'T HAPPEN IN HERE)
	2. UPDATE THE POSITION OF THE OBJECT (DONE, ALSO DOESN'T HAPPEN IN HERE)
	3. PERFORM WORLD TRANSFORMATION TO PLACE IT SOMEWHERE IN THE WORLD OFFSET FROM THE CAMERA (THIS IS WHAT WE ALWAYS DO)
	4. PERFORM PROJECTION TRANSFORMATION (BUT NOT VIEW) OF THE CAMERA ON THE OBJECT ITSELF
		IN ORDER TO DO THIS, YOU'LL NEED A NEW SHADER AND A NEW CONSTANT BUFFER
	*/

	// Clear the back buffer and depth stencil, as well as set viewport and render target (viewport only really needed to set after a resize, and that's disabled so uh)
	//context->ClearRenderTargetView(rtv, clear);
	//context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->RSSetViewports(1, viewport);
	context->OMSetRenderTargets(1, &rtv, dsv);

	//INPUT ASSEMBLER STAGE
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	context->IASetInputLayout(inputLayout.Get());

	//SHADER STAGE
	context->VSSetShader(vertexShader.Get(), NULL, 0);
	context->PSSetShader(pixelShaderPov.Get(), NULL, 0);
	context->PSSetSamplers(0, 1, samplerState.GetAddressOf());

	//SHADOWS AND LIGHTING STUFF
	ID3D11ShaderResourceView* shadowViews[] = { m_povSpotlights.GetStructuredBufferSRV() , m_povSpotlights.GetDepthBufferSRV() }; //Passing in povSpotlights instead of the regular
	context->PSSetShaderResources(3, 2, shadowViews);
	context->PSSetConstantBuffers(0, 1, cam->GetConstantBuffer().GetBufferAddress());
	ID3D11SamplerState* shadowSampler = m_povShadowMap.GetSampler();//Won't be used here, but need to be passed in because the shader expects it
	context->PSSetSamplers(1, 1, &shadowSampler);					//Same as above

	//TESSELLATION
	if (lodIsEnabled)context->RSSetState(wireframeRS.Get());
	else			context->RSSetState(regularRS.Get());
	context->HSSetShader(hullShader.Get(), NULL, 0);
	context->DSSetShader(domainShader.Get(), NULL, 0); //This is where coordinates will be transformed but without the view-transformation
	context->DSSetConstantBuffers(0, 1, cam->GetConstantBuffer().GetBufferAddress());
	context->HSSetConstantBuffers(0, 1, cam->GetConstantBuffer().GetBufferAddress());

	//DRAW (we'll never cull something like this because it's always intended to be in view of the camera)
	for (auto& drawable : m_povDrawables)
	{
		drawable.Bind(context.Get());
	}

	//UNBIND THINGS FOR SANITY REASONS
	context->VSSetShader(NULL, NULL, 0);
	context->PSSetShader(NULL, NULL, 0);
	context->HSSetShader(NULL, NULL, 0);
	context->DSSetShader(NULL, NULL, 0);

	context->HSSetConstantBuffers(0, 0, NULL);
	context->DSSetConstantBuffers(0, 0, NULL);

	ID3D11RenderTargetView* nullRTV = NULL;
	context->OMSetRenderTargets(1, &nullRTV, NULL);

	ID3D11ShaderResourceView* nullSRVs[] = { NULL, NULL, NULL, NULL, NULL };
	context->PSSetShaderResources(0, 5, nullSRVs);

	ID3D11SamplerState* nullSamplers[] = { NULL, NULL };
	context->PSSetSamplers(0, 2, nullSamplers);
}

/*INITIALIZERS FOR DIRECTX STUFF*/
void D3D11Engine::InitRasterizerStates()
{
	//Make two different rasterizer states so we can switch between them if we want to
	//First one will be a "regular" rasterizer state, meant to render things the way we'd typically want to
	D3D11_RASTERIZER_DESC regularDesc;
	ZeroMemory(&regularDesc, sizeof(regularDesc));
	regularDesc.AntialiasedLineEnable = false;
	regularDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;//D3D11_CULL_MODE::D3D11_CULL_NONE;
	regularDesc.DepthBias = 0;
	regularDesc.DepthBiasClamp = 0;
	regularDesc.DepthClipEnable = true;
	regularDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	regularDesc.FrontCounterClockwise = false;
	regularDesc.MultisampleEnable = false;
	regularDesc.ScissorEnable = false;
	regularDesc.SlopeScaledDepthBias = 0;

	HRESULT hr = device->CreateRasterizerState(&regularDesc, regularRS.GetAddressOf()); //& ?

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create regular rasterizer state!", L"Error", MB_OK);
	}

	//Second one will be for wireframe rasterization, which will help check if our tessellation is working
	D3D11_RASTERIZER_DESC wfDesc;
	ZeroMemory(&wfDesc, sizeof(wfDesc));
	wfDesc.AntialiasedLineEnable = false;
	wfDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	wfDesc.DepthBias = 0;
	wfDesc.DepthBiasClamp = 0;
	wfDesc.DepthClipEnable = true;
	wfDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME; //otherwise identical to regular rendering, but we no longer want to fill rasterized triangles with color
	wfDesc.FrontCounterClockwise = false;
	wfDesc.MultisampleEnable = false;
	wfDesc.ScissorEnable = false;
	wfDesc.SlopeScaledDepthBias = 0;

	hr = device->CreateRasterizerState(&wfDesc, wireframeRS.GetAddressOf());

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create wireframe rasterizer state!", L"Error", MB_OK);
	}
}

void D3D11Engine::InitInterfaces(const HWND& window)
{
	/*
	CREATE SWAPCHAIN, DEVICE AND IMMEDIATE CONTEXT
	*/
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BufferDesc.Width = 0;
	desc.BufferDesc.Height = 0;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.BufferDesc.RefreshRate.Numerator = 0;
	desc.BufferDesc.RefreshRate.Denominator = 1; //previously 0
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS;
	desc.BufferCount = 1;
	desc.OutputWindow = window;
	desc.Windowed = TRUE;
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	desc.Flags = 0; //DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH

	UINT swapCreateFlags = D3D11_CREATE_DEVICE_SINGLETHREADED; //previously 0
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		swapCreateFlags,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&desc,
		&swapChain,
		&device,
		NULL,
		&context
	);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create device & swapchain!", L"Error", MB_OK);
	}
}

void D3D11Engine::InitRTV()
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create backbuffer!", L"Error", MB_OK);
		return;
	}

	hr = device->CreateRenderTargetView(backBuffer.Get(), NULL, rtv.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create render target view!", L"Error", MB_OK);
		return;
	}
}

void D3D11Engine::InitViewport(D3D11_VIEWPORT& vp)
{
	vp.Width = m_windowWidth;
	vp.Height = m_windowHeight;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
}

void D3D11Engine::InitDepthStencil()
{
	HRESULT hr;

	//Create depth stencil view + texture
	D3D11_DEPTH_STENCIL_DESC dsd = {};
	dsd.DepthEnable = TRUE;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D11_COMPARISON_LESS;
	hr = device->CreateDepthStencilState(&dsd, &dss);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create depth stencil state!", L"Error", MB_OK);
		return;
	}
	context->OMSetDepthStencilState(dss.Get(), 1u);

	D3D11_TEXTURE2D_DESC dstd = {};
	dstd.Width = m_windowWidth - 16u;	//offsets
	dstd.Height = m_windowHeight - 39u;	//because of window borders
	dstd.MipLevels = 1;
	dstd.ArraySize = 1;
	dstd.Format = DXGI_FORMAT_D32_FLOAT; //DXGI_FORMAT_D24_UNORM_S8_UINT;
	dstd.SampleDesc.Count = 1;
	dstd.SampleDesc.Quality = 0;
	dstd.Usage = D3D11_USAGE_DEFAULT;
	dstd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	hr = device->CreateTexture2D(&dstd, NULL, &dst);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create depth stencil texture!", L"Error", MB_OK);
		return;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
	dsvd.Format = DXGI_FORMAT_D32_FLOAT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(dst.Get(), &dsvd, &dsv);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create depth stencil view!", L"Error", MB_OK);
		return;
	}
}

void D3D11Engine::InitShadersAndInputLayout()
{
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3DBlob> 
		vsBlob, psBlob, errorBlob,
		dpsBlob, csBlob,			//Deferred
		hsBlob, dsBlob, dspBlob,	//Tessellation (new: dspBlob for pov domain shader)
		pspBlob;					//new: pspBlob for pov pixel shader

	/****************************
	//////READ SHADER FILES//////
	****************************/
	hr = D3DReadFileToBlob(L"../x64/Debug/VertexShader.cso", &vsBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to read vertex shader!", L"Error", MB_OK);
	}
	hr = D3DReadFileToBlob(L"../x64/Debug/PixelShader.cso", &psBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to read pixel shader!", L"Error", MB_OK);
	}
	//new
	hr = D3DReadFileToBlob(L"../x64/Debug/PixelShaderPov.cso", &pspBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to read pixel shader pov!", L"Error", MB_OK);
	}
	hr = D3DReadFileToBlob(L"../x64/Debug/DeferredPixelShader.cso", &dpsBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to read deferred pixel shader!", L"Error", MB_OK);
	}

	hr = D3DCompileFromFile(L"ComputeShader.hlsl", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "cs_5_0", D3DCOMPILE_DEBUG, 0, &csBlob, &errorBlob);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to read compute shader!", L"Error", MB_OK);
		return;
	}

	hr = D3DReadFileToBlob(L"../x64/Debug/HullShader.cso", &hsBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to read hull shader!", L"Error", MB_OK);
	}

	hr = D3DReadFileToBlob(L"../x64/Debug/DomainShader.cso", &dsBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to read domain shader!", L"Error", MB_OK);
	}

	//new
	hr = D3DReadFileToBlob(L"../x64/Debug/DomainShaderPov.cso", &dspBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to read domain shader pov!", L"Error", MB_OK);
	}

	/****************************
	//CREATE SHADERS FROM FILES//
	****************************/
	hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &vertexShader);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create vertex shader!", L"Error", MB_OK);
		return;
	}
	hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), NULL, &pixelShader);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create pixel shader!", L"Error", MB_OK);
		return;
	}
	hr = device->CreatePixelShader(dpsBlob->GetBufferPointer(), dpsBlob->GetBufferSize(), NULL, &deferredPixelShader);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create deferred pixel shader!", L"Error", MB_OK);
		return;
	}
	//new
	hr = device->CreatePixelShader(pspBlob->GetBufferPointer(), pspBlob->GetBufferSize(), NULL, &pixelShaderPov);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create pixel shader pov!", L"Error", MB_OK);
		return;
	}
	hr = device->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), NULL, &computeShader);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create compute shader!", L"Error", MB_OK);
		return;
	}
	hr = device->CreateHullShader(hsBlob->GetBufferPointer(), hsBlob->GetBufferSize(), NULL, &hullShader);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create hull shader!", L"Error", MB_OK);
		return;
	}
	hr = device->CreateDomainShader(dsBlob->GetBufferPointer(), dsBlob->GetBufferSize(), NULL, &domainShader);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create domain shader!", L"Error", MB_OK);
		return;
	}
	//new
	hr = device->CreateDomainShader(dspBlob->GetBufferPointer(), dspBlob->GetBufferSize(), NULL, &domainShaderPov);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create domain shader pov!", L"Error", MB_OK);
		return;
	}


	//Create Input Layout using data from our vsBlob
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[3] =
	{
	  { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	  { "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	  { "NOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create input layout!", L"Error", MB_OK);
		return;
	}
}

void D3D11Engine::InitCamera()
{
	m_camera = std::make_unique<Camera>();
	m_camera->UpdateViewMatrix();
	m_camera->InitConstantBufferAndFrustum(device.Get());
}

void D3D11Engine::InitUAV()
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to get backbuffer!", L"Error", MB_OK);
	}

	hr = device->CreateUnorderedAccessView(backBuffer.Get(), NULL, uav.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create unordered access view!", L"Error", MB_OK);
	}
}

void D3D11Engine::InitGraphicsBuffer(GBuffer(&gbuf)[4])
{
	HRESULT hr;

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = m_windowWidth - 16u;	//offsets taking into account window borders
	textureDesc.Height = m_windowHeight - 39u;	//
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	for (UINT i = 0; i < 4; i++) //Create texture(s)
	{
		hr = device->CreateTexture2D(&textureDesc, NULL, &gbuf[i].texture);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Failed to create gbuffer texture!", L"Error", MB_OK);
		}
	}

	for (UINT i = 0; i < 4; i++) //Create RTV(s), used to write geometric data to texture
	{
		hr = device->CreateRenderTargetView(gbuf[i].texture.Get(), NULL, &gbuf[i].rtv); //hr = device->CreateRenderTargetView(gBuffer[i].texture, &renderTargetViewDesc, &gBuffer[i].rtv);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Failed to create gbuffer rtv!", L"Error", MB_OK);
		}
	}

	for (UINT i = 0; i < 4; i++) //Create SRV(s), used to read the data from RTV(s)
	{
		hr = device->CreateShaderResourceView(gbuf[i].texture.Get(), NULL, &gbuf[i].srv); //hr = device->CreateShaderResourceView(gBuffer[i].texture, &shaderResourceViewDesc, &gBuffer[i].srv);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Failed to create gbuffer srv!", L"Error", MB_OK);
		}
	}
}

void D3D11Engine::InitSpotlights()
{
	//Going to write a bunch of comments here because I'm awful at visualizing 3D by just looking at numbers and pi angles
	std::vector<LightData> dataVec;

	//Spotlight in the far left corner angled further in that direction shining light onto boxes
	LightData data;
	data.pos = XMFLOAT3(0.0f, 0.0f, 5.0f);
	data.fovY = XM_PI / 4.0f;
	data.rotX = -XM_PIDIV4;
	data.rotY = XM_PI / 8.0f;
	data.col = XMFLOAT3(1.0f, 1.0f, 1.0f);
	dataVec.push_back(data);

	//Spotlight angled out into the upper corner from the near right side of the room
	LightData data2;
	data2.pos = XMFLOAT3(5.0f, -7.0f, -25.0f);
	data2.fovY = XM_PI / 4.0f;
	data2.rotX = XM_PI;
	data2.rotY = -XM_PI / 8.0f;
	data2.col = XMFLOAT3(1.0f, 1.0f, 1.0f);
	dataVec.push_back(data2);

	//Spotlight angled straight down in the near left corner of the room
	LightData data3;
	data3.pos = XMFLOAT3(-8.0f, 4.0f, -30.0f);
	data3.fovY = XM_PI / 4.0f;
	data3.rotX = 0.0f;
	data3.rotY = XM_PIDIV2;
	data3.col = XMFLOAT3(1.0f, 1.0f, 1.0f);
	dataVec.push_back(data3);

	//Directional light
	LightData data4;
	data4.pos = XMFLOAT3(-80.0f, 1.0f, -6.0f);
	data4.fovY = 0.0f;
	data4.rotX = XM_PIDIV2;
	data4.rotY = XM_PI / 8.0f;
	data4.col = XMFLOAT3(0.25f, 0.25f, 0.25f); //Since directional lights don't fall off with distance, we manually lower the strength of the light here (got bright white walls earlier)
	dataVec.push_back(data4);

	m_spotlights = SpotLights(device.Get(), dataVec);
}

DirectX::XMFLOAT3& D3D11Engine::ClosestPointOnBox(const DirectX::XMFLOAT3& point, const DirectX::BoundingBox& box)
{
	//This function should serve to help me figure out where to push out when collisions happen against a box
	DirectX::XMFLOAT3 toReturn;

	toReturn.x = max((box.Center.x - box.Extents.x), min(point.x, box.Center.x + box.Extents.x));
	toReturn.y = max((box.Center.y - box.Extents.y), min(point.y, box.Center.y + box.Extents.y));
	toReturn.z = max((box.Center.z - box.Extents.z), min(point.z, box.Center.z + box.Extents.z));

	return toReturn;
}

void D3D11Engine::MovePlayer(const DirectX::XMFLOAT3& toMove)
{
	//Move the bounding volume instead of the camera itself
	m_camera->SetBoundingSphereCenter(toMove);

	//Check for collision (Sphere vs AABB right now, add more later if needed)
	bool didCollide = false;
	for (auto drawable : m_drawables)
	{
		if (drawable.IsActive() != true) //Skip checking for collisions against inactive drawables
		{
			continue;
		}
		//Get which corner of the box is closest to the sphere
		XMFLOAT3 closest = ClosestPointOnBox(m_camera->GetBoundingSphere().Center, drawable.GetBoundingBox());

		//Get the distance from that corner to the center of the sphere
		XMFLOAT3 dist = XmFloat3Subtract(m_camera->GetBoundingSphere().Center, closest);

		//If that distance is lesser than the radius of the sphere, there's an intersection
		if (XmFloat3Length(dist) < m_camera->GetBoundingSphere().Radius)
		{
			didCollide = true;

			//Normalize the distance to get a temporary directional vector
			XMFLOAT3 push = XmFloat3Norm(dist);

			//Multiply this vector by the radius of the sphere (how far we need to push out)
			XMFLOAT3 pushMultRad = XmFloat3Multiply(push, m_camera->GetBoundingSphere().Radius);

			//Add the direction of the closest point to walk along that vector
			XMFLOAT3 newPos = XmFloat3Add(closest, pushMultRad);

			//Adjust for camera y-offset
			newPos.y += 9.0f;

			//Update positions
			m_camera->SetPosition(newPos);
			m_camera->SetBoundingSphereCenter(newPos);
		}
	}

	//If no collision happened, just move by updating the camera position with the temp position
	if (!didCollide)
		m_camera->SetPosition(toMove);
}

void D3D11Engine::InitSampler()
{
	D3D11_SAMPLER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;
	desc.MipLODBias = 0;
	desc.MaxAnisotropy = 1;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	HRESULT hr = device->CreateSamplerState(&desc, samplerState.GetAddressOf());

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create sampler!", L"Error", MB_OK);
	}
}

bool D3D11Engine::InitEntityGraphics(EntityID& entity, DirectX::XMFLOAT3 translate)
{
	//So this function no longer leads to any errors, but let's see if we can actually draw the cube ahaA

	//Stuff to pass along
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::string ambientData = "Textures/dark_grey.png";
	std::string diffuseData = "Textures/grey.png";
	std::string specularData = "Textures/light_grey.png";
	float shineData = 1.0f;

	//Hard-code cube vertices and indices for now (blegh)
	Vertex verts[] = 
	{
		{ {-1.0f, -1.0f, -1.0f}, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } }, //
		{ { 1.0f, -1.0f, -1.0f}, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
		{ {-1.0f,  1.0f, -1.0f}, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
		{ { 1.0f,  1.0f, -1.0f}, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },

		{ {-1.0f, -1.0f,  1.0f}, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
		{ { 1.0f, -1.0f,  1.0f}, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
		{ {-1.0f,  1.0f,  1.0f}, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
		{ { 1.0f,  1.0f,  1.0f}, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
	};
	
	for (int i = 0; i < 8; i++)
	{
		vertices.push_back(verts[i]);
	}

	uint32_t inds[] =
	{
		0,2,1,	2,3,1,
		1,3,5,	3,7,5,
		2,6,3,	3,6,7,
		4,5,7,	4,7,6,
		0,4,2,	2,4,6,
		0,1,4,	1,5,4
	};

	for (int i = 0; i < 36; i++)
	{
		indices.push_back(inds[i]);
	}

	//Put all the stuff into bufferdata
	BufferData bufferData;
	bufferData.vData.size = sizeof(Vertex);
	bufferData.vData.count = (UINT)vertices.size();
	bufferData.vData.vector = vertices;

	bufferData.iData.size = sizeof(uint32_t);
	bufferData.iData.count = (UINT)indices.size();
	bufferData.iData.vector = indices;

	bufferData.mData.ambient = ambientData;
	bufferData.mData.diffuse = diffuseData;
	bufferData.mData.specular = specularData;
	bufferData.mData.shininess = shineData;

	//Create graphicscomponent passing in bufferdata and pray
	m_registry.AddComponent<GraphicsComponent>(entity, device.Get(), bufferData, translate);

	return true;
}