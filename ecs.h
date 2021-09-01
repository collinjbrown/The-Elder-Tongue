#ifndef ECS_H
#define ECS_H

#include <vector>
#include "system.h"
#include "component.h"

using namespace std;

// This might seem a little strange, so we'll have to explain why this is set up
// the way it is...

class ComponentBlock
{
public:
	System* system;
	int componentID;

	void Update(float deltaTime)
	{
		system->Update(deltaTime);
	}
	void AddComponent(Component* c)
	{
		system->AddComponent(c);
	}

	ComponentBlock(System* system, int componentID)
	{
		this->system = system;
		this->componentID = componentID;
	}
};


class ECS
{
private:
	uint32_t entityIDCounter = 0;
	int round = 0;

public:
	static ECS main;
	vector<Entity*> entities;

	vector<ComponentBlock*> componentBlocks;

	uint32_t GetID()
	{
		return ++entityIDCounter;
	}

	void Init()
	{
		// I think we're going to have to initiate every component block
		// at the beginning of the game. This might be long.

		PhysicsSystem* physicsSystem = new PhysicsSystem();
		ComponentBlock* physicsBlock = new ComponentBlock(physicsSystem, physicsComponentID);
		componentBlocks.push_back(physicsBlock);

		RenderingSystem* renderingSystem = new RenderingSystem();
		ComponentBlock* renderingBlock = new ComponentBlock(renderingSystem, spriteComponentID);
		componentBlocks.push_back(renderingBlock);
	}

	void Update(float deltaTime)
	{
		round++;

		if (round == 1)
		{
			Entity* e = CreateEntity("Bob");
			Texture2D* tex = Game::main.textureMap["test"];
			ECS::main.RegisterComponent(new PositionComponent(e, true, 0, 0), e);
			ECS::main.RegisterComponent(new PhysicsComponent(e, true, 5.0f, 5.0f, 0.1f, 0), e);
			ECS::main.RegisterComponent(new SpriteComponent(e, true, tex->width, tex->height, tex), e);
		}

		for (int i = 0; i < componentBlocks.size(); i++)
		{
			componentBlocks[i]->Update(deltaTime);
		}
	}

	Entity* CreateEntity(std::string name)
	{
		Entity* e = new Entity(GetID(), name);
		return e;
	}

	void RegisterComponent(Component* component, Entity* entity)
	{
		entity->components.push_back(component);
		entity->componentIDMap.emplace(component->ID, component);

		for (int i = 0; i < componentBlocks.size(); i++)
		{
			if (componentBlocks[i]->componentID == component->ID)
			{
				componentBlocks[i]->AddComponent(component);
				return;
			}
		}
	}
};

#endif
