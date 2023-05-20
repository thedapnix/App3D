#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(ID3D11Device* device)
{
	//Create some amount of particles (manipulate this value through imgui slider later)
	Particle particles[1]
	{
		{{-3.0f, -3.0f, 3.0f}}
	};
	InitStructuredBuffer(device, false, true, true, sizeof(Particle), 1, particles);
	
}

void ParticleSystem::InitStructuredBuffer(ID3D11Device* device, bool isDynamic, bool hasSRV, bool hasUAV, UINT elementSize, UINT elementCount, void* bufferData)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = elementSize * elementCount;
	desc.Usage = isDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT; //IMMUTABLE?
	desc.BindFlags = hasSRV ? D3D11_BIND_SHADER_RESOURCE : 0u;
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
		MessageBox(NULL, L"Failed to create structured buffer!", L"Error", MB_OK);
	}

	//Bind srv and uav to the structured buffer
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = elementCount;
	hr = device->CreateShaderResourceView(structuredBuffer.Get(), &srvDesc, srv.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create shader resource view for structured buffer!", L"Error", MB_OK);
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = elementCount;
	uavDesc.Buffer.Flags = 0;
	hr = device->CreateUnorderedAccessView(structuredBuffer.Get(), &uavDesc, uav.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create unordered access view for structured buffer!", L"Error", MB_OK);
	}
}
