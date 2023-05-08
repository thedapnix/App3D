#include "Drawable.h"

Drawable::Drawable(ID3D11Device* device, const BufferData& data, DirectX::XMFLOAT3 scaling, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 translation)
{
	//Every drawable has a vertex buffer, an index buffer, and a constant buffer
	m_vertexBuffer.Init(
		device,
		data.vData.size,
		data.vData.count,
		(void*)(data.vData.vector.data())
	);

	m_indexBuffer.Init(
		device,
		data.iData.size,
		data.iData.count,
		(uint32_t*)(data.iData.vector.data()));

	//Quoted stackoverflow answer to the question on which order we do world matrix multiplications: "Usually it is scale, then rotation and lastly translation."
	m_scale = scaling;
	m_rotate = rotation;
	m_translate = translation;
	DirectX::XMStoreFloat4x4(&m_transform.scale, DirectX::XMMatrixTranspose(
		DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z)
	));
	DirectX::XMStoreFloat4x4(&m_transform.rotate, DirectX::XMMatrixTranspose(
		DirectX::XMMatrixRotationX(m_rotate.x) * DirectX::XMMatrixRotationY(m_rotate.y) * DirectX::XMMatrixRotationZ(m_rotate.z)
	));
	DirectX::XMStoreFloat4x4(&m_transform.translate, DirectX::XMMatrixTranspose(
		DirectX::XMMatrixTranslation(m_translate.x, m_translate.y, m_translate.z)
	));
	m_constantBuffer.Init(device, &m_transform, sizeof(m_transform));

	//They can also have shader resource view+texture (realistically everything will be textured but hey, we have the option not to)
	//m_shaderResource.Init(device, mesh.textureFileName.c_str());
}

void Drawable::Bind(ID3D11DeviceContext* context)
{
	//Vertex Buffer
	ID3D11Buffer* buffer[] = { m_vertexBuffer.GetBuffer() };
	UINT stride = m_vertexBuffer.GetVertexSize();
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, buffer, &stride, &offset);

	//Index Buffer
	context->IASetIndexBuffer(m_indexBuffer.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);

	//Texture
	//ID3D11ShaderResourceView* views[] = { m_shaderResource.GetSRV() };
	//context->PSSetShaderResources(0, 1, views);
}

void Drawable::Draw(ID3D11DeviceContext* context) const
{
	context->VSSetConstantBuffers(1, 1, m_constantBuffer.GetBufferAddress());
	context->Draw(m_vertexBuffer.GetVertexCount(), 0); //obj-parser makes it so
	//context->DrawIndexed(m_indexBuffer.GetIndexCount(), 0, 0);
}

void Drawable::UpdateConstantBuffer(ID3D11DeviceContext* context)
{
	D3D11_MAPPED_SUBRESOURCE mapped = {};												//Set up the new data for the resource, zero the memory
	context->Map(m_constantBuffer.GetBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);	//Disable GPU access to the data we want to change, and get a pointer to its memory
	memcpy(mapped.pData, &m_transform, sizeof(m_transform));							//Write the new data to memory
	context->Unmap(m_constantBuffer.GetBuffer(), 0);									//Re-enable GPU access to the data
}

void Drawable::CreateBoundingBoxFromPoints(DirectX::XMVECTOR min, DirectX::XMVECTOR max)
{
	DirectX::BoundingBox::CreateFromPoints(m_aabb, min, max);
}

void Drawable::EditTranslation(float x, float y, float z)
{
	m_translate.x += x;
	m_translate.y += y;
	m_translate.z += z;
	DirectX::XMStoreFloat4x4(&m_transform.translate, DirectX::XMMatrixTranspose(
		DirectX::XMMatrixTranslation(m_translate.x, m_translate.y, m_translate.z)
	));
}

DirectX::XMMATRIX Drawable::World()
{
	//This feels like a horribly expensive function. I might be wrong though. But if I'm not, consider storing a XMFLOAT4X4 m_world
	return DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) * 
		DirectX::XMMatrixRotationX(m_rotate.x) * DirectX::XMMatrixRotationY(m_rotate.y) * DirectX::XMMatrixRotationZ(m_rotate.z) * 
		DirectX::XMMatrixTranslation(m_translate.x, m_translate.y, m_translate.z);
}

DirectX::BoundingBox Drawable::GetBoundingBox()
{
	return m_aabb;
}
