#include "ImGuiHelper.h" //Which in turn includes D3D11Engine.h

#include <dxgi.h>			//directx driver interface
#include <d3dcompiler.h>	//compiling shaders

using namespace DirectX;

D3D11Engine::D3D11Engine(const HWND& hWnd, const UINT& width, const UINT& height)
{
	//Base setup for interface (swapchain, device and immediate context), render target (and backbuffer), and viewport
	InitInterfaces(hWnd);
	InitRTV();
	InitViewport(width, height);

	//Render setup (Vertex shader first, then input layout, then pixel shader (for the sake of reusing the shader blob))
	InitVertexShader();
	InitInputLayout();
	InitPixelShader();

	//Camera setup (matrices and constant buffer)
	InitCamera();

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
	InitCube({ 5.0f, 0.5f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, -3.0f, 0.0f });

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

	StartImGuiFrame();
	
	ImGuiEngineWindow(m_camera.get());

	EndImGuiFrame();

	Render(dt);

	swapChain->Present(1, 0); //vSync enabled
}

Camera& D3D11Engine::GetCamera()
{
	return *m_camera;
}

void D3D11Engine::Render(float dt)
{
	//Viewport only ever needs to be reset if we resize the window and uh.. *I've disabled that option*
	//RTV only needs to be reset if we change render targets, and we don't do that either (We will in the future though)
	context->RSSetViewports(1, &viewport);
	
	// Clear the back buffer (and depth stencil later)
	context->ClearRenderTargetView(rtv.Get(), CLEAR_COLOR);
	context->OMSetRenderTargets(1, rtv.GetAddressOf(), NULL);

	/*Input Assembler Stage*/
	//Set primitive topology and input layout
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(inputLayout.Get());

	//Bind vertex and index buffers (per drawable)
	for (auto& drawable : m_drawables)
	{
		drawable.Bind(context.Get());
	}

	/*Vertex Shader Stage*/
	context->VSSetConstantBuffers(0, 1, m_cameraCB.GetBufferAddress());
	context->VSSetShader(vertexShader.Get(), NULL, 0);

	/*Pixel Shader Stage*/
	context->PSSetShader(pixelShader.Get(), NULL, 0);
	//this->context->PSSetSamplers(0, 1, samplerState.GetAddressOf());

	//Draw (internally, these also do VSSetConstantBuffers so sue me for not following the Rendering Pipeline stages)
	for (auto& drawable : m_drawables)
	{
		drawable.Draw(context.Get());
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
	desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.BufferDesc.RefreshRate.Numerator = 0;
	desc.BufferDesc.RefreshRate.Denominator = 0;
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = 1;
	desc.OutputWindow = window;
	desc.Windowed = TRUE;
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	desc.Flags = 0; //DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH

	UINT swapCreateFlags = 0;
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
		swapChain.GetAddressOf(),
		device.GetAddressOf(),
		NULL,
		context.GetAddressOf()
	);

	_CrtDumpMemoryLeaks();

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

void D3D11Engine::InitViewport(const UINT& width, const UINT& height)
{
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
}

void D3D11Engine::InitVertexShader()
{
	D3DReadFileToBlob(L"../x64/Debug/VertexShader.cso", &shaderBlob);
	HRESULT hr = device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &vertexShader);
	
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create vertex shader!", L"Error", MB_OK);
		return;
	}
}

void D3D11Engine::InitInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[3] =
	{
	  { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	  { "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	  { "NOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	HRESULT hr = device->CreateInputLayout(
		inputElementDesc,
		ARRAYSIZE(inputElementDesc),
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		&inputLayout);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create input layout!", L"Error", MB_OK);
		return;
	}
}

void D3D11Engine::InitPixelShader()
{
	D3DReadFileToBlob(L"../x64/Debug/PixelShader.cso", &shaderBlob);
	HRESULT hr = device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &pixelShader);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create pixel shader!", L"Error", MB_OK);
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
	//bounding frustum to be made here as well
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