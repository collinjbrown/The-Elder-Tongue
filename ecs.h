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

		ColliderSystem* colliderSystem = new ColliderSystem();
		ComponentBlock* colliderBlock = new ComponentBlock(colliderSystem, colliderComponentID);
		componentBlocks.push_back(colliderBlock);
	}

	void Update(float deltaTime)
	{
		round++;

		if (round == 1)
		{
			Entity* bob = CreateEntity("Bob");
			Texture2D* tex = Game::main.textureMap["test"];
			ECS::main.RegisterComponent(new PositionComponent(bob, true, 0, 5), bob);
			ECS::main.RegisterComponent(new PhysicsComponent(bob, true, 0.0f, 0.0f, 0.1f, 10.0f), bob);
			ECS::main.RegisterComponent(new ColliderComponent(bob, true, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f), bob);
			ECS::main.RegisterComponent(new SpriteComponent(bob, true, tex->width, tex->height, tex), bob);

			Entity* joe = CreateEntity("Joe");
			Texture2D* tex2 = Game::main.textureMap["test"];
			ECS::main.RegisterComponent(new PositionComponent(joe, true, 0, -100), joe);
			ECS::main.RegisterComponent(new PhysicsComponent(joe, true, 0.0f, 0.0f, 0.1f, 0.0f), joe);
			ECS::main.RegisterComponent(new ColliderComponent(joe, true, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f), joe);
			ECS::main.RegisterComponent(new SpriteComponent(joe, true, tex2->width, tex2->height, tex2), joe);
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
