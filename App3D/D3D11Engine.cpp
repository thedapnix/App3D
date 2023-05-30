#include "ImGuiHelper.h" //Which in turn includes D3D11Engine.h

#include <dxgi.h>			//directx driver interface
#include <d3dcompiler.h>	//compiling shaders

using namespace DirectX;

D3D11Engine::D3D11Engine(const HWND& hWnd, const UINT& width, const UINT& height)
{
	m_windowWidth = width;
	m_windowHeight = height;

	//Base d3d setup
	InitInterfaces(hWnd);
	InitViewport();
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
	//m_cubeMap = CubeMap(device.Get(), false); //Complains that the copy-assignment operator has been deleted but I haven't done that?
	m_cubeMap.Init(device.Get(), true); //Workaround, though I prefer having working constructors and operators

	//Shadowmap setup
	InitSpotlights();
	m_shadowMap = ShadowMap(device.Get(), &m_drawables, &m_spotlights);

	//srand((unsigned)time(NULL));
	//for (int i = 0; i < 20; i++)
	//{
	//	//Generate random numbers between -+10
	//	/*int r1 = -10 + (rand() % 20);
	//	int r2 = -10 + (rand() % 20);
	//	int r4 = -10 + (rand() % 20);
	//	int r5 = -10 + (rand() % 20);
	//	int r6 = -10 + (rand() % 20);*/
	//}
	
	//Drawable setup
	//for (int i = 0; i < 5; i++)
	//{
	//	for (int j = 0; j < 5; j++)
	//	{
	//		InitDrawableFromFile("Models/cube3.obj", "Textures/brick_wall.jpg", m_drawables, { 0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f, 0.0f }, { -5.0f + (float)i * 3, -5.0f + (float)j * 3, 5.0f });
	//		//InitCube("Textures/icon.png", m_drawables, { 0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f, 0.0f }, { -5.0f + (float)i*3, -5.0f + (float)j*3, 5.0f});
	//	}
	//}

	//Ground
	InitDrawableFromFile("Meshes/cube.obj", "Textures/gravel.png", m_drawables, {14.0f, 1.0f, 14.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, -10.0f, 5.0f});

	//Back wall
	InitDrawableFromFile("Meshes/cube.obj", "Textures/brick_wall.png", m_drawables, { 15.0f, 5.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, -4.0f, 19.0f });

	//Left and right wall
	InitDrawableFromFile("Meshes/cube.obj", "Textures/brick_wall.png", m_drawables, { 1.0f, 5.0f, 14.0f }, { 0.0f, 0.0f, 0.0f }, { -14.0f, -4.0f, 4.0f });
	InitDrawableFromFile("Meshes/cube.obj", "Textures/brick_wall.png", m_drawables, { 1.0f, 5.0f, 14.0f }, { 0.0f, 0.0f, 0.0f }, {  14.0f, -4.0f, 4.0f });

	//Reflective drawable
	InitDrawableFromFile("Meshes/cube.obj", "Textures/dog.png", m_reflectiveDrawables, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, -2.5f, 5.0f }); //The internal cubemap camera is at (0, 0, -5) so match

	//Ability to sample from textures in a shader-file
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
	/*Update constant buffers*/
	m_camera->UpdateConstantBuffer(context.Get());

	for (auto& drawable : m_drawables)
	{
		//apply whatever changes should happen per tick
		drawable.UpdateConstantBuffer(context.Get());
	}

	for (auto& mirror : m_reflectiveDrawables)
	{
		mirror.RotateY(0.005f);
		mirror.UpdateConstantBuffer(context.Get());
	}

	/*Render*/
	Render(dt, rtv.Get(), dsv.Get(), &viewport, m_camera.get(), CLEAR_COLOR);
	if (billboardingIsEnabled) RenderParticles(m_camera.get());
	if (cubemapIsEnabled)RenderReflectiveObject(dt);
	if (shadowmapIsEnabled)RenderDepth(dt);

	/*Present the final scene*/
	swapChain->Present(1, 0); //vSync enabled
}

void D3D11Engine::ImGuiSceneData(D3D11Engine* d3d11engine, bool shouldUpdateFps, int state)
{
	StartImGuiFrame();
	m_fpsCounter++;
	if (shouldUpdateFps)
	{
		m_fpsString = std::to_string(m_fpsCounter);
		m_fpsCounter = 0;
	}
	ImGuiEngineWindow(
		m_camera.get(), m_fpsString, state,
		objIsEnabled, deferredIsEnabled, cullingIsEnabled, billboardingIsEnabled, lodIsEnabled, cubemapIsEnabled, shadowmapIsEnabled,
		m_drawablesBeingRendered
	);
	EndImGuiFrame();
}

void D3D11Engine::MovePlayer(float speed)
{
	//Right now we should have 2 drawables, one at [0] which is the ground, and one at [1] that we consider to be the player. This is the biggest of temp solutions
	//m_drawables.at(1).EditTranslation(speed, 0.0f, 0.0f);
}

Camera& D3D11Engine::GetCamera() const noexcept
{
	return *m_camera;
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

		/*Input Assembler Stage*/
		//Set primitive topology and input layout
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST); //farewell D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
		context->IASetInputLayout(inputLayout.Get());

		/*Shader Stage*/
		context->VSSetShader(vertexShader.Get(), NULL, 0);
		context->PSSetShader(pixelShader.Get(), NULL, 0);
		//context->PSSetSamplers(0, 1, samplerState.GetAddressOf());

		//Tessellation
		if(lodIsEnabled)context->RSSetState(wireframeRS.Get());
		else			context->RSSetState(regularRS.Get());
		context->HSSetShader(hullShader.Get(), NULL, 0);
		context->DSSetShader(domainShader.Get(), NULL, 0);
		context->DSSetConstantBuffers(0, 1, cam->GetConstantBuffer().GetBufferAddress()); //m_cameraCB.GetBufferAddress()
		context->HSSetConstantBuffers(0, 1, cam->GetConstantBuffer().GetBufferAddress());

		/*Culling*/
		if (cullingIsEnabled)
		{
			int visibleDrawables = 0;
			for (auto& drawable : m_drawables)
			{
				if (DrawableIsVisible(cam->GetFrustum(), drawable.GetBoundingBox(), m_camera->View(), drawable.World()))
				{
					drawable.Bind(context.Get(), NULL);
					drawable.Draw(context.Get());
					visibleDrawables++;
				}
				m_drawablesBeingRendered = visibleDrawables;
			}
		}
		else
		{
			//Per drawable: bind vertex and index buffers, then draw them
			for (auto& drawable : m_drawables)
			{
				drawable.Bind(context.Get(), NULL);
				drawable.Draw(context.Get());
			}
			m_drawablesBeingRendered = (int)m_drawables.size();
		}

		/*Unbind shaders before we move on to particle drawing*/
		context->VSSetShader(NULL, NULL, 0);
		context->PSSetShader(NULL, NULL, 0);
		context->HSSetShader(NULL, NULL, 0);
		context->DSSetShader(NULL, NULL, 0);

		/*Unbind constant buffers too*/
		context->HSSetConstantBuffers(0, 0, NULL);
		context->DSSetConstantBuffers(0, 0, NULL);

		/*Aaaand shader resource stuff*/
		ID3D11RenderTargetView* nullRTV = NULL;
		context->OMSetRenderTargets(1, &nullRTV, NULL);
		ID3D11ShaderResourceView* nullSRV = NULL;
		context->PSSetShaderResources(0, 0, &nullSRV);
		ID3D11SamplerState* nullSampler = NULL;
		context->PSSetSamplers(0, 0, &nullSampler);
	}

	if (deferredIsEnabled)
	{
		DefPassTwo(); //Lighting pass, editing the backbuffer using a compute shader
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
	context->GSSetShader(m_particles.GetGeometryShader(), NULL, 0);
	context->GSSetConstantBuffers(0, 1, cam->GetConstantBuffer().GetBufferAddress());
	context->PSSetShader(m_particles.GetPixelShader(), NULL, 0);

	//Draw
	context->Draw(m_particles.GetElementCount(), 0);

	//Unbind shaders and constant buffer
	context->VSSetShader(NULL, NULL, 0);
	context->GSSetShader(NULL, NULL, 0);
	context->GSSetConstantBuffers(0, 0, NULL);
	context->PSSetShader(NULL, NULL, 0);

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
		drawable.Bind(context.Get(), m_cubeMap.GetShaderResourceView());
		drawable.Draw(context.Get());
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

void D3D11Engine::RenderDepth(float dt)
{
	/*Bind stuff*/
	context->IASetInputLayout(inputLayout.Get());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->VSSetShader(m_shadowMap.GetVertexShader(), NULL, 0);
	context->RSSetViewports(1, m_shadowMap.GetViewport());

	/*Render depth from the perspective of every light*/
	for (UINT i = 0; i < m_spotlights.size(); i++)
	{
		ID3D11DepthStencilView* dsView = m_shadowMap.GetDepthStencilViewAt(i);
		context->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH, 1, 0);
		context->OMSetRenderTargets(0, NULL, dsView); //no rtv, only dsv

		//Some way of getting the camera cb of individual lights
		ID3D11Buffer* cameraCB = m_spotlights.at(i).GetCameraConstantBuffer().GetBuffer();
		context->VSSetConstantBuffers(1, 1, &cameraCB);

		for (auto& drawable : m_drawables)
		{
			drawable.Bind(context.Get(), NULL);
			drawable.Draw(context.Get());
		}
	}

	/*Unbind stuff*/
	context->OMSetRenderTargets(0, NULL, NULL);
	context->VSSetShader(NULL, NULL, 0);
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
	context->ClearDepthStencilView(dsv.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0.0f);
	context->RSSetViewports(1, &viewport);
	ID3D11RenderTargetView* rtvArr[] = { m_gBuffers[0].rtv.Get(), m_gBuffers[1].rtv.Get(), m_gBuffers[2].rtv.Get() }; //Create an array of render target views and fill it with the rtv's from our gbuffers
	context->OMSetRenderTargets(3, rtvArr, dsv.Get()); //When render targets are bound to the output merger (if I understand correctly), they are sent to the pixel shader where they get filled with data yes?

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	context->IASetInputLayout(inputLayout.Get());

	///////////////////////////////////////////////////////////////////////////////
	//DRAW PASS, DRAW SCENE ONTO BACKBUFFER WITHOUT DOING LIGHTING CALCULATIONS
	context->VSSetShader(vertexShader.Get(), NULL, 0);
	context->PSSetShader(deferredPixelShader.Get(), NULL, 0);
	context->PSSetSamplers(0, 1, samplerState.GetAddressOf());

	/*Tessellation ting*/
	if (lodIsEnabled)context->RSSetState(wireframeRS.Get());
	else			context->RSSetState(regularRS.Get());
	context->HSSetShader(hullShader.Get(), NULL, 0);
	context->DSSetShader(domainShader.Get(), NULL, 0);
	context->DSSetConstantBuffers(0, 1, cam->GetConstantBuffer().GetBufferAddress()); //Moved from vertex shader to domain shader (move to hull shader? that's where patching happens so makes sense?)

	//context->PSSetShaderResources(0, 1, &srv);
	//context->PSSetSamplers(0, 1, &samplerState);

	//This right here is why deferred rendering is better with multiple lights, but worse with multiple drawables
	//We're drawing all our drawables onto all 3 render targets
	if (cullingIsEnabled)
	{
		int visibleDrawables = 0;
		for (auto& drawable : m_drawables)
		{
			if (DrawableIsVisible(cam->GetFrustum(), drawable.GetBoundingBox(), m_camera->View(), drawable.World()))
			{
				drawable.Bind(context.Get(), NULL);
				drawable.Draw(context.Get());
				visibleDrawables++;
			}
			m_drawablesBeingRendered = visibleDrawables;
		}
	}
	else
	{
		//Per drawable: bind vertex and index buffers, then draw them
		for (auto& drawable : m_drawables)
		{
			drawable.Bind(context.Get(), NULL);
			drawable.Draw(context.Get());
		}
		m_drawablesBeingRendered = (int)m_drawables.size();
	}

	//Now that we're done writing data to the render targets, unbind them
	rtvArr[0] = NULL;
	rtvArr[1] = NULL;
	rtvArr[2] = NULL;
}

void D3D11Engine::DefPassTwo()
{
	///////////////////////////////////////////////////////////////////////////////
	//LIGHTING PASS, USE COMPUTE SHADER TO EDIT THE BACKBUFFER AND DO LIGHTING COMPUTATIONS
	//Looking at it with a more experienced eye, I'd say no. We're now no longer rendering, more like using the compute shader to edit the final image
	ID3D11RenderTargetView* nullRtv = NULL;
	context->OMSetRenderTargets(1, &nullRtv, NULL);

	ID3D11ShaderResourceView* srvArr[] = { m_gBuffers[0].srv.Get(), m_gBuffers[1].srv.Get(), m_gBuffers[2].srv.Get() };
	context->CSSetShaderResources(0, 3, srvArr);

	//Use compute shader to edit the backbuffer
	context->CSSetShader(computeShader.Get(), NULL, 0);
	context->CSSetUnorderedAccessViews(0, 1, uav.GetAddressOf(), NULL); //Last value matters in case of "append consume" buffers, but I've only heard of that, I've *no idea* what it means
	context->Dispatch(m_windowWidth / 8, m_windowHeight / 8, 1);		//In order to make our Dispatch cover the entire window, we group threads by the width and height of the 
	//window divided by 8 (as 8x8 is defined in compute shader)

	//Unbind UAV
	ID3D11UnorderedAccessView* nullUav = NULL;
	context->CSSetUnorderedAccessViews(0, 1, &nullUav, NULL);

	//Unbind SRVs
	srvArr[0] = NULL;
	srvArr[1] = NULL;
	srvArr[2] = NULL;
	context->CSSetShaderResources(0, 3, srvArr);
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

void D3D11Engine::InitViewport()
{
	viewport.Width = m_windowWidth;
	viewport.Height = m_windowHeight;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
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
		hsBlob, dsBlob;				//Tessellation

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
	hr = D3DReadFileToBlob(L"../x64/Debug/DeferredPixelShader.cso", &dpsBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to read deferred pixel shader!", L"Error", MB_OK);
	}

	hr = D3DCompileFromFile(L"ComputeShader.hlsl", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "cs_5_0", D3DCOMPILE_DEBUG, 0, &csBlob, &errorBlob); //This works but D3DReadFileToBlob doesn't. oof ig
	//hr = D3DReadFileToBlob(L"../64/Debug/ComputeShader.cso", &csBlob);

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

void D3D11Engine::InitGraphicsBuffer(GBuffer(&gbuf)[3])
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

	for (UINT i = 0; i < 3; i++) //Create texture(s)
	{
		hr = device->CreateTexture2D(&textureDesc, NULL, &gbuf[i].texture);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Failed to create gbuffer texture!", L"Error", MB_OK);
		}
	}

	for (UINT i = 0; i < 3; i++) //Create RTV(s), used to write geometric data to texture
	{
		hr = device->CreateRenderTargetView(gbuf[i].texture.Get(), NULL, &gbuf[i].rtv); //hr = device->CreateRenderTargetView(gBuffer[i].texture, &renderTargetViewDesc, &gBuffer[i].rtv);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Failed to create gbuffer rtv!", L"Error", MB_OK);
		}
	}

	for (UINT i = 0; i < 3; i++) //Create SRV(s), used to read the data from RTV(s)
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
	LightData data;
	data.pos = XMFLOAT3(0.0f, 0.0f, -10.0f);
	data.fovY = XM_PI / 4.0f;
	data.rotX = 0.0f;
	data.rotY = 0.0f;
	data.col = XMFLOAT3(1.0f, 1.0f, 1.0f);

	m_spotlights.push_back(SpotLight(device.Get(), data));
}

bool D3D11Engine::DrawableIsVisible(DirectX::BoundingFrustum frustum, DirectX::BoundingBox aabb, DirectX::XMMATRIX view, DirectX::XMMATRIX world)
{
	//The frustum is in view space, and the AABB is in the local space of the object it wraps
	//So the frustum needs to be multiplied by the inverse view- (which we get from the camera) and world-matrices (of the particular object) to take it to the object's local space

	//Get determinants and the the inverses of the view- and world-matrices
	DirectX::XMVECTOR detView = DirectX::XMMatrixDeterminant(view);
	DirectX::XMVECTOR detWorld = DirectX::XMMatrixDeterminant(world);

	DirectX::XMMATRIX invView = DirectX::XMMatrixInverse(&detView, view);
	DirectX::XMMATRIX invWorld = DirectX::XMMatrixInverse(&detWorld, world);

	//Create the matrix that transforms from view space to the local space of the object
	DirectX::XMMATRIX viewToLocal = DirectX::XMMatrixMultiply(invView, invWorld);

	DirectX::BoundingFrustum localFrustum = frustum;	//Copy the input frustum so as to not modify the original
	localFrustum.Transform(localFrustum, viewToLocal);	//Transform the copied frustum so it's in the same space as the object we're testing it against

	return localFrustum.Intersects(aabb);
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

bool D3D11Engine::InitDrawableFromFile(std::string objFileName, std::string textureFileName, std::vector<Drawable>& vecToFill, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotate, DirectX::XMFLOAT3 translate)
{
	std::ifstream ifs(objFileName);
	if (!ifs)
	{
		MessageBox(NULL, L"Couldn't read obj file name!", L"Error", MB_OK);
		return false;
	}

	//Member variables to store positions, texture coordinates and normals
	std::vector<XMFLOAT3> vPos;
	std::vector<XMFLOAT2> vTex;
	std::vector<XMFLOAT3> vNor;

	//Error handling for faces (preferably every .obj file has v, vt and vn, but not all .obj files were made equal)
	bool fileHasTexcoord = false;
	bool fileHasNormal = false;

	//Wrap the mesh in a bounding box by getting the highest and lowest x-, y-, and z-values (use a library for actual infinities xd)
	XMFLOAT3 vMinf3(1000000.0f, 1000000.0f, 1000000.0f);
	XMFLOAT3 vMaxf3(-1000000.0f, -1000000.0f, -1000000.0f);

	XMVECTOR vMin = DirectX::XMLoadFloat3(&vMinf3);
	XMVECTOR vMax = DirectX::XMLoadFloat3(&vMaxf3);

	/*Pass along to drawable*/
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	UINT vCount = 0u;
	UINT iCount = 0u;

	//Read the text file
	std::string lineStr;
	while (std::getline(ifs, lineStr))
	{
		std::istringstream lineSS(lineStr);
		std::string lineType;
		lineSS >> lineType;

		if (lineType == "v")
		{
			float x, y, z;
			lineSS >> x >> y >> z;
			vPos.push_back({ x,y,z });

			//Check vMin and vMax against the x-, y-, and z-values of the vertex, once we've looped through everything we should have the edges of our box
			XMFLOAT3 vf3(x, y, z);
			XMVECTOR V = DirectX::XMLoadFloat3(&vf3);
			vMin = DirectX::XMVectorMin(vMin, V);
			vMax = DirectX::XMVectorMax(vMax, V);
		}

		if (lineType == "vt")
		{
			float u, v;
			lineSS >> u >> v;
			vTex.push_back({ u,v });
			fileHasTexcoord = true;
		}

		if (lineType == "vn")
		{
			float nx, ny, nz;
			lineSS >> nx >> ny >> nz;
			vNor.push_back({ nx, ny, nz });
			fileHasNormal = true;
		}

		if (lineType == "f")
		{
			//Each face references three vertices, with their respective positions, texture coordinates and normals (think triangles)
			//Some obj-files simply have something like "f 2 5 3", while others are built more like "f 2/1/0 5/3/1 3/2/2"
			//We write the following code in a way that works with both of these formats
			std::vector<VertexReference> refs;
			std::string refStr;
			while (lineSS >> refStr)
			{
				std::istringstream ref(refStr);
				std::string vStr, vtStr, vnStr;

				//Split the line at every / (this is the point where we know what kind of obj-file we're reading, as mentioned above)
				std::getline(ref, vStr, '/');
				std::getline(ref, vtStr, '/');
				std::getline(ref, vnStr, '/');

				//Convert to integer
				int v = atoi(vStr.c_str());
				int vt = atoi(vtStr.c_str());
				int vn = atoi(vnStr.c_str());

				//Error checking in case of negative references (shouldn't be necessary as long as the .obj-file isn't trash but I did have to deal with it one time)
				if (v < 0) v = (int)vPos.size() + v;
				if (vt < 0) vt = (int)vTex.size() + vt;
				if (vn < 0) vn = (int)vNor.size() + vn;

				//Subtract by one because the "faces" part of an obj-file starts counting from 1, but we want to count from 0
				indices.push_back(v - 1);
				refs.push_back({ v - 1 , vt - 1 , vn - 1 });
				iCount++;
				//vCountMesh++;
			}

			//We now have all the information we need to construct our vertices
			for (size_t i = 1; i + 1 < refs.size(); ++i)
			{
				//Get the actual values of the vertices referenced by the face
				const VertexReference* p[3] = { &refs[0], &refs[i], &refs[i + 1] };

				//If we haven't gotten normals from the .obj-file we calculate them manually
				//If we already have them, skip this step so as to not waste processing power on unnecessary calculations
				XMVECTOR faceNormal;
				if (!fileHasNormal)
				{
					XMVECTOR U = XMVectorSubtract(XMLoadFloat3(&vPos[p[1]->v]), XMLoadFloat3(&vPos[p[0]->v]));
					XMVECTOR V = XMVectorSubtract(XMLoadFloat3(&vPos[p[2]->v]), XMLoadFloat3(&vPos[p[0]->v]));
					faceNormal = XMVector3Cross(U, V);
					faceNormal = XMVector3Normalize(faceNormal);
				}

				for (size_t j = 0; j < 3; ++j)
				{
					Vertex vert;
					vert.position = vPos[p[j]->v];
					vert.texcoord = (fileHasTexcoord ? vTex[p[j]->vt] : XMFLOAT2({0.0f, 0.0f}));
					if (fileHasNormal) vert.normal = vNor[p[j]->vn]; else XMStoreFloat3(&vert.normal, faceNormal); //Ternary operator replaced with if-else since we're having different return values

					vertices.push_back(vert);
					vCount++;
				}
			}
		}
	}

	//Before we return out of this function, we store the values that we've now received to make a bounding box
	//Either pass into the function, or perhaps more fittingly, create a new function in the Drawable-class that calls CreateFromPoints() to make its own aabb
	//DirectX::BoundingBox::CreateFromPoints(aabb, vMin, vMax);

	BufferData bufferData;
	bufferData.vData.size = sizeof(Vertex);
	bufferData.vData.count = vCount;
	bufferData.vData.vector = vertices;

	bufferData.iData.size = sizeof(uint32_t);
	bufferData.iData.count = iCount;
	bufferData.iData.vector = indices;

	bufferData.textureFileName = textureFileName;

	Drawable cube(device.Get(), bufferData, scale, rotate, translate);
	cube.CreateBoundingBoxFromPoints(vMin, vMax);
	vecToFill.push_back(cube);

	return true;
}
