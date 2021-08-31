#ifndef COMPONENT_H
#define COMPONENT_H

#include "entity.h"
#include "renderer.h"
#include "texture_2D.h"

class Component
{
public:
	bool active;
	Entity* entity;
};

class PositionComponent : public Component
{
public:
	float x;
	float y;

	PositionComponent(float x, float y)
	{
		this->x = x;
		this->y = y;
	}
};

class PhysicsComponent : public Component
{
public:
	float velocityX;
	float velocityY;
	float gravityMod;
	PositionComponent* position;

	PhysicsComponent(float vX, float vY, float gravMod, PositionComponent* position)
	{
		this->velocityX = vX;
		this->velocityY = vY;
		this->gravityMod = gravMod;
		this->position = position;
	}
};

class SpriteComponent : public Component
{
public:
	float width;
	float height;

	Texture2D* texture;
	PositionComponent* position;

	SpriteComponent(float width, float height, Texture2D* texture, PositionComponent* position)
	{
		this->width = width;
		this->height = height;
		this->texture = texture;
		this->position = position;
	}
};

#endif