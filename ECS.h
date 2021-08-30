#ifndef ECS_H
#define ECS_H

#include <vector>
#include <queue>
#include <cstdint>
#include <unordered_map>

using namespace std;

// The purpose of this is to handle the entity-component system.
// I know there are those who dislike this sort of thing;
// I think it'll make any future attempt at an editor an easier task
// and it'll hopefully keep game objects in a neat system.

#pragma region IDs
typedef std::uint64_t IDType;
typedef IDType EntityID;
typedef IDType ComponentTypeID;
const IDType NULL_ENTITY = 0;

typedef std::vector<ComponentTypeID> ArchetypeID;

template<class T>
class TypeIDGenerator
{
private:
	static IDType mCount;

public:
	template<class U>
	static const IDType GetNewID()
	{
		static const IDType idCounter = mCount++;
		return idCounter;
	}
};

template<class T> IDType TypeIDGenerator<T>::mCount = 0;
#pragma endregion

#pragma region Component Base

class ComponentBase
{
public:
	virtual ~ComponentBase(){}

	virtual void DestroyData(unsigned char* data) const = 0;
	virtual void MoveData(unsigned char* source, unsigned char* destination) const = 0;
	virtual void ConstructData(unsigned char* data) const = 0;

	virtual std::size_t GetSize() const = 0;
};

template<class C>
class Component : public ComponentBase
{
public:
	virtual void DestroyData(unsigned char* data) const override;
	virtual void MoveData(unsigned char* source, unsigned char* destination) const override;
	virtual void ConstructData(unsigned char* data) const override;
	
	virtual std::size_t GetSize() const override;
	static ComponentTypeID GetTypeID();
};

#pragma endregion

#pragma region Entities

class Entity
{
public:
	explicit Entity(ECS& ecs)
		:
		mID(ecs.GetNewID()),
		mECS(ecs)
	{
		mECS.RegisterEntity(mID);
	}

	template<class C, typename... Args>
	C* Add(Args&&... args)
	{
		return mECS.AddComponent<C>(mID, std::forward<Args>(args)...);
	}

	template<class C>
	C* Add(C&& c)
	{
		return mECS.AddComponent<C>(mID, std::forward<C>(c));
	}

	EntityID GetID() const
	{
		return mID;
	}

private:
	EntityID mID;
	ECS& mECS;
};

#pragma endregion

#pragma region Archetypes

typedef unsigned char* ComponentData;

struct Archetype
{
	ArchetypeID type;
	std::vector<ComponentData> componentData;
	std::vector<EntityID> entityIDs;
	std::vector<std::size_t> componentDataSize;
};



#pragma endregion

class ECS
{
private:
	typedef std::unordered_map<ComponentTypeID, ComponentBase*> ComponentTypeIDBaseMap;

	struct Record
	{
		Archetype* archetype;
		std::size_t index;
	};

	typedef std::unordered_map<EntityID, Record> EntityArchetypeMap;
	typedef std::vector<Archetype*> ArchetypesArray;

	typedef std::unordered_map<std::uint8_t, std::vector<SystemBase*>> SystemsArrayMap;

public:
	ECS();
	~ECS();

	EntityID GetNewID();
	
	template<class C>
	void RegisterComponent();

	template<class C>
	bool IsComponentRegistered();

	void RegisterSystem(const std::uint8_t& layer, SystemBase* system);

	void RegisterEntity(const EntityID entityID);

	void RunSystems(const std::uint8_t& layer, const float elapsedMilliseconds);

	Archetype* GetArchetype(const ArchetypeID& id);

	template<class C, typename... Args>
	C* AddComponent(const EntityID& entityID, Args&&... args);

	template<class C>
	void RemoveComponent(const EntityID& entityID);

	template<class C>
	C* GetComponent(const EntityID& entityID);

	template<class C>
	bool HasComponent(const EntityID& entityID);

	void RemoveEntity(const EntityID& entityID);

	template<class... Cs>
	std::vector<EntityID> GetEntitiesWith();

private:
	EntityArchetypeMap mEntityArchetypeMap;
	ArchetypesArray mArchetypes;
	EntityID mEntityIDCounter;
	SystemsArrayMap mSystems;
	ComponentTypeIDBaseMap mComponentMap;
};

#endif