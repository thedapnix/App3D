#include "IndexBuffer.h"

void IndexBuffer::Init(ID3D11Device* device, UINT size, UINT indexCount, uint32_t* indexData)
{
	m_count = indexCount;

	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = size * m_count;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = size;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = indexData;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&desc, &data, m_buffer.GetAddressOf());

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create index buffer!", L"Error", MB_OK);
	}
}

UINT IndexBuffer::GetIndexCount() const
{
	return m_count;
}

ID3D11Buffer* IndexBuffer::GetBuffer() const
{
	return m_buffer.Get();
}
