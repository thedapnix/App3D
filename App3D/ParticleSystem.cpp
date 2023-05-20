#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(ID3D11Device* device, bool isDynamic, bool hasSRV, bool hasUAV, std::vector<Drawable> drawables)
{
	UINT vCount = 0u, vSize = 0u;
	for (auto& i : drawables)
	{
		vCount += i.GetVertexBuffer().GetVertexCount();
	}
	vSize = drawables.at(0).GetVertexBuffer().GetVertexSize(); //All vertices will have the same size so picking one at any index will do

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = vSize * vCount;
	desc.Usage = isDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = hasSRV ? D3D11_BIND_SHADER_RESOURCE : 0u;
	desc.BindFlags |= hasUAV ? D3D11_BIND_UNORDERED_ACCESS : 0u;
	desc.CPUAccessFlags = isDynamic ? D3D11_CPU_ACCESS_WRITE : 0u;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = vSize;

	HRESULT hr;
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = drawables.at(0).GetVertexVectorData();
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
	srvDesc.Buffer.NumElements = vCount;
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
	uavDesc.Buffer.NumElements = vCount;
	uavDesc.Buffer.Flags = 0;
	hr = device->CreateUnorderedAccessView(structuredBuffer.Get(), &uavDesc, sbUAV.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create unordered access view for structured buffer!", L"Error", MB_OK);
	}
}
