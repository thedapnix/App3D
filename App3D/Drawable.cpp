#include "Drawable.h"

//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

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
		(uint32_t*)(data.iData.vector.data()));

	m_scale = scaling;
	m_rotate = rotation;
	m_translate = translation;

	//Later on whenever we update constant buffers we will pass in additional information to this function
	CalculateAndTransposeWorld();

	m_constantBuffer.Init(device, &m_transform, sizeof(m_transform));

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
}

void Drawable::Bind(ID3D11DeviceContext* context, bool isReflective) const
{
	if (m_drawableInfo.isActive)
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
			m_submeshes.at(i).Unbind(context, isReflective);
		}
		context->VSSetConstantBuffers(0, 0, NULL);
	}
}

void Drawable::UpdateConstantBuffer(ID3D11DeviceContext* context, bool orbits, Camera* camera)
{
	//Calculate transform (might have been altered during frame) and then transpose before sending to shaders
	CalculateAndTransposeWorld(orbits, camera);

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

void Drawable::SetPosition(float x, float y, float z)
{
	m_translate.x = x;
	m_translate.y = y;
	m_translate.z = z;
}

void Drawable::Move(float x, float y, float z)
{
	m_translate.x += x;
	m_translate.y += y;
	m_translate.z += z;
}

void Drawable::SetRotation(float angleX, float angleY, float angleZ)
{
	m_rotate.x = angleX;
	m_rotate.y = angleY;
	m_rotate.z = angleZ;
}

void Drawable::Rotate(float angleX, float angleY, float angleZ)
{
	m_rotate.x += angleX;
	m_rotate.y += angleY;
	m_rotate.z += angleZ;
}

const DirectX::XMFLOAT3& Drawable::GetPosition() const
{
	return m_translate;
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

void* Drawable::GetVertexVectorData()
{
	return m_vertexVectorData;
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

void Drawable::Destroy()
{
	m_drawableInfo.isActive = false;
}

void Drawable::CalculateAndTransposeWorld(bool orbits, Camera* camera)
{
	//World transform differs depending on if its rotation is supposed to be around its own axis, rolling around another objects axis, or orbitting another object with an offset

	//"Normally"	->	scale * rotate * translate
	//"Rolling"		->	scale * translate * rotate
	//"Orbiting"	->	scale * selfTranslate * otherRotate * otherTranslate
	DirectX::XMMATRIX world;
	if (!orbits) //"normal" world transform
	{
		world =
			DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) *
			(DirectX::XMMatrixRotationX(m_rotate.x) * DirectX::XMMatrixRotationY(m_rotate.y) * DirectX::XMMatrixRotationZ(m_rotate.z)) *
			DirectX::XMMatrixTranslation(m_translate.x, m_translate.y, m_translate.z)
			;
	}
	else
	{
		if (camera == nullptr) //Dirty assumption for now: If we're not passing in the pointer to a camera that we're supposed to orbit, we do the "rolling" world transform
		{
			world =
				DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) *
				DirectX::XMMatrixTranslation(m_translate.x, m_translate.y, m_translate.z) *
				(DirectX::XMMatrixRotationX(m_rotate.x) * DirectX::XMMatrixRotationY(m_rotate.y) * DirectX::XMMatrixRotationZ(m_rotate.z))
				;
		}
		else //"orbiting" world transform
		{
			world =
				DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) *
				DirectX::XMMatrixTranslation(m_translate.x, m_translate.y, m_translate.z) *
				DirectX::XMMatrixRotationX(camera->GetPitch()) * DirectX::XMMatrixRotationY(camera->GetYaw()) *
				DirectX::XMMatrixTranslation(camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z)
				;
		}
	}
	
	//We only ever call this function before we're about to enter Render()-function, so we transpose the world-matrix before passing it to shaders
	world = DirectX::XMMatrixTranspose(world);
	DirectX::XMStoreFloat4x4(&m_transform.world, world);
}
