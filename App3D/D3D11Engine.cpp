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

	InitGraphicsBuffer(m_gBuffers); //deferred

	//Render setup
	InitShadersAndInputLayout();

	//Camera setup (matrices and constant buffer)
	InitCamera();

	InitRasterizerStates(); //tesselation helper

	//Init all our drawables
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
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			InitDrawableFromFile("Models/cube.obj", { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { -5.0f + (float)i*3, -5.0f + (float)j*3, 5.0f});
		}
	}

	SetupImGui(hWnd, device.Get(), context.Get());
}

D3D11Engine::~D3D11Engine()
{
	ClearImGui();
}

void D3D11Engine::Update(float dt)
{
	/*Update constant buffers*/
	DirectX::XMStoreFloat4x4(&m_viewProj.view, XMMatrixTranspose(m_camera->View()));
	DirectX::XMStoreFloat4x4(&m_viewProj.proj, XMMatrixTranspose(m_camera->Proj()));
	UpdateConstantBuffer(m_cameraCB.GetBuffer(), &m_viewProj, sizeof(m_viewProj));

	for (auto& drawable : m_drawables)
	{
		//apply whatever changes should happen per tick
		drawable.UpdateConstantBuffer(context.Get());
	}

	Render(dt);

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
	ImGuiEngineWindow(m_camera.get(), m_fpsString, state,
		objIsEnabled, deferredIsEnabled, cullingIsEnabled, billboardingIsEnabled, lodIsEnabled, cubemapIsEnabled, shadowIsEnabled,
		m_drawablesBeingRendered);
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
void D3D11Engine::Render(float dt)
{
	/*Update buffers and camera frustum here*/

	if (deferredIsEnabled)
	{
		DefPassOne(); //Does the same as what's in the else-statement, except to several rendertargets
	}
	else
	{
		// Clear the back buffer and depth stencil, as well as set viewport and render target (viewport only really needed to set after a resize, and that's disabled so uh)
		context->ClearRenderTargetView(rtv.Get(), CLEAR_COLOR);
		context->ClearDepthStencilView(dsv.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
		context->RSSetViewports(1, &viewport);
		context->OMSetRenderTargets(1, rtv.GetAddressOf(), dsv.Get());

		/*Input Assembler Stage*/
		//Set primitive topology and input layout
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(inputLayout.Get());

		/*Tessellation ting*/
		if (lodIsEnabled)
		{
			currentRS = wireframeRS;
			context->RSSetState(currentRS.Get());
			context->HSSetShader(hullShader.Get(), NULL, 0);
			context->DSSetShader(domainShader.Get(), NULL, 0);
		}
		else
		{
			currentRS = regularRS;
			context->RSSetState(currentRS.Get());
		}

		/*Vertex Shader Stage*/
		context->VSSetConstantBuffers(0, 1, m_cameraCB.GetBufferAddress());
		context->VSSetShader(vertexShader.Get(), NULL, 0);

		/*Pixel Shader Stage*/
		context->PSSetShader(pixelShader.Get(), NULL, 0);
		//this->context->PSSetSamplers(0, 1, samplerState.GetAddressOf());

		/*Culling*/
		if (cullingIsEnabled)
		{
			int visibleDrawables = 0;
			for (auto& drawable : m_drawables)
			{
				if (DrawableIsVisible(m_frustum, drawable.GetBoundingBox(), m_camera->View(), drawable.World()))
				{
					drawable.Bind(context.Get());
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
				drawable.Bind(context.Get());
				drawable.Draw(context.Get());
			}
			m_drawablesBeingRendered = (int)m_drawables.size();
		}
	}

	if (deferredIsEnabled)
	{
		DefPassTwo(); //Lighting pass, editing the backbuffer using a compute shader
	}
	
}

void D3D11Engine::DefPassOne()
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

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(inputLayout.Get());

	///////////////////////////////////////////////////////////////////////////////
	//DRAW PASS, DRAW SCENE ONTO BACKBUFFER WITHOUT DOING LIGHTING CALCULATIONS
	context->VSSetShader(vertexShader.Get(), NULL, 0);
	context->PSSetShader(pixelShader.Get(), NULL, 0);

	//context->PSSetShaderResources(0, 1, &srv);
	//context->PSSetSamplers(0, 1, &samplerState);

	for (auto& drawable : m_drawables)	//This right here is why deferred rendering is better with multiple lights, but worse with multiple drawables
										//We're drawing all our drawables onto all 3 render targets
	{
		drawable.Bind(context.Get()); //Bind vertex and index buffers (textures todo)
		drawable.Draw(context.Get()); //Set constant buffers and perform DrawIndexed()-calls
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
	regularDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
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

	currentRS = regularRS; //Default to using regular rasterizer state, switch to wireframe through boolean
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

	hr = device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtv.GetAddressOf());
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
		csBlob,						//Deferred
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
	XMStoreFloat4x4(&m_viewProj.view, XMMatrixTranspose(m_camera->View()));
	XMStoreFloat4x4(&m_viewProj.proj, XMMatrixTranspose(m_camera->Proj()));
	m_cameraCB.Init(device.Get(), &m_viewProj, sizeof(m_viewProj));
	DirectX::BoundingFrustum::CreateFromMatrix(m_frustum, m_camera->Proj());
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

//void D3D11Engine::InitSampler()
//{
//	D3D11_SAMPLER_DESC desc;
//	ZeroMemory(&desc, sizeof(desc));
//
//	//desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
//	//desc.Filter = D3D11_FILTER_ANISOTROPIC;
//	desc.AddressU = adressMode;
//	desc.AddressV = adressMode;
//	desc.AddressW = adressMode;
//	desc.MinLOD = 0.0f;
//	desc.MaxLOD = D3D11_FLOAT32_MAX;
//	desc.MipLODBias = 0.0f;
//	desc.MaxAnisotropy = 1;
//	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
//
//	/*if (borderColour.has_value())
//	{
//		for (size_t i = 0; i < borderColour.value().size(); i++)
//		{
//			desc.BorderColor[i] = borderColour.value()[i];
//		}
//	}*/
//	HRESULT hr = device->CreateSamplerState(&desc, samplerState.GetAddressOf());
//
//	if (FAILED(hr))
//	{
//		MessageBox(NULL, L"Failed to create sampler!", L"Error", MB_OK);
//	}
//}

void D3D11Engine::InitQuad(DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotate, DirectX::XMFLOAT3 translate)
{
	std::vector<Vertex> vertices({
	{ {-1.0f,   1.0f, 0.0f}, {0.0f,0.0f}, {0.0f, 0.0f, -1.0f}}, //Top left
	{ { 1.0f,  -1.0f, 0.0f}, {1.0f,1.0f}, {0.0f, 0.0f, -1.0f}}, //Bottom right
	{ {-1.0f,  -1.0f, 0.0f}, {0.0f,1.0f}, {0.0f, 0.0f, -1.0f}}, //Bottom left
	{ { 1.0f,   1.0,  0.0f}, {1.0f,0.0f}, {0.0f, 0.0f, -1.0f}}, //Top right
	});

	BufferData bufferData;
	bufferData.vData.size = sizeof(Vertex);
	bufferData.vData.count = 4;
	bufferData.vData.vector = vertices;

	std::vector<uint32_t> indices({ 
		0, 1, 2, 
		0, 3, 1 
	});

	bufferData.iData.size = sizeof(uint32_t);
	bufferData.iData.count = 6;
	bufferData.iData.vector = indices;

	Drawable quad(device.Get(), bufferData, scale, rotate, translate);
	m_drawables.push_back(quad);

	//meshData.textureFile = "dog.png";

	/*MeshData::SubMeshInfo subMeshInfo;
	subMeshInfo.startIndexValue = 0;
	subMeshInfo.nrOfIndicesInSubMesh = 6;

	meshData.subMeshInfo.push_back(subMeshInfo);*/
}

void D3D11Engine::InitCube(DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotate, DirectX::XMFLOAT3 translate)
{
	//Ain't no way I'm doing this by hand
	std::vector<Vertex> vertices({
		//Front
		{ {-1.0f, -1.0f,  -1.0f, }, {0.0f, 0.0f,}, { 0.0f, 0.0f, -1.0f,} },
		{ { 1.0f, -1.0f,  -1.0f, }, {1.0f, 0.0f,}, { 0.0f, 0.0f, -1.0f,} },
		{ {-1.0f,  1.0f,  -1.0f, }, {0.0f, 1.0f,}, { 0.0f, 0.0f, -1.0f,} },
		{ { 1.0f,  1.0f,  -1.0f, }, {1.0f, 1.0f,}, { 0.0f, 0.0f, -1.0f,} },

		//Back
		{ {-1.0f, -1.0f,   1.0f, }, {1.0f, 0.0f,}, { 0.0f, 0.0f,  1.0f,} },
		{ { 1.0f, -1.0f,   1.0f, }, {0.0f, 0.0f,}, { 0.0f, 0.0f,  1.0f,} },
		{ {-1.0f,  1.0f,   1.0f, }, {1.0f, 1.0f,}, { 0.0f, 0.0f,  1.0f,} },
		{ { 1.0f,  1.0f,   1.0f, }, {0.0f, 1.0f,}, { 0.0f, 0.0f,  1.0f,} },

		//Right
		{ { 1.0f, -1.0f,  -1.0f, }, {0.0f, 0.0f,}, { 1.0f, 0.0f,  0.0f,} },
		{ { 1.0f, -1.0f,   1.0f, }, {1.0f, 0.0f,}, { 1.0f, 0.0f,  0.0f,} },
		{ { 1.0f,  1.0f,  -1.0f, }, {0.0f, 1.0f,}, { 1.0f, 0.0f,  0.0f,} },
		{ { 1.0f,  1.0f,   1.0f, }, {1.0f, 1.0f,}, { 1.0f, 0.0f,  0.0f,} },

		//Left
		{ {-1.0f, -1.0f,  -1.0f, }, {1.0f, 0.0f,}, {-1.0f, 0.0f,  0.0f,} },
		{ {-1.0f, -1.0f,   1.0f, }, {0.0f, 0.0f,}, {-1.0f, 0.0f,  0.0f,} },
		{ {-1.0f,  1.0f,  -1.0f, }, {1.0f, 1.0f,}, {-1.0f, 0.0f,  0.0f,} },
		{ {-1.0f,  1.0f,   1.0f, }, {0.0f, 1.0f,}, {-1.0f, 0.0f,  0.0f,} },

		//Top
		{ {-1.0f,  1.0f,  -1.0f, }, {0.0f, 0.0f,}, {0.0f, -1.0f,  0.0f,} },
		{ {-1.0f,  1.0f,   1.0f, }, {0.0f, 1.0f,}, {0.0f, -1.0f,  0.0f,} },
		{ { 1.0f,  1.0f,  -1.0f, }, {1.0f, 0.0f,}, {0.0f, -1.0f,  0.0f,} },
		{ { 1.0f,  1.0f,   1.0f, }, {1.0f, 1.0f,}, {0.0f, -1.0f,  0.0f,} },

		//Bot
		{ {-1.0f, -1.0f,  -1.0f, }, {0.0f, 1.0f,}, {0.0f,  1.0f,  0.0f,} },
		{ { 1.0f, -1.0f,  -1.0f, }, {1.0f, 1.0f,}, {0.0f,  1.0f,  0.0f,} },
		{ {-1.0f, -1.0f,   1.0f, }, {0.0f, 0.0f,}, {0.0f,  1.0f,  0.0f,} },
		{ { 1.0f, -1.0f,   1.0f, }, {1.0f, 0.0f,}, {0.0f,  1.0f,  0.0f,} },
		});

	BufferData bufferData;
	bufferData.vData.size = sizeof(Vertex);
	bufferData.vData.count = 24;
	bufferData.vData.vector = vertices;

	std::vector<uint32_t> indices({
		 0, 2, 1,  2, 3, 1, //Front
		 8,10, 9, 10,11, 9, //Right
	    16,17,18, 18,17,19,//Top
		 4, 5, 7,  4, 7, 6, //Back
		12,13,14, 14,13,15, //Left
		20,21,22, 22,21,23  //Bot
		});

	bufferData.iData.size = sizeof(uint32_t);
	bufferData.iData.count = 36;
	bufferData.iData.vector = indices;

	Drawable cube(device.Get(), bufferData, scale, rotate, translate);
	m_drawables.push_back(cube);
}

void D3D11Engine::UpdateConstantBuffer(ID3D11Buffer* cb, void* data, size_t size)
{
	D3D11_MAPPED_SUBRESOURCE mapped = {};						//Set up the new data for the resource, zero the memory
	context->Map(cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);	//Disable GPU access to the data we want to change, and get a pointer to the memory containing said data
	memcpy(mapped.pData, data, size);							//Write the new data to memory
	context->Unmap(cb, 0);										//Re-enable GPU access to the data
}

bool D3D11Engine::InitDrawableFromFile(std::string fileName, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotate, DirectX::XMFLOAT3 translate)
{
	std::ifstream ifs(fileName);
	if (!ifs)
	{
		MessageBox(NULL, L"Couldn't read file name!", L"Error", MB_OK);
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
					vert.texcoord = (fileHasTexcoord ? vTex[p[j]->vn] : XMFLOAT2({0.0f, 0.0f}));
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

	Drawable cube(device.Get(), bufferData, scale, rotate, translate);
	cube.CreateBoundingBoxFromPoints(vMin, vMax);
	m_drawables.push_back(cube);

	return true;
}