#ifndef COMPONENT_H
#define COMPONENT_H

#define _USE_MATH_DEFINES

#include "renderer.h"
#include "particleengine.h"
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
static int damageComponentID = 12;
static int particleComponentID = 13;

static int dragonriderAnimControllerSubID = 1;

enum EntityClass { player, enemy, object };

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

	glm::vec2 Rotate(glm::vec2 point);

	glm::vec2 RelativeLocation(glm::vec2 p, glm::vec2 up, glm::vec2 right);

	PositionComponent(Entity* entity, bool active, bool stat, float x, float y, float rotation);
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

	PhysicsComponent(Entity* entity, bool active, PositionComponent* pos, float vX, float vY, float vR, float drag, float gravityMod);
};

class StaticSpriteComponent : public Component
{
public:
	float width;
	float height;

	Texture2D* sprite;
	PositionComponent* pos;

	StaticSpriteComponent(Entity* entity, bool active, PositionComponent* pos, float width, float height, Texture2D* sprite);
};

class ColliderComponent : public Component
{
public:
	bool platform;		// Only collides on the top.
	bool onPlatform;	// Every frame that you collide with a platform,
						// this is set to true so that I don't have to
						// raycast to check if you're on a platform.
	bool onewayPlatform;
	
	bool collidedLastTick;
	bool climbable;

	bool trigger;
	bool takesDamage;
	bool doesDamage;

	EntityClass entityClass;

	float mass;
	float bounce;
	float friction;

	float width;
	float height;

	float offsetX;
	float offsetY;

	PositionComponent* pos;

	ColliderComponent(Entity* entity, bool active, PositionComponent* pos, bool platform, bool onewayPlatform, bool climbable, bool trigger, bool takesDamage, bool doesDamage, EntityClass entityClass, float mass, float bounce, float friction, float width, float height, float offsetX, float offsetY);
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

	float lastProjectile;
	float projectileDelay;

	float slashSpeed;
	float projectileSpeed;

	InputComponent(Entity* entity, bool active, bool acceptInput, float projectionDelay, float projectionDepth, float maxCoyoteTime, int maxJumps, float projectileDelay, float slashSpeed, float projectileSpeed);
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

	float maxClimbHeight;
	float minClimbHeight;

	MovementComponent(Entity* entity, bool active, float acceleration, float maxSpeed, float maxJumpHeight, float stabDepth, float moveAttemptDelay, bool canMove, bool canClimb, bool shouldClimb);
};

class CameraFollowComponent : public Component
{
public:
	float speed;

	CameraFollowComponent(Entity* entity, bool active, float speed);
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

	void SetAnimation(std::string s);

	void AddAnimation(std::string s, Animation2D* anim);

	AnimationComponent(Entity* entity, bool active, PositionComponent* pos, Animation2D* idleAnimation, std::string animationName);
};

class AnimationControllerComponent : public Component
{
public:
	AnimationComponent* animator;
	int subID;
};

class PlayerAnimationControllerComponent : public AnimationControllerComponent
{
public:
	PlayerAnimationControllerComponent(Entity* entity, bool active, AnimationComponent* animator);
};

class HealthComponent : public Component
{
public:
	float health;

	bool dead;

	HealthComponent(Entity* entity, bool active, float health, bool dead);
};

class DuelistComponent : public Component
{
public:
	bool hasSword;
	bool isDrawn;
	bool isAttacking;

	float lastTick;

	DuelistComponent(Entity* entity, bool active, bool hasSword, bool isDrawn);
};

class DamageComponent : public Component
{
public:
	bool hasLifetime;
	float lifetime;
	bool showAfterUses;
	bool limitedUses;
	int uses;
	float damage;

	// There's definitely a more elegant way to handle this,
	// but I wonder if it would be worth it to implement it.
	bool damagesPlayers;
	bool damagesEnemies;
	bool damagesObjects;

	DamageComponent(Entity* entity, bool active, bool hasLifetime, float lifetime, bool showAfterUses, bool limitedUses, int uses, float damage, bool damagesPlayers, bool damagesEnemies, bool damagesObjects);
};

class ParticleComponent : public Component
{
public:
	float lastTick;
	float tickRate;

	float xOffset;
	float yOffset;
	int number;
	Element element;
	float minLifetime;
	float maxLifetime;

	ParticleComponent(Entity* entity, bool active, float tickRate, float xOffset, float yOffset, int number, Element element, float minLifetime, float maxLifetime);
};

#endif
