#ifndef SYSTEM_H
#define SYSTEM_H

#include "game.h"
#include <vector>
#include <array>
#include "glm/gtx/norm.hpp"

using namespace std;

class Component;
class PositionComponent;
class PhysicsComponent;
class StaticSpriteComponent;
class ColliderComponent;
class InputComponent;
class MovementComponent;
class CameraFollowComponent;
class AnimationComponent;
class AnimationControllerComponent;
class PlayerAnimationControllerComponent;
class HealthComponent;
class DuelistComponent;
class DamageComponent;
class ParticleComponent;
class AIComponent;
class BladeComponent;
class Entity;

struct Collision
{
	glm::vec2 contactPoint;
	glm::vec2 contactNormal;
	float time;

	bool resolve;

	ColliderComponent* colB;

	Collision(glm::vec2 contactPoint, glm::vec2 contactNormal, float time, ColliderComponent* colB, bool resolve)
	{
		this->contactPoint = contactPoint;
		this->contactNormal = contactNormal;
		this->time = time;
		this->colB = colB;
		this->resolve = resolve;
	}
};

class System
{
public:
	virtual void Update(int activeScene, float deltaTime) = 0;
	virtual void AddComponent(Component* component) = 0;
	virtual void PurgeEntity(Entity* e) = 0;
};

class StaticRenderingSystem : public System
{
public:
	vector<StaticSpriteComponent*> sprites;

	void Update(int activeScene, float deltaTime);

	void AddComponent(Component* component);

	void PurgeEntity(Entity* e);
};

class PhysicsSystem : public System
{
public:
	vector<PhysicsComponent*> phys;

	void Update(int activeScene, float deltaTime);

	void AddComponent(Component* component);

	void PurgeEntity(Entity* e);
};

class PositionSystem : public System
{
public:
	vector<PositionComponent*> pos;

	void Update(int activeScene, float deltaTime);

	void AddComponent(Component* component);

	void PurgeEntity(Entity* e);
};

class ColliderSystem : public System
{
	vector<ColliderComponent*> colls;

	void Update(int activeScene, float deltaTime);

	bool RaycastDown(float size, float distance, ColliderComponent* colA, PositionComponent* posA, ColliderComponent* colB, PositionComponent* posB);

	bool TestCollision(ColliderComponent* colA, PositionComponent* posA, PhysicsComponent* physA, ColliderComponent* colB, PositionComponent* posB, PhysicsComponent* physB, float deltaTime);

	bool TestAndResolveCollision(ColliderComponent* colA, PositionComponent* posA, PhysicsComponent* physA, ColliderComponent* colB, PositionComponent* posB, PhysicsComponent* physB, float deltaTime);

	Collision* ArbitraryRectangleCollision(ColliderComponent* colA, PositionComponent* posA, PhysicsComponent* physA, ColliderComponent* colB, PositionComponent* posB, PhysicsComponent* physB, float deltaTime);

	Collision* DynamicArbitraryRectangleCollision(ColliderComponent* colA, PositionComponent* posA, PhysicsComponent* physA, ColliderComponent* colB, PositionComponent* posB, PhysicsComponent* physB, float deltaTime);

	float Dot(glm::vec2 a, glm::vec2 b);

	glm::vec2 Project(glm::vec2 v, glm::vec2 a);

	glm::vec2 Bounce(glm::vec2 v, glm::vec2 n);

	void AddComponent(Component* component);

	void PurgeEntity(Entity* e);
};

class InputSystem : public System
{
public:
	vector<InputComponent*> move;

	void Update(int activeScene, float deltaTime);

	void CalculateProjection(PhysicsComponent* phys, InputComponent* m, MovementComponent* move);

	void AddComponent(Component* component);

	void PurgeEntity(Entity* e);
};

class CameraFollowSystem : public System
{
public:
	vector<CameraFollowComponent*> folls;

	void Update(int activeScene, float deltaTime);

	float Lerp(float a, float b, float t);

	void AddComponent(Component* component);

	void PurgeEntity(Entity* e);
};

class AnimationControllerSystem : public System
{
public:
	vector<AnimationControllerComponent*> controllers;

	void Update(int activeScene, float deltaTime);

	void AddComponent(Component* component);

	void PurgeEntity(Entity* e);
};

class AnimationSystem : public System
{
public:
	vector<AnimationComponent*> anims;

	void Update(int activeScene, float deltaTime);

	void AddComponent(Component* component);

	void PurgeEntity(Entity* e);
};

class HealthSystem : public System
{
public:
	vector<HealthComponent*> healths;

	void Update(int activeScene, float deltaTime);

	void AddComponent(Component* component);

	void PurgeEntity(Entity* e);
};

class ParticleSystem : public System
{
public:
	vector<ParticleComponent*> particles;

	void Update(int activeScene, float deltaTime);

	void AddComponent(Component* component);

	void PurgeEntity(Entity* e);
};

class DamageSystem : public System
{
public:
	vector<DamageComponent*> damagers;

	void Update(int activeScene, float deltaTime);

	void AddComponent(Component* component);

	void PurgeEntity(Entity* e);
};

class AISystem : public System
{
public:
	vector<AIComponent*> ai;

	void Update(int activeScene, float deltaTime);

	void AddComponent(Component* component);

	void PurgeEntity(Entity* e);
};

class BladeSystem : public System
{
public:
	vector<BladeComponent*> blades;

	void Update(int activeScene, float deltaTime);

	void AddComponent(Component* component);

	void PurgeEntity(Entity* e);
};

#endif
