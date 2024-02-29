#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

//Buffers and stuff needed for graphicscomponent (since it's basically just a new way of making drawables)
#include "Drawable.h"

#include "Upgrades.h"

//Temporary graphics component gibberish that needs to work with however the rest of graphics stuff is set up so this is all super temp
struct GraphicsComponent
{
	//Defaults
	DirectX::XMFLOAT3 m_scale = { 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 m_rotate = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 m_translate = { 0.0f, 0.0f, 0.0f };

	//Container/descriptor for the transform
	struct WorldTransform
	{
		DirectX::XMFLOAT4X4 scale;
		DirectX::XMFLOAT4X4 rotate;
		DirectX::XMFLOAT4X4 translate;
	};
	WorldTransform m_transform;

	//Ababa
	ConstantBuffer m_constantBuffer;

	//Buffers
	VertexBuffer m_vertexBuffer;
	IndexBuffer m_indexBuffer;

	//ConstructoRR (ehe do Initialize()-function instead maybe)
	GraphicsComponent(ID3D11Device* device, const BufferData& data, DirectX::XMFLOAT3 translation )
	{
		//Every drawable has a vertex buffer, an index buffer, and a constant buffer
		m_vertexBuffer.Init(
			device,
			data.vData.size,
			data.vData.count,
			(void*)(data.vData.vector.data()));

		m_indexBuffer.Init(
			device,
			data.iData.size,
			data.iData.count,
			(uint32_t*)(data.iData.vector.data()));


		m_translate = translation; //ehe do Initialize()-functions instead

		//Store all the stuff into the m_transform
		DirectX::XMStoreFloat4x4(&m_transform.scale, DirectX::XMMatrixTranspose(
			DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z)
		));
		DirectX::XMStoreFloat4x4(&m_transform.rotate, DirectX::XMMatrixTranspose(
			DirectX::XMMatrixRotationX(m_rotate.x) * DirectX::XMMatrixRotationY(m_rotate.y) * DirectX::XMMatrixRotationZ(m_rotate.z)
		));
		DirectX::XMStoreFloat4x4(&m_transform.translate, DirectX::XMMatrixTranspose(
			DirectX::XMMatrixTranslation(m_translate.x, m_translate.y, m_translate.z)
		));

		//Init constant buffer using the transform
		m_constantBuffer.Init(device, &m_transform, sizeof(m_transform));

		//Submesh stuff? Idk I have no idea how the rest of graphics stuff is handled here
	}

	//Guhhh..?
	void Move(float x, float y, float z)
	{
		m_translate.x += x;
		m_translate.y += y;
		m_translate.z += z;

		DirectX::XMStoreFloat4x4(&m_transform.translate, DirectX::XMMatrixTranspose(
			DirectX::XMMatrixTranslation(m_translate.x, m_translate.y, m_translate.z)
		));
	}
};

#define MAX_UPGRADES 8
struct PlayerComponent
{
#define MAX_LENGTH 16

	char name[MAX_LENGTH] = "Default Name";
	int hp = 100;

	std::bitset<MAX_UPGRADES> relicBitset; //I have become bitset enjoyer

	float damage = 20.0f;
	float defense = 0.0f; //percentage
	float speed = 0.25f;

	PlayerComponent(const char name_in[MAX_LENGTH], const int hp) :hp(hp)
	{
		std::memcpy(name, name_in, MAX_LENGTH);
	}

	struct UpgradeArray
	{
		UpgradeArray(size_t upgradeSize)
		{
			this->upgradeSize = upgradeSize;
			this->data = new char[upgradeSize];
		}

		inline void* get()
		{
			return data;
		}

		~UpgradeArray()
		{
			delete[] data;
		}

		char* data = nullptr;

	private:
		size_t upgradeSize = 0;
	};

	//Any time GetId() is called on a new type of upgrade, count gets incremented, which results in the ID number for every type of upgrade being unique
	int upgradeCount = 0;
	template <typename T>
	int GetId()
	{
		static int id = upgradeCount++;
		return id;
	}

	template<typename T>
	void AddRelic()
	{
		//Ehe
		int id = GetId<T>();

		//Create relic and cast to RelicArray data
		T* relicPointer = new (relics->get()) T();

		//Set the component bitset of the entity at "id" to match the component we're passing in
		relicBitset.set(id);
	}

	template<typename T>
	T* GetRelic()
	{
		int id = GetId<T>(); //Since GetId is being called on a type T that it's been called on before, the id won't be incremented. It's brilliant honestly

		if (!relicBitset.test(id)) //Test to see if we have the relic before we try returning it
			return nullptr;

		//Get relic by casting RelicArray data back to the relic struct (but pointer)
		T* relicPointer = (T*)(relics->get());

		return relicPointer;
	}

	template<typename T>
	void RemoveRelic()
	{
		int id = GetId<T>();
		relicBitset.reset(id);
	}

private:
	UpgradeArray* relics = new UpgradeArray(sizeof(DamageRelic)); //Since all relics are the same size, this is fine
};