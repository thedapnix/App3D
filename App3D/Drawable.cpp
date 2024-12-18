#include "Drawable.h"

using namespace DirectX; //We about to do some maths in here

Drawable::Drawable(ID3D11Device* device, const BufferData& data, DirectX::XMFLOAT3 scaling, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 translation, int interact, std::vector<int> interactsWith)
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
		(uint32_t*)(data.iData.vector.data())
	);

	m_scale = scaling;
	m_rotate = rotation;
	m_translate = translation;

	//Later on whenever we update constant buffers we will pass in additional information to this function
	//CalculateAndTransposeWorld();

	m_constantBuffer.Init(device, &m_cbd, sizeof(m_cbd));

	/*Submesh stuff*/
	for (auto& submesh : data.subMeshVector)
	{
		m_submeshes.push_back(
			SubMesh(device, submesh.startIndex, submesh.indexCount,
			submesh.ambientSRV.Get(), submesh.diffuseSRV.Get(), submesh.specularSRV.Get(), submesh.shininess)
		);
	}

	m_drawableInfo.interactID = interact;
	m_drawableInfo.interactsWith = interactsWith;
	m_drawableInfo.isActive = true;

	m_isDirty = true; //This way, we can ensure that the CalculateAndTransposeWorld() function is called once, and in a proper manner
}

void Drawable::Draw(ID3D11DeviceContext* context) const
{
	//Bind, Draw, and Unbind for all submeshes (assuming that the drawable is considered active)
	if (m_drawableInfo.isActive)
	{
		//Buffers
		context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetBufferAddress());
		context->PSSetConstantBuffers(0, 1, m_constantBuffer.GetBufferAddress());

		ID3D11Buffer* buffer[] = { m_vertexBuffer.GetBuffer() };
		UINT stride = m_vertexBuffer.GetVertexSize();
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, buffer, &stride, &offset);

		context->IASetIndexBuffer(m_indexBuffer.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);

		for (int i = 0; i < m_submeshes.size(); i++)
		{
			m_submeshes.at(i).Bind(context, m_isReflective, m_hasNormalMap);
			m_submeshes.at(i).Draw(context);
			m_submeshes.at(i).Unbind(context, m_isReflective, m_hasNormalMap);
		}
		context->VSSetConstantBuffers(0, 0, NULL);
		context->PSSetConstantBuffers(0, 0, NULL);
	}
}

void Drawable::DrawInstanced(ID3D11DeviceContext* context) const
{
	//Bind, Draw, and Unbind for all submeshes (assuming that the drawable is considered active)
	if (m_drawableInfo.isActive)
	{
		//Buffers
		context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetBufferAddress());
		context->PSSetConstantBuffers(0, 1, m_constantBuffer.GetBufferAddress());

		//New: IASetVertexBuffers() will now be taking in two different structs of data, one for the "regular" per-vertex info (pos, uv, nor), and one for per-instance info (world)
		/*
		Example:
		UINT stride[2] = { sizeof(PerVertexInfo), sizeof(PerInstanceInfo) };
		UINT offset[2] = { 0, 0 };

		ID3D11Buffer* vertexBuffers[2] = { DrawableVertexBuffer, InstancedBuffer };
		context->IASetVertexBuffers(0, 2, vertexBuffers, stride, offset);
		*/

		//UINT stride[2] = { sizeof(Vertex), sizeof(Instance) }; //alt. m_vertexBuffer and m_instancedBuffer .GetSize()
		//UINT offset[2] = { 0, 0 };

		//ID3D11Buffer* vertexBuffers[2] = { m_vertexBuffer.GetBuffer(), m_instancedBuffer.GetBuffer()};
		//context->IASetVertexBuffers(0, 2, vertexBuffers, stride, offset);

		context->IASetIndexBuffer(m_indexBuffer.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);

		for (int i = 0; i < m_submeshes.size(); i++)
		{
			m_submeshes.at(i).Bind(context, m_isReflective, m_hasNormalMap);
			m_submeshes.at(i).Draw(context);
			m_submeshes.at(i).Unbind(context, m_isReflective, m_hasNormalMap);
		}
		context->VSSetConstantBuffers(0, 0, NULL);
		context->PSSetConstantBuffers(0, 0, NULL);
	}
}

void Drawable::UpdateConstantBuffer(ID3D11DeviceContext* context, Camera* camera, const DirectX::XMFLOAT3& pos)
{
	if (m_isDirty)
	{
		//Calculate transform (might have been altered during frame) and then transpose before sending to shaders
		CalculateAndTransposeWorld(pos, camera);

		m_cbd.hasNormalMap = m_hasNormalMap;

		D3D11_MAPPED_SUBRESOURCE mapped = {};												//Set up the new srtData for the resource, zero the memory
		context->Map(m_constantBuffer.GetBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);	//Disable GPU access to the srtData we want to change, and get a pointer to its memory
		memcpy(mapped.pData, &m_cbd, sizeof(m_cbd));										//Write the new srtData to memory
		context->Unmap(m_constantBuffer.GetBuffer(), 0);									//Re-enable GPU access to the srtData

		m_isDirty = false;
	}
}

void Drawable::CreateBoundingBoxFromPoints(DirectX::XMVECTOR min, DirectX::XMVECTOR max)
{
	DirectX::BoundingBox::CreateFromPoints(m_aabb, min, max);
	m_aabb.Transform(m_aabb, World()); //Transform without transposing the world matrix
}

void Drawable::SetPosition(float x, float y, float z)
{
	m_translate.x = x;
	m_translate.y = y;
	m_translate.z = z;

	m_isDirty = true;
}

void Drawable::Move(float x, float y, float z)
{
	m_translate.x += x;
	m_translate.y += y;
	m_translate.z += z;

	m_isDirty = true;
}

void Drawable::SetRotation(float angleX, float angleY, float angleZ)
{
	m_rotate.x = angleX;
	m_rotate.y = angleY;
	m_rotate.z = angleZ;

	m_isDirty = true;
}

void Drawable::Rotate(float angleX, float angleY, float angleZ)
{
	m_rotate.x += angleX;
	m_rotate.y += angleY;
	m_rotate.z += angleZ;

	m_isDirty = true;
}

const DirectX::XMFLOAT3& Drawable::GetPosition() const
{
	return m_translate;
}

const DirectX::XMFLOAT3& Drawable::GetRotation() const
{
	return m_rotate;
}

DirectX::XMMATRIX Drawable::World() const
{
	//This feels like a horribly expensive function. I might be wrong though. But if I'm not, consider storing a XMFLOAT4X4 m_world
	return DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) * 
		DirectX::XMMatrixRotationX(m_rotate.x) * DirectX::XMMatrixRotationY(m_rotate.y) * DirectX::XMMatrixRotationZ(m_rotate.z) * 
		DirectX::XMMatrixTranslation(m_translate.x, m_translate.y, m_translate.z);
}

const DirectX::BoundingBox& Drawable::GetBoundingBox() const
{
	return m_aabb;
}

const VertexBuffer& Drawable::GetVertexBuffer() const
{
	return m_vertexBuffer;
}

const IndexBuffer& Drawable::GetIndexBuffer() const
{
	return m_indexBuffer;
}

void* Drawable::GetVertexVectorData()
{
	return m_vertexVectorData;
}

const ConstantBuffer& Drawable::GetConstantBuffer() const
{
	return m_constantBuffer;
}

const std::vector<SubMesh>& Drawable::GetSubMeshes() const
{
	return m_submeshes;
}

void Drawable::SetWorld(DirectX::XMMATRIX mat)
{
	DirectX::XMStoreFloat4x4(&m_cbd.world, mat);
	m_isDirty = true; //Gotta update the constant buffer after this
}

bool Drawable::IsInteractible() const
{
	return (m_drawableInfo.interactID > 0);
}

int Drawable::GetInteractID() const
{
	return m_drawableInfo.interactID;
}

DrawableInfo& Drawable::GetDrawableInfo()
{
	return m_drawableInfo;
}

void Drawable::RemoveInteraction()
{
	m_drawableInfo.interactID = 0;
}

bool Drawable::IsActive() const
{
	return m_drawableInfo.isActive;
}

void Drawable::SetReflective(bool set)
{
	m_isReflective = set;
}

bool Drawable::IsReflective() const
{
	return m_isReflective;
}

void Drawable::SetConcave(bool set)
{
	m_isConcave = set;
}

bool Drawable::IsConcave() const
{
	return m_isConcave;
}

bool Drawable::DoesOrbit() const
{
	return m_orbits;
}

void Drawable::SetOrbit(bool set)
{
	m_orbits = set;
}

void Drawable::SetNormalMap(ID3D11Device* device, std::string ddsFileName)
{
	//Store the .dds file as texture to sample in the shader
	m_normalMapTexture.Init(device, ddsFileName.c_str());
	
	//Naively put this normal map texture onto every submesh, for now
	for (auto& submesh : m_submeshes)
	{
		submesh.AddNormalMap(m_normalMapTexture.GetSRV());
	}

	m_hasNormalMap = true;
	m_isDirty = true; //Constant buffer will need to be updated
}

void Drawable::SetIndex(int idx)
{
	m_index = idx;
}

const int& Drawable::GetIndex() const
{
	return m_index;
}

void Drawable::SetInstanceCount(int n)
{
	m_instanceCount = n;
}

const int& Drawable::GetInstanceCount() const
{
	return m_instanceCount;
}

void Drawable::Destroy()
{
	m_drawableInfo.isActive = false;
}

void Drawable::CalculateAndTransposeWorld(const DirectX::XMFLOAT3& pos, Camera* camera)
{
	//World transform differs depending on if its rotation is supposed to be around its own axis, rolling around another objects axis, or orbitting another object with an offset

	//"Normally"	->	scale * rotate * translate
	//"Rolling"		->	scale * translate * rotate
	//"Orbiting"	->	scale * selfTranslate * otherRotate * otherTranslate
	DirectX::XMFLOAT3 defaultPos = { 0.0f, 0.0f, 0.0f };
	bool isDefault = XmFloat3Compare(pos, defaultPos);
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX invWorld;
	if (!m_orbits) //"normal" world transform
	{
		world =
			DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) *
			DirectX::XMMatrixRotationX(m_rotate.x) * DirectX::XMMatrixRotationY(m_rotate.y) * DirectX::XMMatrixRotationZ(m_rotate.z) *
			DirectX::XMMatrixTranslation(m_translate.x, m_translate.y, m_translate.z)
			;
	}
	else
	{
		if (isDefault && !camera) //Dirty assumption I think, but we do the "rolling" transform if we haven't passed in a position for it to orbit around
		{
			world =
				DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) *
				DirectX::XMMatrixTranslation(m_translate.x, m_translate.y, m_translate.z) *
				(DirectX::XMMatrixRotationX(m_rotate.x) * DirectX::XMMatrixRotationY(m_rotate.y) * DirectX::XMMatrixRotationZ(m_rotate.z))
				;
		}
		else //"orbiting" world transform
		{
			if (!camera) //If camera hasn't been passed in, orbit around the position that was passed in (If you've done SetOrbit() on a drawable and aren't passing in a position here, you're griefing)
			{
				world =
					DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) *
					DirectX::XMMatrixTranslation(m_translate.x, m_translate.y, m_translate.z) *
					DirectX::XMMatrixRotationX(m_rotate.x) * DirectX::XMMatrixRotationY(m_rotate.y) *
					DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z)
					;
			}
			else //If camera has been passed in, orbit around that
			{
				world =
					DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) *
					DirectX::XMMatrixTranslation(m_translate.x, m_translate.y, m_translate.z) *
					DirectX::XMMatrixRotationX(camera->GetPitch()) * DirectX::XMMatrixRotationY(camera->GetYaw()) *
					DirectX::XMMatrixTranslation(camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z)
					;
			}
		}
	}
	
	//We only ever call this function before we're about to enter Render()-function, so we transpose the world-matrix before passing it to shaders
	invWorld = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(NULL, world));
	world = DirectX::XMMatrixTranspose(world);
	DirectX::XMStoreFloat4x4(&m_cbd.world, world);
	DirectX::XMStoreFloat4x4(&m_cbd.invWorld, invWorld); //Does this also need to be transposed?

	//Update the bounding box as well so interacting with moving targets isn't terrible
	if (m_orbits)
	{
		if (!isDefault) //Add in the translation of the input point
		{
			m_aabb.Center.x = pos.x + m_translate.x;
			m_aabb.Center.y = pos.y + m_translate.y;
			m_aabb.Center.z = pos.z + m_translate.z;
		}
		else if (camera) //Add in the translation of the camera
		{
			m_aabb.Center.x = camera->GetPosition().x + m_translate.x;
			m_aabb.Center.y = camera->GetPosition().y + m_translate.y;
			m_aabb.Center.z = camera->GetPosition().z + m_translate.z;
		}
	}
	else //Just set the bounding box to be the translate, no need to worry about orbitting
	{
		m_aabb.Center.x = m_translate.x;
		m_aabb.Center.y = m_translate.y;
		m_aabb.Center.z = m_translate.z;
	}
}
