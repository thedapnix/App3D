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

	UINT shadowResolution = 1024;

	viewport.Width = shadowResolution;
	viewport.Height = shadowResolution;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	InitSampler(device);
}

ID3D11VertexShader* ShadowMap::GetVertexShader()
{
	return vertexShader.Get();
}

ID3D11VertexShader* ShadowMap::GetVertexShaderPov()
{
	return vertexShaderPov.Get();
}

const D3D11_VIEWPORT* ShadowMap::GetViewport() const
{
	return &viewport;
}

ID3D11SamplerState* ShadowMap::GetSampler()
{
	return shadowSampler.Get();
}

void ShadowMap::InitShaderAndInputLayout(ID3D11Device* device)
{
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3DBlob>
		vsBlob,
		vspBlob, //new
		errorBlob;

	hr = D3DReadFileToBlob(L"../x64/Debug/ShadowMapVertexShader.cso", &vsBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to read vertex shader for shadowmap!", L"Error", MB_OK);
	}

	//new
	hr = D3DReadFileToBlob(L"../x64/Debug/ShadowMapVertexShaderPov.cso", &vspBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to read vertex shader for shadowmap pov!", L"Error", MB_OK);
	}
	
	hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &vertexShader);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create vertex shader for shadowmap!", L"Error", MB_OK);
		return;
	}

	//new
	hr = device->CreateVertexShader(vspBlob->GetBufferPointer(), vspBlob->GetBufferSize(), NULL, &vertexShaderPov);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create vertex shader for shadowmap pov!", L"Error", MB_OK);
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

void ShadowMap::InitSampler(ID3D11Device* device)
{
	D3D11_SAMPLER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP; //Border?
	desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.MinLOD = 0.0f;
	desc.MaxLOD = D3D11_FLOAT32_MAX;
	desc.MipLODBias = 0.0f;
	desc.MaxAnisotropy = 1;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	HRESULT hr = device->CreateSamplerState(&desc, shadowSampler.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create sampler for shadowmap!", L"Error", MB_OK);
	}
}
