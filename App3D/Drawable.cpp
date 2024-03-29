#include "Drawable.h"

//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

Drawable::Drawable(ID3D11Device* device, const BufferData& data, DirectX::XMFLOAT3 scaling, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 translation)
{
	//Every drawable has a vertex buffer, an index buffer, and a constant buffer
	m_vertexVectorData = (void*)(data.vData.vector.data());
	m_vertexBuffer.Init(
		device,
		data.vData.size,
		data.vData.count,
		m_vertexVectorData
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

	/*Submesh stuff*/
	for (auto& submesh : data.subMeshVector)
	{
		m_submeshes.push_back(
			SubMesh(device, submesh.startIndex, submesh.indexCount,
			submesh.ambientSRV.Get(), submesh.diffuseSRV.Get(), submesh.specularSRV.Get(), submesh.shininess)
		);
	}
}

void Drawable::Bind(ID3D11DeviceContext* context, bool isReflective) const
{
	//Buffers
	context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetBufferAddress());
	
	ID3D11Buffer* buffer[] = { m_vertexBuffer.GetBuffer() };
	UINT stride = m_vertexBuffer.GetVertexSize();
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, buffer, &stride, &offset);

	context->IASetIndexBuffer(m_indexBuffer.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);

	for (int i = 0; i < m_submeshes.size(); i++)
	{
		m_submeshes.at(i).Bind(context, isReflective);
		m_submeshes.at(i).Draw(context);
	}
}

void Drawable::Draw(ID3D11DeviceContext* context, UINT index) const
{
	m_submeshes.at(index).Draw(context);
}

void Drawable::Unbind(ID3D11DeviceContext* context)
{
	ID3D11ShaderResourceView* nullSRV = NULL;
	context->PSSetShaderResources(0, 1, &nullSRV);
}

void Drawable::UpdateConstantBuffer(ID3D11DeviceContext* context)
{
	D3D11_MAPPED_SUBRESOURCE mapped = {};												//Set up the new srtData for the resource, zero the memory
	context->Map(m_constantBuffer.GetBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);	//Disable GPU access to the srtData we want to change, and get a pointer to its memory
	memcpy(mapped.pData, &m_transform, sizeof(m_transform));							//Write the new srtData to memory
	context->Unmap(m_constantBuffer.GetBuffer(), 0);									//Re-enable GPU access to the srtData
}

void Drawable::CreateBoundingBoxFromPoints(DirectX::XMVECTOR min, DirectX::XMVECTOR max)
{
	DirectX::BoundingBox::CreateFromPoints(m_aabb, min, max);
	m_aabb.Transform(m_aabb, World()); //Transform without transposing the world matrix
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

void Drawable::Rotate(float angleX, float angleY, float angleZ)
{
	m_rotate.x += angleX;
	m_rotate.y += angleY;
	m_rotate.z += angleZ;
	DirectX::XMStoreFloat4x4(&m_transform.rotate, DirectX::XMMatrixTranspose(
		DirectX::XMMatrixRotationX(m_rotate.x) * DirectX::XMMatrixRotationY(m_rotate.y) * DirectX::XMMatrixRotationZ(m_rotate.z)
	));
}

DirectX::XMMATRIX Drawable::World() const
{
	//This feels like a horribly expensive function. I might be wrong though. But if I'm not, consider storing a XMFLOAT4X4 m_world
	return DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) * 
		DirectX::XMMatrixRotationX(m_rotate.x) * DirectX::XMMatrixRotationY(m_rotate.y) * DirectX::XMMatrixRotationZ(m_rotate.z) * 
		DirectX::XMMatrixTranslation(m_translate.x, m_translate.y, m_translate.z);
}

const DirectX::BoundingBox Drawable::GetBoundingBox() const
{
	return m_aabb;
}

const VertexBuffer& Drawable::GetVertexBuffer() const
{
	return m_vertexBuffer;
}

void* Drawable::GetVertexVectorData()
{
	return m_vertexVectorData;
}
