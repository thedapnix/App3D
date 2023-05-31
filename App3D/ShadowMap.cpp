#include "ShadowMap.h"
#include <d3dcompiler.h>

ShadowMap::ShadowMap(ID3D11Device* device, std::vector<Drawable>* drawables, std::vector<SpotLight>* spotlights)
{
	//Fill vectors
	m_spotlights = spotlights;
	if (m_spotlights->size() == 0)
	{
		MessageBox(NULL, L"No lights, shadowmap not created!", L"Error", MB_OK);
	}
	m_drawables = drawables;

	InitShaderAndInputLayout(device);

	UINT shadowResolution = 512;

	InitDepthBuffer(device, shadowResolution, m_spotlights->size());

	viewport.Width = shadowResolution;
	viewport.Height = shadowResolution;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	InitStructuredBuffer(device, false, false, sizeof(SpotLight), m_spotlights->size(), &m_spotlights);
}

ID3D11VertexShader* ShadowMap::GetVertexShader()
{
	return vertexShader.Get();
}

const D3D11_VIEWPORT* ShadowMap::GetViewport() const
{
	return &viewport;
}

ID3D11DepthStencilView* ShadowMap::GetDepthStencilViewAt(UINT index)
{
	return DSVs.at(index).Get();
}

void ShadowMap::InitStructuredBuffer(ID3D11Device* device, bool isDynamic, bool hasUAV, UINT elementSize, UINT elementCount, void* bufferData)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = elementSize * elementCount;
	desc.Usage = isDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
	desc.BindFlags =  D3D11_BIND_SHADER_RESOURCE;
	desc.BindFlags |= hasUAV ? D3D11_BIND_UNORDERED_ACCESS : 0u;
	desc.CPUAccessFlags = isDynamic ? D3D11_CPU_ACCESS_WRITE : 0u;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = elementSize;

	HRESULT hr;
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = bufferData;
	data.SysMemPitch = data.SysMemSlicePitch = 0;
	hr = device->CreateBuffer(&desc, &data, structuredBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create structured buffer for shadowmap!", L"Error", MB_OK);
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = elementCount;

	hr = device->CreateShaderResourceView(structuredBuffer.Get(), &srvDesc, &srv);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create shader resource view for shadowmap!", L"Error", MB_OK);
		return;
	}
}

void ShadowMap::InitDepthBuffer(ID3D11Device* device, UINT resolution, UINT arraySize)
{
	//Cookbook says that only the depth stencil view is necessary, so I'm skipping the depth stencil state
	HRESULT hr;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = resolution;
	desc.Height = resolution;
	desc.MipLevels = 1;
	desc.ArraySize = arraySize; //"ArraySize member of the description will be the number of textures in the array" uhh
	desc.Format = DXGI_FORMAT_R32_TYPELESS; //DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	hr = device->CreateTexture2D(&desc, NULL, &DST);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create depth stencil texture for shadowmap!", L"Error", MB_OK);
		return;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
	dsvd.Format = DXGI_FORMAT_D32_FLOAT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	dsvd.Texture2DArray.MipSlice = 0;
	dsvd.Texture2DArray.ArraySize = 1;
	for (UINT i = 0; i < arraySize; i++)
	{
		dsvd.Texture2DArray.FirstArraySlice = i;
		ID3D11DepthStencilView* dsv = NULL;
		hr = device->CreateDepthStencilView(DST.Get(), &dsvd, &dsv);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Failed to create depth stencil view for shadowmap!", L"Error", MB_OK);
			return;
		}
		DSVs.push_back(dsv);
	}
}

void ShadowMap::InitShaderAndInputLayout(ID3D11Device* device)
{
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3DBlob>
		vsBlob,
		errorBlob;

	hr = D3DReadFileToBlob(L"../x64/Debug/ShadowMapVertexShader.cso", &vsBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to read vertex shader for shadowmap!", L"Error", MB_OK);
	}
	
	hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &vertexShader);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create vertex shader for shadowmap!", L"Error", MB_OK);
		return;
	}

	//Create Input Layout using data from our vsBlob, copied from D3D11Engine
	//D3D11_INPUT_ELEMENT_DESC inputElementDesc[1] =
	//{
	//  { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//  //{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//  //{ "NOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//};
	//hr = device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
	//if (FAILED(hr))
	//{
	//	MessageBox(NULL, L"Failed to create input layout for shadowmap!", L"Error", MB_OK);
	//	return;
	//}
}