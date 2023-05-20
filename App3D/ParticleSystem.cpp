#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(ID3D11Device* device, bool isDynamic, bool hasSRV, bool hasUAV)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	//temp values, we will need to get our data from vertex buffer
	float value = 5;
	int count = 3;

	desc.ByteWidth = sizeof(value) * count;
	desc.Usage = isDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = hasSRV ? D3D11_BIND_SHADER_RESOURCE : 0;
	desc.BindFlags |= hasUAV ? D3D11_BIND_UNORDERED_ACCESS : 0;
	desc.CPUAccessFlags = isDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = sizeof(float);

	HRESULT hr;
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));

	//data.pSysMem = vertexBufferData;
	data.SysMemPitch = data.SysMemSlicePitch = 0;
	hr = device->CreateBuffer(&desc, &data, &structuredBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create structured buffer!", L"Error", MB_OK);
	}

	//Bind srv and uav to the structured buffer
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = count; //change
	hr = device->CreateShaderResourceView(structuredBuffer.Get(), &srvDesc, sbSRV.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create shader resource view for structured buffer!", L"Error", MB_OK);
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = count; //chaaange
	uavDesc.Buffer.Flags = 0;
	hr = device->CreateUnorderedAccessView(structuredBuffer.Get(), &uavDesc, sbUAV.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create unordered access view for structured buffer!", L"Error", MB_OK);
	}
}
