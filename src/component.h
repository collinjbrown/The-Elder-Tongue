// This header exists solely to hold all the basic components and their variables;
// their implementations can be found in ecs.cpp.

#ifndef COMPONENT_H
#define COMPONENT_H

#define _USE_MATH_DEFINES

#include "renderer.h"
#include "particleengine.h"
#include <math.h>
#include <map>
#include <vector>

class Entity;

// There is surely a more elegant way to handle IDs; we could dynamically assign them at runtime,
// but honestly, this works so I'm not particularly compelled to change it.
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
static int aiComponentID = 14;
static int bladeComponentID = 15;
static int imageComponentID = 16;

static int lilyAnimControllerSubID = 1;

enum EntityClass { player, enemy, object };

class Component
{
	// This just contains the basic data universal to all components.
public:
	bool active;
	Entity* entity;
	int ID;
};

class PositionComponent : public Component
{
	// The position component lacked, for quite some time, its own system,
	// but I've recently carved the position-modifying portion of the physics system out
	// and given it its own dedicated system (note, though, that position and physics
	// components were still independent of one another).
public:
	// Stat is just short for static. It tells the game whether or not it should make certain
	// assumptions about the object's relationship to other objects. For example, the collision system
	// doesn't calculate collisions for static objects.
	bool stat;

	// These are just the basic x, y, and z coordinates for the object. Too clarify,
	// higher z-coordinates are placed on top of those with lower ones, so an object with
	// a z-coordinate of 100 will appear in front of one with a z-coordinate of 0. We could
	// easily change this by flipping the '<' to an '>' in the sprite rendering system and
	// the animation rendering system.
	float x;
	float y;
	float z;

	// Rotation should only be applied to objects without colliders. I want to figure out, at some point,
	// some efficient algorithm for calculating continuous collisions for rotating objects, but our current
	// algorithm expects all colliders to be rectangles that are aligned with the y-axis.
	float rotation; // In degrees.

	// This is the only component that should have any logic in it.
	// I'm making an exception here for simplicity's sake.
	// These just help calculate rotation for rendering.
	glm::vec2 Rotate(glm::vec2 point);
	glm::vec2 RelativeLocation(glm::vec2 p, glm::vec2 up, glm::vec2 right);

	// And the constructor.
	PositionComponent(Entity* entity, bool active, bool stat, float x, float y, float z, float rotation);
};

class PhysicsComponent : public Component
{
public:
	// This is just the velocity vector for a given object.
	// This is in units per second.
	float velocityX;
	float velocityY;

	// This is rotational velocity in degrees per second.
	float rotVelocity;

	// We have a drag and baseDrag because sometimes we'll change the former
	// but we always want to have something to tell us what the default
	// drag for the object should be.
	// Drag usually only applies when one is "onPlatform," according to the collider
	// system. For example, drag doesn't apply in mid-air.
	float drag;					// How much velocity one loses each turn.
	float baseDrag;

	// Gravity should be around 2000.0f by default.
	// This gives the game a nice weighty feel, and since we decrease
	// gravity when you are jumping (if you're still holding the jump button)
	// it makes the character feel particularly agile.
	float gravityMod;			// How much gravity should one experience.
	float baseGravityMod;

	// In certain components, we keep a reference to the position component since one needs to exist
	// for the system to work properly. I really should either remove this or standardize it.
	PositionComponent* pos;

	PhysicsComponent(Entity* entity, bool active, PositionComponent* pos, float vX, float vY, float vR, float drag, float gravityMod);
};

class StaticSpriteComponent : public Component
{
public:
	// This should be the width and height of the texture, not the desired width and height of the sprite when rendered.
	// For that, one should make use of the scale variables below.
	// The width and height are needed for calculating map sampling, so changing them will mess up the appearance of sprites.
	float width;
	float height;

	// If one wants to change the size of an object, one should use these instead of changing the dimensions above.
	float scaleX;
	float scaleY;

	// This just tells the renderer whether it should flip the X or Y value of the texture coordinates.
	bool flippedX;
	bool flippedY;

	// This tells the renderer whether or not to repeat the texture over the sprite's whole size or to stretch it across instead.
	bool tiled;

	// The "sprite" should be a texture whose each pixel contains an r and g value that are coordinates to the pixel on the map which
	// contains the color that the renderer should apply. This is a little weird, but it allows us to switch out this map on the fly
	// to make subtle (or sometimes unsubtle) changes to the appearance of the sprite without having to create multiple sprites for 
	// every possible variation we desire.
	Texture2D* sprite;
	Texture2D* mapTex;

	// Again, a quick and dirty reference to the position component allows us to reference it directly instead of going through the entity's component map.
	PositionComponent* pos;

	StaticSpriteComponent(Entity* entity, bool active, PositionComponent* pos, float width, float height, float scaleX, float scaleY, Texture2D* sprite, Texture2D* mapTex, bool flippedX, bool flippedY, bool tiled);
};

class ColliderComponent : public Component
{
public:
	bool platform;		// Only collides on the top.
	bool onPlatform;	// Every frame that you collide with a platform,
						// this is set to true so that I don't have to
						// raycast to check if you're on a platform.
	bool onewayPlatform;
	bool ignoreOnewayPlatforms;
	
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
	float baseHeight;

	float offsetX;
	float offsetY;
	float baseOffsetY;

	PositionComponent* pos;

	ColliderComponent(Entity* entity, bool active, PositionComponent* pos, bool platform, bool onewayPlatform, bool ignoreOnewayPlatforms, bool climbable, bool trigger, bool takesDamage, bool doesDamage, EntityClass entityClass, float mass, float bounce, float friction, float width, float height, float offsetX, float offsetY);
};

class InputComponent : public Component
{
public:
	Entity* moonlightBlade;
	bool acceptInput;

	bool releasedJump;
	float coyoteTime;
	float maxCoyoteTime;
	int jumps;
	int maxJumps;

	float lastTarget;
	float targetDelay;

	Texture2D* baseMap;

	InputComponent(Entity* entity, bool active, Entity* moonlightBlade, bool acceptInput, float maxCoyoteTime, int maxJumps, float targetDelay, Texture2D* baseMap);
};

class MovementComponent : public Component
{
public:
	bool canMove;

	float acceleration;
	float maxSpeed;
	float baseMaxSpeed;
	float maxJumpHeight;

	bool jumping;
	float airControl;

	bool crouching;
	float crouchMod;

	bool wallRunning;
	float maxWallRun;

	bool canClimb;
	bool shouldClimb;
	bool climbing;
	float climbMod;

	float maxClimbHeight;
	float minClimbHeight;

	MovementComponent(Entity* entity, bool active, bool canMove, float acceleration, float maxSpeed, float maxJumpHeight, float airControl, float crouchMod, bool canClimb, bool shouldClimb, float climbMod);
};

class CameraFollowComponent : public Component
{
public:
	float speed;

	bool lockX;
	bool lockY;

	CameraFollowComponent(Entity* entity, bool active, float speed, bool lockX, bool lockY);
};

class AnimationComponent : public Component
{
public:
	int activeX;
	int activeY;

	std::string activeAnimation;
	map<std::string, Animation2D*> animations;
	Texture2D* mapTex;

	PositionComponent* pos;

	float lastTick;

	float scaleX;
	float scaleY;

	bool flippedX;
	bool flippedY;

	void SetAnimation(std::string s);

	void AddAnimation(std::string s, Animation2D* anim);

	AnimationComponent(Entity* entity, bool active, PositionComponent* pos, Animation2D* idleAnimation, std::string animationName, Texture2D* mapTex, float scaleX, float scaleY, bool flippedX, bool flippedY);
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

class DamageComponent : public Component
{
public:
	Entity* creator;

	bool hasLifetime;
	float lifetime;
	bool showAfterUses;
	bool limitedUses;
	int uses;
	float damage;
	bool lodges;
	bool lodged;

	// There's definitely a more elegant way to handle this,
	// but I wonder if it would be worth it to implement it.
	bool damagesPlayers;
	bool damagesEnemies;
	bool damagesObjects;

	DamageComponent(Entity* entity, bool active, Entity* creator, bool hasLifetime, float lifetime, bool showAfterUses, bool limitedUses, int uses, float damage, bool damagesPlayers, bool damagesEnemies, bool damagesObjects, bool lodges);
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

enum class AIType { aerial, ground };
class AIComponent : public Component
{
public:
	bool proc;
	float procRange;
	float chaseRange;

	float movementSpeed;
	float projectileSpeed;

	float lastAttack;
	float attackRate;

	AIType aiType;

	AIComponent(Entity* entity, bool active, bool proc, float procRange, float chaseRange, float movementSpeed, float projectileSpeed, float attackRate, AIType aiType);
};

class BladeComponent : public Component
{
public:
	bool attacking;
	bool thrown;
	bool lodged;

	float rushRange;
	float slowRange;

	float followSpeed;
	float projectileSpeed;

	ColliderComponent* platformCollider;

	Texture2D* corporealMap;
	Texture2D* incorporealMap;

	glm::vec2 manualTarget;
	float lastTargetSet;
	float minTargetSetDelay;

	BladeComponent(Entity* entity, bool active, float rushRange, float slowRange, float followSpeed, float projectileSpeed, ColliderComponent* platformCollider, Texture2D* corporealMap, Texture2D* incorporealMap, float minTargetSetDelay);
};

enum class Anchor { topLeft, bottomLeft, topRight, bottomRight };
class ImageComponent : public Component
{
public:
	Anchor anchor;
	float x;
	float y;

	ImageComponent(Entity* entity, bool active, Anchor anchor, float x, float y);
};

#endif
