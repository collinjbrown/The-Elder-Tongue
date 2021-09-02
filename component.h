#ifndef COMPONENT_H
#define COMPONENT_H

#define _USE_MATH_DEFINES

#include "renderer.h"
#include "texture_2D.h"
#include <math.h>

class Entity;

static int positionComponentID = 1;
static int physicsComponentID = 2;
static int spriteComponentID = 3;
static int colliderComponentID = 4;
static int movementComponentID = 5;
static int cameraFollowComponentID = 6;

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
	bool stat;

	float x;
	float y;
	float z;

	float rotation; // In degrees.

	// This is the only component that should have any logic in it.
	// I'm making an exception here for simplicity's sake.

	glm::vec2 Rotate(glm::vec2 point)
	{
		glm::vec3 forward = glm::vec3();
		glm::vec3 up = glm::vec3();
		glm::vec3 right = glm::vec3();

		if (rotation != 0)
		{
			float radians = rotation * (M_PI / 180.0f);

			forward = glm::vec3(0, 0, 1);
			right = glm::vec3(cos(radians), sin(radians), 0);
			up = glm::cross(forward, right);
		}
		else
		{
			up = glm::vec3(0, 1, 0);
			right = glm::vec3(1, 0, 0);
		}

		return RelativeLocation(point, up, right);
	}

	glm::vec2 RelativeLocation(glm::vec2 p, glm::vec2 up, glm::vec2 right)
	{
		return glm::vec2((p.x * right.x) + (p.y * up.x), (p.x * right.y) + (p.y * up.y));
	}

	PositionComponent(Entity* entity, bool active, bool stat, float x, float y, float rotation)
	{
		ID = positionComponentID;
		this->active = active;
		this->entity = entity;
		this->stat = stat;
		this->x = x;
		this->y = y;
		this->z = 0;
		this->rotation = rotation;
	}
};

class PhysicsComponent : public Component
{
public:
	float velocityX;
	float velocityY;

	float rotVelocity;

	float drag;					// How much velocity one loses each turn.
	float gravityMod;			// How much gravity should one experience.

	PositionComponent* pos;

	PhysicsComponent(Entity* entity, bool active, PositionComponent* pos, float vX, float vY, float vR, float drag, float gravityMod)
	{
		ID = physicsComponentID;
		this->active = active;
		this->entity = entity;
		this->pos = pos;

		this->velocityX = vX;
		this->velocityY = vY;
		this->rotVelocity = vR;
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
	PositionComponent* pos;

	SpriteComponent(Entity* entity, bool active, PositionComponent* pos, float width, float height, Texture2D* sprite)
	{
		ID = spriteComponentID;
		this->active = active;
		this->entity = entity;
		this->pos = pos;

		this->width = width;
		this->height = height;
		this->sprite = sprite;
	}
};


class ColliderComponent : public Component
{
public:
	float mass;
	float bounce;
	float friction;

	float width;
	float height;

	float offsetX;
	float offsetY;

	PositionComponent* pos;

	ColliderComponent(Entity* entity, bool active, PositionComponent* pos, float mass, float bounce, float friction, float width, float height, float offsetX, float offsetY)
	{
		ID = colliderComponentID;
		this->active = active;
		this->entity = entity;
		this->pos = pos;

		this->mass = mass;
		this->bounce = bounce;
		this->friction = friction;

		this->width = width;
		this->height = height;
		
		this->offsetX = offsetX;
		this->offsetY = offsetY;
	}
};

class MovementComponent : public Component
{
public:
	float acceleration;
	float maxSpeed;
	float maxJumpHeight;

	MovementComponent(Entity* entity, bool active, float acceleration, float maxSpeed, float maxJumpHeight)
	{
		this->ID = movementComponentID;
		this->active = active;
		this->entity = entity;

		this->acceleration = acceleration;
		this->maxSpeed = maxSpeed;
		this->maxJumpHeight = maxJumpHeight;
	}
};

class CameraFollowComponent : public Component
{
public:
	float speed;

	CameraFollowComponent(Entity* entity, bool active, float speed)
	{
		this->ID = cameraFollowComponentID;
		this->active = active;
		this->entity = entity;

		this->speed = speed;
	}
};

#endif
