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

void SubMesh::Bind(ID3D11DeviceContext* context, bool isReflective) const
{
	//Textures
	if (!isReflective)
	{
		ID3D11ShaderResourceView* views[] = { ambientSRV.Get(), diffuseSRV.Get(), specularSRV.Get() };
		context->PSSetShaderResources(0, 3, views);

		context->PSSetConstantBuffers(1, 1, m_constantBufferShininess.GetBufferAddress());
	}

	//Shine cb
	//context->PSSetConstantBuffers(1, 1, m_constantBufferShininess.GetBufferAddress());
}

void SubMesh::Draw(ID3D11DeviceContext* context) const
{
	context->DrawIndexed(m_indexCount, m_startIndex, 0);
}

void SubMesh::Unbind(ID3D11DeviceContext* context, bool isReflective) const
{
	if (!isReflective)
	{
		ID3D11ShaderResourceView* views[] = { NULL, NULL, NULL };
		context->PSSetShaderResources(0, 3, views);
	}

	context->PSSetConstantBuffers(1, 0, NULL); //Probably not that important but ah
}
