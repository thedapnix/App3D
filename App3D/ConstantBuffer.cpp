#include "ConstantBuffer.h"

void ConstantBuffer::Init(ID3D11Device* device, void* initialData, size_t byteSize)
{
	m_bufferSize = byteSize;

	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = byteSize;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = initialData;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&desc, &data, m_buffer.GetAddressOf());

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create constant buffer!", L"Error", MB_OK);
	}
}

void ConstantBuffer::UpdateBuffer(ID3D11DeviceContext* context, void* data)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	HRESULT hr = context->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to map resource!", L"Error", MB_OK);
		return;
	}
	memcpy(mappedResource.pData, data, m_bufferSize);

	context->Unmap(m_buffer.Get(), 0);
}

ID3D11Buffer* ConstantBuffer::GetBuffer() const
{
	return m_buffer.Get();
}

ID3D11Buffer* const *ConstantBuffer::GetBufferAddress() const
{
	return m_buffer.GetAddressOf();
}

size_t ConstantBuffer::GetSize() const
{
	return m_bufferSize;
}