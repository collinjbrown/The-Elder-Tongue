#ifndef COMPONENT_H
#define COMPONENT_H

#include "renderer.h"
#include "texture_2D.h"

class Entity;

static int positionComponentID = 1;
static int physicsComponentID = 2;
static int spriteComponentID = 3;

class Component
{
public:
	bool active;
	Entity* entity;
	int ID;
};

class PositionComponent : public Component
{
public:
	float x;
	float y;
	float z;

	PositionComponent(Entity* entity, bool active, float x, float y)
	{
		ID = positionComponentID;
		this->active = active;
		this->entity = entity;
		this->x = x;
		this->y = y;
	}
};

class PhysicsComponent : public Component
{
public:
	float velocityX;
	float velocityY;

	float drag;					// How much velocity one loses each turn.
	float gravityMod;			// How much gravity should one experience.

	PhysicsComponent(Entity* entity, bool active, float vX, float vY, float drag, float gravityMod)
	{
		ID = physicsComponentID;
		this->active = active;
		this->entity = entity;
		this->velocityX = vX;
		this->velocityY = vY;
		this->drag = drag;
		this->gravityMod = gravityMod;
	}
};

class SpriteComponent : public Component
{
public:
	float width;
	float height;

	Texture2D* sprite;

	SpriteComponent(Entity* entity, bool active, float width, float height, Texture2D* sprite)
	{
		ID = spriteComponentID;
		this->active = active;
		this->entity = entity;
		this->width = width;
		this->height = height;
		this->sprite = sprite;
	}
};

#endif