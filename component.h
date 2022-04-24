#ifndef COMPONENT_H
#define COMPONENT_H

#define _USE_MATH_DEFINES

#include "renderer.h"
#include <math.h>
#include <map>
#include <vector>

class Entity;

static int positionComponentID = 1;
static int physicsComponentID = 2;
static int spriteComponentID = 3;
static int colliderComponentID = 4;
static int inputComponentID = 5;
static int animationComponentID = 6;
static int animationControllerComponentID = 7;
static int cameraFollowComponentID = 8;
static int movementComponentID = 9;
static int healthComponentID = 10;
static int duelistComponentID = 11;
static int pixelEffectsComponentID = 12;

static int dragonriderAnimControllerSubID = 1;

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
	float baseGravityMod;

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
		this->baseGravityMod = gravityMod;
	}
};

class StaticSpriteComponent : public Component
{
public:
	float width;
	float height;

	Texture2D* sprite;
	PositionComponent* pos;

	StaticSpriteComponent(Entity* entity, bool active, PositionComponent* pos, float width, float height, Texture2D* sprite)
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
	bool platform;		// Only collides on the top.
	bool onPlatform;	// Every frame that you collide with a platform,
						// this is set to true so that I don't have to
						// raycast to check if you're on a platform.
	bool collidedLastTick;
	bool climbable;

	float mass;
	float bounce;
	float friction;

	float width;
	float height;

	float offsetX;
	float offsetY;

	PositionComponent* pos;

	ColliderComponent(Entity* entity, bool active, PositionComponent* pos, bool platform, bool climbable, float mass, float bounce, float friction, float width, float height, float offsetX, float offsetY)
	{
		ID = colliderComponentID;
		this->active = active;
		this->entity = entity;
		this->pos = pos;

		this->platform = platform;
		this->onPlatform = false;
		this->collidedLastTick = false;
		this->climbable = climbable;

		this->mass = mass;
		this->bounce = bounce;
		this->friction = friction;

		this->width = width;
		this->height = height;
		
		this->offsetX = offsetX;
		this->offsetY = offsetY;
	}
};

class InputComponent : public Component
{
public:
	bool acceptInput;

	bool projecting;
	float projectionTime;
	float projectionDelay;
	int projectionDepth;

	bool releasedJump;
	float coyoteTime;
	float maxCoyoteTime;
	int jumps;
	int maxJumps;

	float lastTick;

	InputComponent(Entity* entity, bool active, bool acceptInput, float projectionDelay, float projectionDepth, float maxCoyoteTime, int maxJumps)
	{
		this->ID = inputComponentID;
		this->active = active;
		this->entity = entity;

		this->acceptInput = acceptInput;
		this->projectionDelay = projectionDelay;
		this->projectionDepth = projectionDepth;
		this->lastTick = 0.0f;
		this->projectionTime = 0.0f;
		this->projecting = false;
		this->releasedJump = true;
		this->coyoteTime = 0.0f;
		this->maxCoyoteTime = maxCoyoteTime;
		this->maxJumps = maxJumps;
	}
};

class MovementComponent : public Component
{
public:
	float acceleration;
	float maxSpeed;
	float maxJumpHeight;
	float stabDepth;

	bool jumping;
	bool preparingToJump;

	bool canMove;
	float lastMoveAttempt;
	float moveAttemptDelay;

	bool canClimb;
	bool shouldClimb;
	bool climbing;

	MovementComponent(Entity* entity, bool active, float acceleration, float maxSpeed, float maxJumpHeight, float stabDepth, float moveAttemptDelay, bool canMove, bool canClimb, bool shouldClimb)
	{
		this->ID = movementComponentID;
		this->entity = entity;
		this->active = active;

		this->acceleration = acceleration;
		this->maxSpeed = maxSpeed;
		this->maxJumpHeight = maxJumpHeight;
		this->canMove = canMove;
		this->jumping = false;
		this->preparingToJump = false;
		this->stabDepth = stabDepth;

		this->canClimb = canClimb;
		this->shouldClimb = shouldClimb;
		this->climbing = false;
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

class AnimationComponent : public Component
{
public:
	int activeX;
	int activeY;

	bool flipped;

	std::string activeAnimation;
	map<std::string, Animation2D*> animations;
	PositionComponent* pos;

	float lastTick;

	void SetAnimation(std::string s)
	{
		if (animations[s] != NULL)
		{
			activeAnimation = s;
			activeX = 0;
			activeY = animations[s]->rows - 1;
			lastTick = 0;
		}
	}

	void AddAnimation(std::string s, Animation2D* anim)
	{
		animations.emplace(s, anim);
	}

	AnimationComponent(Entity* entity, bool active, PositionComponent* pos, Animation2D* idleAnimation, std::string animationName)
	{
		this->ID = animationComponentID;
		this->entity = entity;
		this->active = active;

		lastTick = 0;
		activeX = 0;
		activeY = 0;
		flipped = false;

		this->pos = pos;
		activeAnimation = animationName;
		animations.emplace(animationName, idleAnimation);
	}
};

class AnimationControllerComponent : public Component
{
public:
	AnimationComponent* animator;
	int subID;
};

class DragonriderAnimationControllerComponent : public AnimationControllerComponent
{
public:
	DragonriderAnimationControllerComponent(Entity* entity, bool active, AnimationComponent* animator)
	{
		this->ID = animationControllerComponentID;
		this->subID = dragonriderAnimControllerSubID;
		this->entity = entity;
		this->active = active;

		this->animator = animator;
	}
};

class HealthComponent : public Component
{
public:
	// Rather than having health, the Elder Tongue
	// focuses on fatigue and other such things;
	// most humans die in one (direct) hit from a sword,
	// unless they are protected by magic,
	// but they will accrue debuffs as they collide with
	// things and exert themselves.
	// They may also bleed out over a longer period of time
	// from smaller wounds they receive.

	float health;
	float fatigue;
	float blood;

	float bleeding;
	float coalgulationRate;

	bool dead;

	HealthComponent(Entity* entity, bool active, float health, float fatigue, float blood, float bleeding, float coagulationRate, bool dead)
	{
		this->ID = healthComponentID;
		this->entity = entity;
		this->active = active;

		this->health = health;
		this->fatigue = fatigue;
		this->blood = blood;

		this->bleeding = bleeding;
		this->coalgulationRate = coagulationRate;

		this->dead = dead;
	}
};

class DuelistComponent : public Component
{
public:
	bool hasSword;
	bool isDrawn;
	bool isAttacking;

	float lastTick;

	DuelistComponent(Entity* entity, bool active, bool hasSword, bool isDrawn)
	{
		this->ID = duelistComponentID;
		this->entity = entity;
		this->active = active;

		this->hasSword = hasSword;
		this->isDrawn = isDrawn;
		this->isAttacking = false;
		lastTick = 0;
	}
};

#endif
