#include "SubMesh.h"

SubMesh::SubMesh(ID3D11Device* device, UINT startIndex, UINT indexCount,
	ID3D11ShaderResourceView* ambient, ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* specular, float shine)
{
	m_startIndex = startIndex;
	m_indexCount = indexCount;

	ambientSRV = ambient;
	diffuseSRV = diffuse;
	specularSRV = specular;

	ShininessCB m_shineCB = {};
	m_shineCB.shininess = shine;
	m_constantBufferShininess.Init(device, &m_shineCB, sizeof(m_shineCB));
}

void SubMesh::Bind(ID3D11DeviceContext* context) const
{
	ID3D11ShaderResourceView* SRVs[] = { ambientSRV.Get(), diffuseSRV.Get(), specularSRV.Get() };
	context->PSSetShaderResources(0, 3, SRVs);
	context->PSSetConstantBuffers(0, 1, m_constantBufferShininess.GetBufferAddress());
}

void SubMesh::Draw(ID3D11DeviceContext* context) const
{
	context->DrawIndexed(m_indexCount, m_startIndex, 0);
}
