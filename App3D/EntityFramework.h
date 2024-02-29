#pragma once

#include <iostream>
#include <vector>
#include <windows.h>

#include <bitset>
#include <Windows.h>

#include "ComponentHelper.h"

//Just for better readability
struct EntityID
{
	int index;
	bool state; //Boolean to check if the entity has been destroyed or not
};

//Defines a componentBitset as a bitset made up of 32 bits, and function GetId (It's either this solution, or Registry and View redefines these)
namespace EntityGlobals
{
	static constexpr int MAX_COMPONENTS = 32;
	typedef std::bitset<MAX_COMPONENTS> componentBitset; //cppreference bitset: "N -> the number of bits to allocate storage for"

	//Any time GetId() is called on a new type of component T, compCount gets incremented, which results in the ID number for every component type being unique
	static int compCount = 0; //edit: no longer extern, now just static
	template <typename T>
	int GetId()
	{
		static int compId = compCount++;
		return compId;
	}

	static constexpr bool IsEntityValid(const EntityID& id)
	{
		return id.index != -1;
	}
}

class Registry
{
private:
	struct Entity
	{
		EntityID id;
		EntityGlobals::componentBitset components;

		//Quick operator hack giving better visualization to the user, you're welcome <3
		operator const int& () const { return id.index; }
	};

	std::vector<int> availableEntitySlots;	//Contains the indices that are allowed to be used when creating new entitites (because they've for instance been 
											//destroyed prior, and we dont want to adjust the indices of existing entities)

public:
	std::vector<Entity> entities;

	size_t GetEntityCount()
	{
		//Since destroying an entity doesn't technically remove it (rather it stores its index into the availableEntitySlots-vector for future insertions) 
		//this is the "real" entity count
		return entities.size() - availableEntitySlots.size();
	}

	EntityID CreateEntity()
	{
		//When we destroy an entity, we store its index and version in the freeEntities-vector so we know where we can 
		//If there's space in the freeEntities-vector, we use the version number stored in there. Otherwise we simply create a new id with version number 0
		if (!availableEntitySlots.empty())
		{
			int newIndex = availableEntitySlots.back();
			availableEntitySlots.pop_back();

			EntityID newId = CreateEntityId(newIndex, GetEntityState(entities[newIndex].id));
			entities[newIndex].id = newId;
			return newId;
		}
		entities.push_back({ CreateEntityId((int)entities.size(), false), EntityGlobals::componentBitset() });
		return entities.back().id;
	}

	void DestroyEntity(EntityID id)
	{
		EntityID nullID = CreateEntityId(-1, true);

		entities[GetEntityIndex(id)].id = nullID;
		entities[GetEntityIndex(id)].components.reset();

		availableEntitySlots.push_back(GetEntityIndex(id));
	}

	template<typename T, typename ...Args>
	void AddComponent(EntityID id, Args ...args)
	{
		//Don't try to access entity that has already been removed
		if (entities[GetEntityIndex(id)].id.index != id.index)
			return;

		//Unique index of the component itself
		int compId = EntityGlobals::GetId<T>();

		//Adding a new component that's greater than the size of the pool requires a resize for memory reasons
		if ((int)componentArrays.size() <= compId)
			componentArrays.resize(compId + 1, nullptr);

		//If there isn't already a componentArray of this type of component, make space for one
		if (componentArrays[compId] == nullptr)
			componentArrays[compId] = new ComponentArray(sizeof(T));

		T* componentPointer = new (componentArrays[compId]->get(GetEntityIndex(id))) T(args...);

		//Set the component bitset of the entity at "id" to match the component we're passing in
		entities[GetEntityIndex(id)].components.set(compId);
	}

	template <typename T>
	void RemoveComponent(EntityID id)
	{
		if (entities[GetEntityIndex(id)].id.index != id.index) //Don't try to access entity that has already been removed
			return;

		//Remove any component in this entity's component bitset that matches the component ID we're looking for
		int compId = EntityGlobals::GetId<T>();
		entities[GetEntityIndex(id)].components.reset(compId);
	}

	template<typename T>
	T* GetComponent(EntityID id)
	{
		if (entities[GetEntityIndex(id)].id.index != id.index) //Don't try to access entity that has been destroyed
			return nullptr;

		int compId = EntityGlobals::GetId<T>();

		if (!entities[GetEntityIndex(id)].components.test(compId)) //Test to see if the entity has the component
			return nullptr;

		//Return the component at the index of the specified entity ID
		T* componentPointer = (T*)(componentArrays[compId]->get(id.index));
		return componentPointer;
		//return &componentPointer[id.index];
	}

private:
	EntityID CreateEntityId(int index, bool hasBeenDestroyed)
	{
		return { index, hasBeenDestroyed };
	}

	int GetEntityIndex(EntityID id)
	{
		return id.index;
	}

	bool GetEntityState(EntityID id)
	{
		return id.state;
	}

	static constexpr int MAX_ENTITIES = 10;
	struct ComponentArray
	{
		ComponentArray(size_t componentSize) //Allocate enough memory to hold as many entities as we allow, along with the size of components
		{
			this->componentSize = componentSize;
			this->data = new char[componentSize * MAX_ENTITIES];
		}

		inline void* get(size_t index)
		{
			return data + index * componentSize;
		}

		~ComponentArray()
		{
			delete[] data;
		}

		char* data = nullptr;
		size_t componentSize = 0;
	};

	//Private member variables
	std::vector<ComponentArray*> componentArrays;
};

//Templated to allow for views that can contain entities with any combination of components
template<typename ...Args>
class View
{
public:
	View(Registry& registry) : pRegistry(&registry)
	{
		//Array of integers for the id's ranging from 0 to however many component-types we're passing in
		int compIds[] = { 0, EntityGlobals::GetId<Args>()... };

		for (int i = 1; i < (sizeof...(Args) + 1); i++) //note that we start from 1, since compIds already holds a 0, everything AFTER that is actual component types
		{
			components.set(compIds[i]);
		}
	}

	struct Iterator
	{
		Iterator(Registry* pRegistry, int entityIndex, EntityGlobals::componentBitset components)
			: pRegistry(pRegistry), entityIndex(entityIndex), components(components) {}

		EntityID operator*() const
		{
			return pRegistry->entities[entityIndex].id;
		}

		bool operator==(const Iterator& other) const
		{
			return entityIndex == other.entityIndex || entityIndex == pRegistry->entities.size();
		}

		bool operator!=(const Iterator& other) const
		{
			return entityIndex != other.entityIndex && entityIndex != pRegistry->entities.size();
		}

		bool ValidIndex()
		{
			return EntityGlobals::IsEntityValid(pRegistry->entities[entityIndex].id) &&
				(components == (components & pRegistry->entities[entityIndex].components)); //Bitwise "and", checks to see that the things on either side of & are the same
		}

		Iterator& operator++()
		{
			//Iterate through the registry and increment the view's index but ONLY IF the entity has the appropriate component types and is a valid entity
			do
			{
				entityIndex++;
			} while (entityIndex < (int)pRegistry->entities.size() && !ValidIndex()); //Bit cleaner ngl
			//Previously:
			/*while (entityIndex < (int)pRegistry->entities.size() &&
				!(EntityGlobals::IsEntityValid(pRegistry->entities[entityIndex].id) &&
					(components == (components & pRegistry->entities[entityIndex].components))));*/
			return *this;
		}

		int entityIndex;
		Registry* pRegistry;
		EntityGlobals::componentBitset components;
	};

	const Iterator begin() const
	{
		int first = 0;
		while (first < (int)pRegistry->entities.size() &&
			(components != (components & pRegistry->entities[first].components) || !EntityGlobals::IsEntityValid(pRegistry->entities[first].id)))
			first++;
		return Iterator(pRegistry, first, components);
	}

	const Iterator end() const
	{
		return Iterator(pRegistry, pRegistry->entities.size(), components);
	}

	Registry* pRegistry;
	EntityGlobals::componentBitset components;
};