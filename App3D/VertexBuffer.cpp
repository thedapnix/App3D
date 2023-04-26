#include "VertexBuffer.h"

void VertexBuffer::Init(ID3D11Device* device, UINT vertexSize, UINT vertexCount, void* vertexData)
{
	m_size = vertexSize;
	m_count = vertexCount;

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = m_size * m_count;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = vertexData;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&bufferDesc, &data, m_buffer.GetAddressOf());

	if (FAILED(hr)) {
		MessageBox(NULL, L"Failed to create vertex buffer!", L"Error", MB_OK);
	}
}

UINT VertexBuffer::GetVertexCount() const
{
	return m_count;
}

UINT VertexBuffer::GetVertexSize() const
{
	return m_size;
}

ID3D11Buffer* VertexBuffer::GetBuffer() const
{
	return m_buffer.Get();
}
