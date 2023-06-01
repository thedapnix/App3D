#include "ShadowMap.h"
#include <d3dcompiler.h>

ShadowMap::ShadowMap(ID3D11Device* device, std::vector<Drawable>* drawables, SpotLights* spotlights)
{
	//Fill vectors
	m_spotlights = spotlights;
	if (m_spotlights->GetLightCount() == 0)
	{
		MessageBox(NULL, L"No lights, shadowmap not created!", L"Error", MB_OK);
	}
	m_drawables = drawables;

	InitShaderAndInputLayout(device);

	UINT shadowResolution = 512;

	viewport.Width = shadowResolution;
	viewport.Height = shadowResolution;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
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
	//return DSVs.at(index).Get();
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