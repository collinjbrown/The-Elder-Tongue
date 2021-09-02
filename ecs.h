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

		MovementSystem* movementSystem = new MovementSystem();
		ComponentBlock* movementBlock = new ComponentBlock(movementSystem, movementComponentID);
		componentBlocks.push_back(movementBlock);
	}

	void Update(float deltaTime)
	{
		round++;

		if (round == 1)
		{
			Entity* player = CreateEntity("The Player");
			Texture2D* tex2 = Game::main.textureMap["test"];
			ECS::main.RegisterComponent(new PositionComponent(player, true, false, 0, -100, 0.0f), player);
			ECS::main.RegisterComponent(new PhysicsComponent(player, true, (PositionComponent*)player->componentIDMap[positionComponentID], 0.0f, 0.0f, 0.0f, 0.1f, 100.0f), player);
			ECS::main.RegisterComponent(new ColliderComponent(player, true, (PositionComponent*)player->componentIDMap[positionComponentID], 1.0f, 1.0f, 1.0f, 50.0f, 75.0f, 0.0f, 2.0f), player);
			ECS::main.RegisterComponent(new MovementComponent(player, true, 10.0f, 10.0f), player);
			ECS::main.RegisterComponent(new SpriteComponent(player, true, (PositionComponent*)player->componentIDMap[positionComponentID], tex2->width, tex2->height, tex2), player);

			Entity* floor = CreateEntity("floor");
			Texture2D* tex3 = Game::main.textureMap["blank"];
			ECS::main.RegisterComponent(new PositionComponent(floor, true, true, 0, -200, 0.0f), floor);
			ECS::main.RegisterComponent(new PhysicsComponent(floor, true, (PositionComponent*)floor->componentIDMap[positionComponentID], 0.0f, 0.0f, 0.0f, 0.1f, 0.0f), floor);
			ECS::main.RegisterComponent(new ColliderComponent(floor, true, (PositionComponent*)floor->componentIDMap[positionComponentID], 1000.0f, 0.0f, 1.0f, 1000.0f, 75.0f, 0.0f, 2.0f), floor);
			ECS::main.RegisterComponent(new SpriteComponent(floor, true, (PositionComponent*)floor->componentIDMap[positionComponentID], tex3->width * 100, tex3->height * 4.5f, tex3), floor);
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
