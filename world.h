#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include "system.h"

using namespace std;

// I am really uncertain whether I want to go about working on a real ECS architecture
// or whether I want to just use OOD to get the game functioning.
// I'm leaning towards the latter; we don't really need ECS, and while it might be nice
// for future uses of this game's framework, I don't think it is worth the time investment
// that it would take to get working.

// We are, however, going to keep each type of component in its own vector here, updating
// those rather than individual entities. This should increase performance.


class World
{
private:
	uint32_t idCounter = 0;

public:
	static World main;
	vector<Entity> entities;

	// Systems
	PhysicsSystem* physicsSystem;
	RenderingSystem* renderingSystem;

	#pragma region IDs & Init
	uint32_t GetID()
	{
		return ++idCounter;
	}

	void Init()
	{
		// Add the physics system.
		physicsSystem = new PhysicsSystem();

		// Add the rendering system.
		renderingSystem = new RenderingSystem();
	}
	#pragma endregion

	#pragma region Add Components
	PositionComponent* AddPosition(Entity* entity, float x, float y)
	{
		PositionComponent* newPosition = new PositionComponent(x, y);
		newPosition->entity = entity;
		return newPosition;
	}

	PhysicsComponent* AddPhysics(Entity* entity, float vX, float vY, float g, PositionComponent* pos)
	{
		PhysicsComponent* newPhysics = new PhysicsComponent(vX, vY, g, pos);
		newPhysics->entity = entity;
		physicsSystem->vels.push_back(*newPhysics);
		return newPhysics;
	}

	SpriteComponent* AddSprite(Entity* entity, float width, float height, Texture2D* texture, PositionComponent* pos)
	{
		SpriteComponent* newSprite = new SpriteComponent(width, height, texture, pos);
		newSprite->entity = entity;
		renderingSystem->sprites.push_back(*newSprite);
		return newSprite;
	}
	#pragma endregion

	#pragma region Update
	void Update()
	{
		physicsSystem->Update();
		renderingSystem->Update();
	}
	#pragma endregion

	#pragma region Entities
	Entity* CreateEntity()
	{
		Entity* e = new Entity(GetID(), "New Entity");
		entities.push_back(*e);
		return e;
	}
	#pragma endregion
};

#endif
