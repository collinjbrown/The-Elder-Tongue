#include "ECS.h"

template<class C>
void Component<C>::DestroyData(unsigned char* data) const
{
    C* dataLocation = std::launder(reinterpret_cast<C*>(data));

    dataLocation->~C();
}

template<class C>
void Component<C>::ConstructData(unsigned char* data) const
{
    new (&data[0]) C();
}

template<class C>
void Component<C>::MoveData(unsigned char* source, unsigned char* destination) const
{
    new (&destination[0]) C(std::move(*reinterpret_cast<C*>(source)));
}

template<class C>
std::size_t Component<C>::GetSize() const
{
    return sizeof(C);
}

template<class C>
ComponentTypeID Component<C>::GetTypeID()
{
    return TypeIdGenerator<ComponentBase>::GetNewID<C>();
}

ECS::ECS()
    :
    mEntityIDCounter(1)
{}

EntityID ECS::GetNewID()
{
    return mEntityIDCounter++;
}

template<class C>
void ECS::RegisterComponent()
{
    ComponetTypeID componentTypeID = Component<C>::GetTypeID();

    if (mComponentMap.contains(componentTypeID))
    {
        return;
    }

    mComponentMap.emplace(componentTypeID, new Component<C>);
}

void ECS::RegisterSystem(const std::uint8_t& layer, SystemBase* system)
{
    mSystems[layer].push_back(system);
}

void ECS::RegisterEntity(const EntityID entityID)
{
    Record dummyRecord;
    dummyRecord.archetype = nullptr;
    dummyRecord.index = 0;
    mEntityArchetypeMap[entityID] = dummyRecord;
}

void ECS::RunSystems(const std::uint8_t& layer, const float elapsedMilliseconds)
{
    for (SystemBase* system : mSystems[layer])
    {
        const ArchetypeID& key = system->GetKey();

        for (Archetype* archetype : mArchetypes)
        {
            if (std::includes(archetype->type.begin(), archetype->type.end(), key.begin(), key.end()))
            {
                system->DoAction(elapsedMilliseconds, archetype);
            }
        }
    }
}

Archetype* ECS::GetArchetype(const ArchetypeID& id)
{
    for (Archetype* archetype : mArchetypes)
    {
        if (archetype->type == id)
        {
            return archetype;
        }
    }

    Archetype* newArchetype = new Archetype;
    newArchetype->type = id;
    mArchetypes.push_back(newArchetype);

    for (ArchetypeID::size_type i = 0; i < id.size(); ++i)
    {
        newArchetype->componentData.push_back(new unsigned char[0]);
        newArchetype->componentDataSize.push_back(0);
    }

    return newArchetype;
}

template<class C, typename... Args>
C* ECS::AddComponent(const EntityID& entityID, Args&&... args)
{
    // This is long, so I'll explain each part.

    // Get a new component ID.
    ComponentTypeID newCompTypeID = Component<C>::GetTypeID();

    // If the component has not been pre-registered, game over.
    // Make sure you register all components before you add them.
    // We may change this later.
    if (!IsComponentRegistered<C>())
    {;
        throw std::_Xruntime_error("Unregistered component error.");
        return nullptr;
    }

    const std::size_t& compDataSize = mComponentMap[newCompTypeID]->GetSize();

    // Record the archetype to which the entity belonged.
    Record& record = mEntityArchetypeMap[entityID];
    Archetype* oldArchetype = record.archetype;

    C* newComponent = nullptr;

    Archetype* newArchetype = nullptr;

    if (oldArchetype)
    {
        // Checks to see if the entity already includes the component; if so, return null.
        if (std::find(oldArchetype->type.begin(), oldArchetype->type.end(), newCompTypeID) != oldArchetype->type.end())
        {
            return nullptr;
        }

        // Come up with a new archetype ID and add it.
        ArchetypeID newArchetypeID = oldArchetype->type;
        newArchetypeID.push_back(newCompTypeID);
        std::sort(newArchetypeID.begin(), newArchetypeID.end());

        newArchetype = GetArchetype(newArchetypeID);

        for (std::size_t j = 0; j < newArchetypeID.size(); ++j)
        {
            // Move data around in order to fit all our components together.
            const ComponentTypeID& newCompID = newArchetypeID[j];
            const ComponentBase* const newComp = mComponentMap[newCompID];
            const std::size_t& newCompDataSize = newComp->GetSize();

            std::size_t currentSize = newArchetype->entityIDs.size() * newCompDataSize;
            std::size_t newSize = currentSize + newCompDataSize;

            if (newSize > newArchetype->componentDataSize[j])
            {
                newArchetype->componentDataSize[j] *= 2;
                newArchetype->componentDataSize[j] += newCompDataSize;
                unsigned char* newData = new unsigned char[newArchetype->componentDataSize[j]];

                for (std::size_t e = 0; e < newArchetype->entityIDs.size(); ++e)
                {
                    newComp->MoveData(&newArchetype->componentData[j][e * newCompDataSize],
                                      &newData[e * newCompDataSize]);
                    newComp->DestroyData(&newArchetype->componentData[j][e * newCompDataSize]);
                }

                delete[] newArchetype->componentData[j];

                newArchetype->componentData[j] = newData;
            }


            bool avoid = false;
            ArchetypeID oldArchetypeID = oldArchetype->type;

            // Move the data to the new archetype.
            for (std::size_t i = 0; i < oldArchetype->type.size(); ++i)
            {
                const ComponentTypeID& oldCompID = oldArchetype->type[i];

                if (oldCompID == newCompID)
                {
                    const ComponentBase* const oldComp = mComponentMap[oldCompID];

                    const std::size_t& oldCompDataSize = oldComp->GetSize();

                    oldComp->MoveData(&oldArchetype->componentData[i][record.index * oldCompDataSize],
                                      &newArchetype->componentData[j][currentSize]);
                    oldComp->DestroyData(&oldArchetype->componentData[i][record.index * oldCompDataSize]);

                    // I'm told this is of the Devil, so we should change this later.
                    // goto cnt;
                    // Hopefully this achieves the same result, as I think it does.
                    avoid = true;
                    break;
                }
            }

            if (!avoid)
            {
                newComponent = new (&newArchetype->componentData[j][currentSize])
                    C(std::forward<Args>(args)...);
            }

            // cnt:;
        }


        if (!oldArchetype->entityIDs.empty())
        {
            for (std::size_t i = 0; i < oldArchetype->type.size(); ++i)
            {
                const ComponentTypeID& oldCompTypeID = oldArchetype->type[i];

                if (oldCompTypeID == newCompTypeID)
                {
                    ComponentBase* removeWrapper = mComponentMap[newComponentID];
                    removeWrapper->DestroyData(&oldArchetype->componentData[i][record.index * sizeof(C)]);
                }

                const ComponentBase* const oldComp = mComponentMap[oldCompTypeID];

                const std::size_t& oldCompDataSize = oldComp->GetSize();

                std::size_t currentSize = oldArchetype->entityIDs.size() * oldCompDataSize;
                std::size_t newSize = currentSize - oldCompDataSize;
                unsigned char* newData = new unsigned char[oldArchetype->componentDataSize[i] - oldCompDataSize];
                oldArchetype->componentDataSize[i] -= oldCompDataSize;

                for (std::size_t e = 0, ei = 0; e < oldArchetype->entityIDs.size(); ++e)
                {
                    if (e == record.index)
                    {
                        continue;
                    }

                    oldComp->MoveData(&oldArchetype->componentData[i][e * oldCompDataSize],
                                      &newData[ei * oldCompDataSize]);
                    oldComp->DestroyData(&oldArchetype->componentData[i][e * oldCompDataSize]);

                    ++ei;
                }

                delete[] oldArchetype->componentData[i];

                oldArchetype->componentData[i] = newData;
            }
        }

        std::vector<EntityID>::iterator willBeRemoved = std::find(oldArchetype->entityIDs.begin(),
                                                                  oldArchetype->entityIDs.end(),
                                                                  entityID);

        std::for_each(willBeRemoved, oldArchetype->entityIDs.end(), [this, &oldArchetype](const EntityID& eid)
            {
                Record& moveR = mEntityArchetypeMap[eid];
                --moveR.index;
            }
        );

        oldArchetype->entityIDs.erase(willBeRemoved);
    }
    else
    {
        // This is the first component added to this entity
        // which makes the whole process easier.

        // Create a new archetypeID.
        ArchetypeID newArchetypeID(1, newCompTypeID);

        const ComponentBase* const newComp = mComponentMap[newCompTypeID];

        newArchetype = GetArchetype(newArchetypeID);

        std::size_t currentSize = newArchetype->entityIDs.size() * compDataSize;
        std::size_t newSize = currentSize + compDataSize;

        // We need to move some data around because of the data size.
        if (newSize > newArchetype->componentDataSize[0])
        {
            // Reallocate
            newArchetype->componentDataSize[0] *= 2;
            newArchetype->componentDataSize[0] += compDataSize;

            unsigned char* newData = new unsigned char[newArchetype->componentDataSize[0]];

            // Move Data
            for (std::size_t e = 0; e < newArchetype->entityIDs.size(); ++e)
            {
                newComp->MoveData(&newArchetype->componentData[0][e * compDataSize],
                                  &newData[e * compDataSize]);
                newComp->DestroyData(&newArchetype->componentData[0][e * compDataSize]);
            }

            delete[](newArchetype->componentData[0]);

            newArchetype->componentData[0] = newData;
        }

        // Update the archetype's list and the record.
        newComponent = new (&newArchetype->componentData[0][currentSize])
                            C(std::forward<Args>(args)...);
    }

    newArchetype->entityIDs.push_back(entityID);
    record.index = newArchetype->entityIDs.size() - 1;
    record.archetype = newArchetype;

    return newComponent;
}

template<class C>
void ECS::RemoveComponent(const EntityID& entityID)
{
    if (!IsComponentRegistered<C>())
    {
        return;
    }

    ComponentTypeID compTypeID = Component<C>::GetTypeID();

    if (!mEntityArchetypeMap.contains(entityID))
    {
        return;
    }

    Record& record = mEntityArchetypeMap[entityID];

    Archetype* oldArchetype = record.archetype;

    if (!oldArchetype)
    {
        return;
    }

    if (std::find(oldArchetype->type.begin(), oldArchetype->type.end(), compTypeID) == oldArchetype->type.end())
    {
        return;
    }

    ArchetypeID newArchetypeID = oldArchetype->type;
    newArchetypeID.erase(std::remove(newArchetypeID.begin(), newArchetypeID.end(), compTypeID), newArchetypeID.end());
    std::sort(newArchetypeID.begin(), newArchetypeID.end());

    Archetype* newArchetype = GetArchetype(newArchetypeID);

    for (std::size_t j = 0; j < newArchetypeID.size(); ++j)
    {
        const ComponentTypeID& newCompID = newArchetypeID[j];

        const ComponentBase* const newComp = mComponentMap[newCompID];

        const std::size_t& newCompDataSize = newComp->GetSize();

        std::size_t currentSize = newArchetype->entityIDs.size() * newCompDataSize;
        std::size_t newSize = currentSize + newCompDataSize;

        if (newSize > newArchetype->componentDataSize[j])
        {
            newArchetype->componentDataSize[j] *= 2;
            newArchetype->componentDataSize[j] += newCompDataSize;

            unsigned char* newData = new unsigned char[newSize];
            
            for (std::size_t e = 0; e < newArchetype->entityIDs.size(); ++e)
            {
                newComp->MoveData(&newArchetype->componentData[j][e * newCompDataSize],
                                  &newData[e * newCompDataSize]);
                newComp->DestroyData(&newArchetype->componentData[j][e * newCompDataSize]);
            }

            delete[] newArchetype->componentData[j];
            newArchetype->componentData[j] = newData;
        }

        newComp->ConstructData(&newArchetype->componentData[j][currentSize]);

        ArchetypeID oldArchetypeID = oldArchetype->type;

        for (std::size_t i = 0; i < oldArchetype->type.size(); ++i)
        {
            const ComponentTypeID& oldCompID = oldArchetype->type[i];

            if (oldCompID == newCompID)
            {
                const std::size_t& oldCompDataSize = mComponentMap[oldCompID]->GetSize();

                ComponentBase* removeWrapper = mComponentMap[oldCompID];
                removeWrapper->MoveData(&oldArchetype->componentData[i][record.index * oldCompDataSize],
                                        &newArchetype->componentData[j][currentSize]);
                removeWrapper->DestroyData(&oldArchetype->componentData[i][record.index * oldCompDataSize]);

                break;
            }
        }
    }

    for (std::size_t i = 0; i < oldArchetype->type.size(); ++i)
    {
        const ComponentTypeID& oldCompTypeID = oldArchetype->type[i];

        if (oldCompTypeID == compTypeID)
        {
            ComponentBase* removeWrapper = mComponentMap[compTypeID];
            removeWrapper->DestroyData(&oldArchetype->componentData[i][record.index * sizeof(C)]);
        }

        const ComponentBase* const oldComp = mComponentMap[oldCompTypeID];

        const std::size_t& oldCompDataSize = oldComp->GetSize();

        std::size_t currentSize = oldArchetype->entityIDs.size() * oldCompDataSize;
        std::size_t newSize = currentSize - oldCompDataSize;
        unsigned char* newData = new unsigned char[oldArchetype->componentDataSize[i] - oldCompDataSize];
        oldArchetype->componentDataSize[i] -= oldCompDataSize;

        for (std::size_t e = 0, ei = 0; e < oldArchetype->entityIDs.size(); ++i)
        {
            if (e == record.index)
            {
                continue;
            }

            oldComp->MoveData(&oldArchetype->componentData[i][e * oldCompDataSize],
                              &newData[ei * oldCompDataSize]);
            oldComp->DestroyData(&oldArchetype->componentData[i][e * oldCompDataSize]);

            ++ei;
        }

        delete[] oldArchetype->componentData[i];

        oldArchetype->componentData[i] = newData;
    }

    std::vector<EntityID>::iterator willBeRemoved = std::find(oldArchetype->entityIDs.begin(),
                                                              oldArchetype->entityIDs.end(),
                                                              entityID);
    std::for_each(willBeRemoved, oldArchetype->entityIDs.end(), [this, &oldArchetype](const EntityID& eid)
        {
            Record& moveR = mEntityArchetypeMap[eid];
            --moveR.index;
        }
    );

    oldArchetype->entityIDs.erase(std::remove(oldArchetype->entityIDs.begin(),
                                              oldArchetype->entityIDs.end(),
                                              entityID), oldArchetype->entityIDs.end());

    newArchetype->entityIDs.push_back(entityID);
    record.index = newArchetype->entityIDs.size() - 1;
    record.archetype = newArchetype;
}


void ECS::RemoveEntity(const EntityID& entityID)
{
    // This entity doesn't exist.
    if (!mEntityArchetypeMap.contains(entityID))
    {
        return;
    }

    Record& record = mEntityArchetypeMap[entityID];

    Archetype* oldArchetype = record.archetype;

    if (!oldArchetype)
    {
        mEntityArchetypeMap.erase(entityID);
        return;
    }

    for (std::size_t i = 0; i < oldArchetype->type.size(); ++i)
    {
        const ComponentTypeID& oldCompID = oldArchetype->type[i];

        const ComponentBase* const comp = mComponentMap[oldCompID];

        const std::size_t& compSize = comp->GetSize();

        comp->DestroyData(&oldArchetype->componentData[i][record.index * compSize]);
    }

    for (std::size_t i = 0; i < oldArchetype->type.size(); ++i)
    {
        const ComponentTypeID& oldCompID = oldArchetype->type[i];

        const ComponentBase* const oldComp = mComponentMap[oldCompID];

        const std::size_t& oldCompDataSize = oldComp->GetSize();

        std::size_t currentSize = oldArchetype->entityIDs.size() * oldCompDataSize;
        std::size_t newSize = currentSize - oldCompDataSize;

        unsigned char* newData = new unsigned char[oldArchetype->componentDataSize[i] - oldCompDataSize];

        oldArchetype->componentDataSize[i] -= oldCompDataSize;

        for (std::size_t e = 0, ei = 0; e < oldArchetype->entityIDs.size(); ++e)
        {
            if (e == record.index)
            {
                continue;
            }

            oldComp->MoveData(&oldArchetype->componentData[i][e * oldCompDataSize],
                              &newData[ei * oldCompDataSize]);

            oldComp->DestroyData(&oldArchetype->componentData[i][e * oldCompDataSize]);

            ++ei;
        }

        delete[] oldArchetype->componentData[i];

        oldArchetype->componentData[i] = newData;
    }

    mEntityArchetypeMap.erase(entityID);

    std::vector<EntityID>::iterator willBeRemoved = std::find(oldArchetype->entityIDs.begin(),
                                                              oldArchetype->entityIDs.end(),
                                                              entityID);

    std::for_each(willBeRemoved, oldArchetype->entityIDs.end(), [this, &oldArchetype, &entityID](const EntityID& eid)
        {
            if (eid == entityID)
            {
                return;
            }

            Record& moveR = mEntityArchetypeMap[eid];
            moveR.index -= 1;
        }
    );

    oldArchetype->entityIDs.erase(willBeRemoved);
}

ECS::~ECS()
{
    for (Archetype* archetype : mArchetypes)
    {
        for (std::size_t i = 0; i < archetype->type.size(); ++i)
        {
            const ComponentBase* const comp = mComponentMap[archetype->type[i]];
            const std::size_t& dataSize = comp->GetSize();

            for (std::size_t e = 0; e < archetype->entityIDs.size(); ++e)
            {
                comp->DestroyData(&archetype->componentData[i][e * dataSize]);
            }

            delete[] archetype->componentData[i];
        }

        delete archetype;
    }

    for (ComponentTypeIDBaseMap::value_type& p : mComponentMap)
    {
        delete p.second;
    }
}