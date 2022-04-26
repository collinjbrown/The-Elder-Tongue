// ecs.cpp

#include "particleengine.h"
#include "ecs.h"
#include "system.h"
#include "component.h"
#include "entity.h"

#pragma region Math

float Norm(glm::vec2 a)
{
	return sqrt(a.x * a.x + a.y * a.y);
}

glm::vec2 Normalize(glm::vec2 a)
{
	return a * (1 / Norm(a));
}

#pragma endregion

#pragma region Entities

int Entity::Get_ID() { return ID; }
std::string Entity::Get_Name() { return name; }

void Entity::Set_ID(int newID) { ID = newID; }
void Entity::Set_Name(std::string newName) { name = newName; }

Entity:: Entity(int ID, std::string name)
{
	this->ID = ID;
	this->name = name;
};

#pragma endregion

#pragma region Component Blocks
void ComponentBlock::Update(float deltaTime)
{
	system->Update(deltaTime);
}
void ComponentBlock::AddComponent(Component* c)
{
	system->AddComponent(c);
}
void ComponentBlock::PurgeEntity(Entity* e)
{
	system->PurgeEntity(e);
}
ComponentBlock::ComponentBlock(System* system, int componentID)
{
	this->system = system;
	this->componentID = componentID;
}
#pragma endregion

#pragma region ECS
uint32_t ECS::GetID()
{
	return ++entityIDCounter;
}

void ECS::Init()
{
	// I think we're going to have to initiate every component block
	// at the beginning of the game. This might be long.

	InputSystem* inputSystem = new InputSystem();
	ComponentBlock* inputBlock = new ComponentBlock(inputSystem, inputComponentID);
	componentBlocks.push_back(inputBlock);

	PhysicsSystem* physicsSystem = new PhysicsSystem();
	ComponentBlock* physicsBlock = new ComponentBlock(physicsSystem, physicsComponentID);
	componentBlocks.push_back(physicsBlock);

	ParticleSystem* particleSystem = new ParticleSystem();
	ComponentBlock* particleBlock = new ComponentBlock(particleSystem, particleComponentID);
	componentBlocks.push_back(particleBlock);

	ColliderSystem* colliderSystem = new ColliderSystem();
	ComponentBlock* colliderBlock = new ComponentBlock(colliderSystem, colliderComponentID);
	componentBlocks.push_back(colliderBlock);

	HealthSystem* healthSystem = new HealthSystem();
	ComponentBlock* healthBlock = new ComponentBlock(healthSystem, healthComponentID);
	componentBlocks.push_back(healthBlock);

	DuellingSystem* duelistSystem = new DuellingSystem();
	ComponentBlock* duelistBlock = new ComponentBlock(duelistSystem, duelistComponentID);
	componentBlocks.push_back(duelistBlock);

	StaticRenderingSystem* renderingSystem = new StaticRenderingSystem();
	ComponentBlock* renderingBlock = new ComponentBlock(renderingSystem, spriteComponentID);
	componentBlocks.push_back(renderingBlock);

	CameraFollowSystem* camfollowSystem = new CameraFollowSystem();
	ComponentBlock* camfollowBlock = new ComponentBlock(camfollowSystem, cameraFollowComponentID);
	componentBlocks.push_back(camfollowBlock);

	AnimationControllerSystem* animationControllerSystem = new AnimationControllerSystem();
	ComponentBlock* animationControllerBlock = new ComponentBlock(animationControllerSystem, animationControllerComponentID);
	componentBlocks.push_back(animationControllerBlock);

	AnimationSystem* animationSystem = new AnimationSystem();
	ComponentBlock* animationBlock = new ComponentBlock(animationSystem, animationComponentID);
	componentBlocks.push_back(animationBlock);
}

void ECS::Update(float deltaTime)
{
	round++;

	if (round == 1)
	{
		#pragma region Player Instantiation
		Entity* player = CreateEntity("The Player");
		Animation2D* anim1 = Game::main.animationMap["baseIdle"];

		ECS::main.RegisterComponent(new PositionComponent(player, true, false, 0, 100, 0.0f), player);
		ECS::main.RegisterComponent(new PhysicsComponent(player, true, (PositionComponent*)player->componentIDMap[positionComponentID], 0.0f, 0.0f, 0.0f, 1000.0f, 2000.0f), player);
		ECS::main.RegisterComponent(new ColliderComponent(player, true, (PositionComponent*)player->componentIDMap[positionComponentID], false, false, false, false, true, false, EntityClass::player, 1.0f, 0.2f, 1.0f, 25.0f, 55.0f, 0.0f, 0.0f), player);
		ECS::main.RegisterComponent(new MovementComponent(player, true, 2000.0f, 500.0f, 2.5f, 100.0f, 0.1f, true, true, false), player);
		ECS::main.RegisterComponent(new InputComponent(player, true, true, 0.5f, 5000, 0.5f, 2, 0.5f, 2000.0f), player);
		ECS::main.RegisterComponent(new CameraFollowComponent(player, true, 10.0f), player);
		ECS::main.RegisterComponent(new HealthComponent(player, true, 1000.0f, false), player);
		ECS::main.RegisterComponent(new DuelistComponent(player, true, true, true), player);
		ECS::main.RegisterComponent(new AnimationComponent(player, true, (PositionComponent*)player->componentIDMap[positionComponentID], anim1, "idle"), player);
		AnimationComponent* a = (AnimationComponent*)player->componentIDMap[animationComponentID];
		ECS::main.RegisterComponent(new PlayerAnimationControllerComponent(player, true, a), player);
		a->AddAnimation("walk", Game::main.animationMap["baseWalk"]);
		a->AddAnimation("jumpPrep", Game::main.animationMap["baseJumpPrep"]);
		a->AddAnimation("jumpUp", Game::main.animationMap["baseJumpUp"]);
		a->AddAnimation("jumpDown", Game::main.animationMap["baseJumpDown"]);
		a->AddAnimation("dead", Game::main.animationMap["baseDeath"]);

		a->AddAnimation("sword_idle", Game::main.animationMap["sword_baseIdle"]);
		a->AddAnimation("sword_walk", Game::main.animationMap["sword_baseWalk"]);
		a->AddAnimation("sword_jumpPrep", Game::main.animationMap["sword_baseJumpPrep"]);
		a->AddAnimation("sword_jumpUp", Game::main.animationMap["sword_baseJumpUp"]);
		a->AddAnimation("sword_jumpDown", Game::main.animationMap["sword_baseJumpDown"]);
		a->AddAnimation("sword_dead", Game::main.animationMap["baseDeath"]);
		a->AddAnimation("sword_stab", Game::main.animationMap["sword_baseStab"]);
		a->AddAnimation("sword_aerialOne", Game::main.animationMap["sword_baseAerialOne"]);
		#pragma endregion

		#pragma region Test Character Instantiation
		Animation2D* anim7 = Game::main.animationMap["testIdle"];
		Animation2D* anim8 = Game::main.animationMap["testWalk"];
		Animation2D* anim9 = Game::main.animationMap["testJumpPrep"];
		Animation2D* anim10 = Game::main.animationMap["testJumpUp"];
		Animation2D* anim11 = Game::main.animationMap["testJumpDown"];
		Animation2D* anim12 = Game::main.animationMap["testDeath"];

		Entity* character = CreateEntity("Test Character");
		ECS::main.RegisterComponent(new PositionComponent(character, true, false, 100, 100, 0.0f), character);
		ECS::main.RegisterComponent(new PhysicsComponent(character, true, (PositionComponent*)character->componentIDMap[positionComponentID], 0.0f, 0.0f, 0.0f, 200.0f, 1000.0f), character);
		ECS::main.RegisterComponent(new ColliderComponent(character, true, (PositionComponent*)character->componentIDMap[positionComponentID], false, false, false, false, true, false, EntityClass::enemy, 1.0f, 0.2f, 1.0f, 25.0f, 55.0f, 0.0f, 0.0f), character);
		ECS::main.RegisterComponent(new MovementComponent(character, true, 1000.0f, 500.0f, 2.5f, 1.0f, 0.25f, true, true, false), character);
		ECS::main.RegisterComponent(new AnimationComponent(character, true, (PositionComponent*)character->componentIDMap[positionComponentID], anim7, "idle"), character);
		ECS::main.RegisterComponent(new DuelistComponent(character, true, false, false), character);
		ECS::main.RegisterComponent(new HealthComponent(character, true, 100.0f, false), character);
		AnimationComponent* a2 = (AnimationComponent*)character->componentIDMap[animationComponentID];
		ECS::main.RegisterComponent(new PlayerAnimationControllerComponent(character, true, a2), character);
		a2->AddAnimation("walk", anim8);
		a2->AddAnimation("jumpPrep", anim9);
		a2->AddAnimation("jumpUp", anim10);
		a2->AddAnimation("jumpDown", anim11);
		a2->AddAnimation("dead", anim12);
		#pragma endregion

		Texture2D* tex3 = Game::main.textureMap["blank"];

		for (int i = 0; i < 25; i++)
		{
			Entity* platform = CreateEntity("floor");
			ECS::main.RegisterComponent(new PositionComponent(platform, true, true, rand() % 5000, rand() % 5000, 0), platform);
			ECS::main.RegisterComponent(new PhysicsComponent(platform, true, (PositionComponent*)platform->componentIDMap[positionComponentID], 0.0f, 0.0f, 0.0f, 0.1f, 0.0f), platform);
			ECS::main.RegisterComponent(new ColliderComponent(platform, true, (PositionComponent*)platform->componentIDMap[positionComponentID], true, false, true, false, false, false, EntityClass::object, 1000.0f, 0.0f, 1.0f, 540.0f, 80.0f, 0.0f, 0.0f), platform);
			ECS::main.RegisterComponent(new StaticSpriteComponent(platform, true, (PositionComponent*)platform->componentIDMap[positionComponentID], tex3->width * 35, tex3->height * 5.0f, tex3), platform);
		}

		for (int i = 0; i < 50; i++)
		{
			Entity* floor = CreateEntity("floor");
			ECS::main.RegisterComponent(new PositionComponent(floor, true, true, i * 500, -200, 0.0f), floor);
			ECS::main.RegisterComponent(new PhysicsComponent(floor, true, (PositionComponent*)floor->componentIDMap[positionComponentID], 0.0f, 0.0f, 0.0f, 0.1f, 0.0f), floor);
			ECS::main.RegisterComponent(new ColliderComponent(floor, true, (PositionComponent*)floor->componentIDMap[positionComponentID], true, false, true, false, false, false, EntityClass::object, 1000.0f, 0.0f, 1.0f, 540.0f, 80.0f, 0.0f, 0.0f), floor);
			ECS::main.RegisterComponent(new StaticSpriteComponent(floor, true, (PositionComponent*)floor->componentIDMap[positionComponentID], tex3->width * 35, tex3->height * 5.0f, tex3), floor);

			Entity* earth = CreateEntity("floor");
			ECS::main.RegisterComponent(new PositionComponent(earth, true, true, i * 500, -1000, 0.0f), earth);
			ECS::main.RegisterComponent(new StaticSpriteComponent(earth, true, (PositionComponent*)earth->componentIDMap[positionComponentID], tex3->width * 35, tex3->height * 100.0f, tex3), earth);
		}
	}

	for (int i = 0; i < componentBlocks.size(); i++)
	{
		componentBlocks[i]->Update(deltaTime);
	}

	PurgeDeadEntities();
}

void ECS::AddDeadEntity(Entity* e)
{
	if (std::find(dyingEntities.begin(), dyingEntities.end(), e) == dyingEntities.end())
	{
		dyingEntities.push_back(e);
	}
}

void ECS::PurgeDeadEntities()
{
	if (dyingEntities.size() > 0)
	{
		int n = dyingEntities.size();

		for (int i = 0; i < n; i++)
		{
			DeleteEntity(dyingEntities[i]);
		}

		dyingEntities.clear();
	}
}

Entity* ECS::CreateEntity(std::string name)
{
	Entity* e = new Entity(GetID(), name);
	return e;
}

void ECS::DeleteEntity(Entity* e)
{
	for (int i = 0; i < componentBlocks.size(); i++)
	{
		componentBlocks[i]->PurgeEntity(e);
	}

	delete e;
}

void ECS::RegisterComponent(Component* component, Entity* entity)
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
#pragma endregion

#pragma region Components

#pragma region Position Component
glm::vec2 PositionComponent::Rotate(glm::vec2 point)
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

glm::vec2 PositionComponent::RelativeLocation(glm::vec2 p, glm::vec2 up, glm::vec2 right)
{
	return glm::vec2((p.x * right.x) + (p.y * up.x), (p.x * right.y) + (p.y * up.y));
}

PositionComponent::PositionComponent(Entity* entity, bool active, bool stat, float x, float y, float rotation)
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
#pragma endregion

#pragma region Physics Component

PhysicsComponent::PhysicsComponent(Entity* entity, bool active, PositionComponent* pos, float vX, float vY, float vR, float drag, float gravityMod)
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

#pragma endregion

#pragma region Static Sprite Component

StaticSpriteComponent::StaticSpriteComponent(Entity* entity, bool active, PositionComponent* pos, float width, float height, Texture2D* sprite)
{
	ID = spriteComponentID;
	this->active = active;
	this->entity = entity;
	this->pos = pos;

	this->width = width;
	this->height = height;
	this->sprite = sprite;
}

#pragma endregion

#pragma region Collider Component

ColliderComponent::ColliderComponent(Entity* entity, bool active, PositionComponent* pos, bool platform, bool onewayPlatform, bool climbable, bool trigger, bool takesDamage, bool doesDamage, EntityClass entityClass, float mass, float bounce, float friction, float width, float height, float offsetX, float offsetY)
{
	ID = colliderComponentID;
	this->active = active;
	this->entity = entity;
	this->pos = pos;

	this->platform = platform;
	this->onewayPlatform = onewayPlatform;
	this->onPlatform = false;
	this->collidedLastTick = false;
	this->climbable = climbable;

	this->trigger = trigger;
	this->takesDamage = takesDamage;
	this->doesDamage = doesDamage;

	this->entityClass = entityClass;

	this->mass = mass;
	this->bounce = bounce;
	this->friction = friction;

	this->width = width;
	this->height = height;

	this->offsetX = offsetX;
	this->offsetY = offsetY;
}

#pragma endregion

#pragma region Input Component

InputComponent::InputComponent(Entity* entity, bool active, bool acceptInput, float projectionDelay, float projectionDepth, float maxCoyoteTime, int maxJumps, float projectileDelay, float projectileSpeed)
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
	this->jumps = 0;
	this->maxJumps = maxJumps;

	this->lastProjectile = 0.0f;
	this->projectileDelay = projectileDelay;
	this->projectileSpeed = projectileSpeed;
}

#pragma endregion

#pragma region Movement Component

MovementComponent::MovementComponent(Entity* entity, bool active, float acceleration, float maxSpeed, float maxJumpHeight, float stabDepth, float moveAttemptDelay, bool canMove, bool canClimb, bool shouldClimb)
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

#pragma endregion

#pragma region Camera Follow Component

CameraFollowComponent::CameraFollowComponent(Entity* entity, bool active, float speed)
{
	this->ID = cameraFollowComponentID;
	this->active = active;
	this->entity = entity;

	this->speed = speed;
}

#pragma endregion

#pragma region Animation Component

void AnimationComponent::SetAnimation(std::string s)
{
	if (animations[s] != NULL)
	{
		activeAnimation = s;
		activeX = 0;
		activeY = animations[s]->rows - 1;
		lastTick = 0;
	}
}

void AnimationComponent::AddAnimation(std::string s, Animation2D* anim)
{
	animations.emplace(s, anim);
}

AnimationComponent::AnimationComponent(Entity* entity, bool active, PositionComponent* pos, Animation2D* idleAnimation, std::string animationName)
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

#pragma endregion

#pragma region Player Animation Controller Component

PlayerAnimationControllerComponent::PlayerAnimationControllerComponent(Entity* entity, bool active, AnimationComponent* animator)
{
	this->ID = animationControllerComponentID;
	this->subID = dragonriderAnimControllerSubID;
	this->entity = entity;
	this->active = active;

	this->animator = animator;
}

#pragma endregion

#pragma region Health Component

HealthComponent::HealthComponent(Entity* entity, bool active, float health, bool dead)
{
	this->ID = healthComponentID;
	this->entity = entity;
	this->active = active;

	this->health = health;

	this->dead = dead;
}

#pragma endregion

#pragma region Duelist Component

DuelistComponent::DuelistComponent(Entity* entity, bool active, bool hasSword, bool isDrawn)
{
	this->ID = duelistComponentID;
	this->entity = entity;
	this->active = active;

	this->hasSword = hasSword;
	this->isDrawn = isDrawn;
	this->isAttacking = false;
	lastTick = 0;
}

#pragma endregion

#pragma region Damage Component

DamageComponent::DamageComponent(Entity* entity, bool active, bool hasLifetime, float lifetime, bool limitedUses, int uses, float damage, bool damagesPlayers, bool damagesEnemies, bool damagesObjects)
{
	this->ID = damageComponentID;
	this->entity = entity;
	this->active = active;

	this->hasLifetime = hasLifetime;
	this->lifetime = lifetime;
	this->limitedUses = limitedUses;
	this->uses = uses;
	this->damage = damage;

	this->damagesPlayers = damagesPlayers;
	this->damagesEnemies = damagesEnemies;
	this->damagesObjects = damagesObjects;
}

#pragma endregion

#pragma region Particle Component

ParticleComponent::ParticleComponent(Entity* entity, bool active, float tickRate, float xOffset, float yOffset, int number, Element element, float minLifetime, float maxLifetime)
{
	this->ID = particleComponentID;
	this->entity = entity;
	this->active = active;

	this->lastTick = 0.0f;
	this->tickRate = tickRate;

	this->xOffset = xOffset;
	this->yOffset = yOffset;

	this->number = number;

	this->element = element;
	
	this->minLifetime = minLifetime;
	this->maxLifetime = maxLifetime;
}

#pragma endregion

#pragma endregion

#pragma region Systems

#pragma region Static Rendering System

void StaticRenderingSystem::Update(float deltaTime)
{
	float screenLeft = (Game::main.camX - (Game::main.windowWidth * Game::main.zoom / 1.0f));
	float screenRight = (Game::main.camX + (Game::main.windowWidth * Game::main.zoom / 1.0f));
	float screenBottom = (Game::main.camY - (Game::main.windowHeight * Game::main.zoom / 1.0f));
	float screenTop = (Game::main.camY + (Game::main.windowHeight * Game::main.zoom / 1.0f));
	float screenElev = Game::main.camZ;

	for (int i = 0; i < sprites.size(); i++)
	{
		StaticSpriteComponent* s = sprites[i];

		if (s->active)
		{
			PositionComponent* pos = s->pos;

			if (pos->x + (s->width / 2.0f) > screenLeft && pos->x - (s->width / 2.0f) < screenRight &&
				pos->y + (s->height / 2.0f) > screenBottom && pos->y - (s->height / 2.0f) < screenTop &&
				pos->z < screenElev)
			{
				Game::main.renderer->prepareQuad(pos, s->width, s->height, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), s->sprite->ID);
			}
		}
	}
}

void StaticRenderingSystem::AddComponent(Component* component)
{
	sprites.push_back((StaticSpriteComponent*)component);
}

void StaticRenderingSystem::PurgeEntity(Entity* e)
{
	for (int i = 0; i < sprites.size(); i++)
	{
		if (sprites[i]->entity == e)
		{
			StaticSpriteComponent* s = sprites[i];
			sprites.erase(std::remove(sprites.begin(), sprites.end(), s), sprites.end());
			delete s;
		}
	}
}

#pragma endregion

#pragma region Physics System

void PhysicsSystem::Update(float deltaTime)
{
	for (int i = 0; i < phys.size(); i++)
	{
		PhysicsComponent* p = phys[i];

		if (p->active)
		{
			PositionComponent* pos = p->pos;
			ColliderComponent* col = (ColliderComponent*)p->entity->componentIDMap[colliderComponentID];

			if (!pos->stat)
			{
				if (col != nullptr)
				{
					if (col->entity->componentIDMap[movementComponentID] != nullptr)
					{
						MovementComponent* move = (MovementComponent*)col->entity->componentIDMap[movementComponentID];

						if (!col->onPlatform && !move->climbing)
						{
							p->velocityY -= p->gravityMod * deltaTime;
						}
						else if (move->climbing)
						{
							if (p->velocityY > 0)
							{
								p->velocityY -= p->drag * deltaTime;
							}
							else if (p->velocityY < 0)
							{
								p->velocityY += p->drag * deltaTime;
							}
						}
					}
					else if (!col->onPlatform)
					{
						p->velocityY -= p->gravityMod * deltaTime;
					}

					if (p->velocityX > 0 && col->onPlatform)
					{
						p->velocityX -= p->drag * deltaTime;
					}
					else if (p->velocityX < 0 && col->onPlatform)
					{
						p->velocityX += p->drag * deltaTime;
					}

					if (p->velocityY > 0 && col->onPlatform)
					{
						p->velocityY -= p->drag * deltaTime;
					}
					else if (p->velocityY < 0 && col->onPlatform)
					{
						p->velocityY += p->drag * deltaTime;
					}

					if (p->rotVelocity > 0 && col->onPlatform)
					{
						p->rotVelocity -= p->drag * deltaTime;
					}
					else if (p->rotVelocity < 0 && col->onPlatform)
					{
						p->rotVelocity += p->drag * deltaTime;
					}
				}
				else
				{
					p->velocityY -= p->gravityMod * deltaTime;
				}

				if (abs(p->velocityX) < 0.5f)
				{
					p->velocityX = 0;
				}

				if (abs(p->velocityY) < 0.5f)
				{
					p->velocityY = 0;
				}

				if (abs(p->rotVelocity) < 0.5f)
				{
					p->rotVelocity = 0;
				}

				pos->x += p->velocityX * deltaTime;
				pos->y += p->velocityY * deltaTime;
				pos->rotation += p->rotVelocity * deltaTime;
			}
			else
			{
				p->velocityX = 0;
				p->velocityY = 0;
				p->rotVelocity = 0;
			}
		}
	}
}

void PhysicsSystem::AddComponent(Component* component)
{
	phys.push_back((PhysicsComponent*)component);
}

void PhysicsSystem::PurgeEntity(Entity* e)
{
	for (int i = 0; i < phys.size(); i++)
	{
		if (phys[i]->entity == e)
		{
			PhysicsComponent* s = phys[i];
			phys.erase(std::remove(phys.begin(), phys.end(), s), phys.end());
			delete s;
		}
	}
}

#pragma endregion

#pragma region Collider System

void ColliderSystem::Update(float deltaTime)
{
	for (int i = 0; i < colls.size(); i++)
	{
		ColliderComponent* cA = colls[i];
		cA->onPlatform = false;
		cA->collidedLastTick = false;

		if (cA->active)
		{
			bool keepVelocityClimbing = true;

			if (cA->entity->componentIDMap[movementComponentID] != nullptr)
			{
				MovementComponent* moveA = (MovementComponent*)cA->entity->componentIDMap[movementComponentID];

				if (moveA->climbing)
				{
					keepVelocityClimbing = true;
				}
				else
				{
					keepVelocityClimbing = false;
				}
			}

			PositionComponent* posA = cA->pos;
			PhysicsComponent* physA = (PhysicsComponent*)cA->entity->componentIDMap[physicsComponentID];

			/*float tentativeADX = (physA->velocityX - physA->drag) * deltaTime;
			float tentativeADY = ((physA->velocityY - physA->drag) + (physA->gravityMod * deltaTime)) * deltaTime;*/

			// Right now, the game can support as many colliders as we realistically need.
			// But it can't support a ton. It'll run fairly well at a couple hundred colliders
			// so I'm not really worried about that.

			for (int j = 0; j < colls.size(); j++)
			{
				ColliderComponent* cB = colls[j];

				if (cB->entity->Get_ID() != cA->entity->Get_ID())
				{
					PositionComponent* posB = (PositionComponent*)cB->entity->componentIDMap[positionComponentID];
					PhysicsComponent* physB = (PhysicsComponent*)cB->entity->componentIDMap[physicsComponentID];

					// Two static objects shouldn't be able to collide because they won't be able to resolve that collision.
					if (!posA->stat || !posB->stat)
					{
						// Test to see if they're even remotely near one another (with respects to their collider size.)
						float d = sqrt(((posB->y - posA->y) * (posB->y - posA->y)) + ((posB->x - posA->x) * (posB->x - posA->x)));
						float dA = sqrt((cA->width * cA->width) + (cA->height * cA->height));
						float dB = sqrt((cB->width * cB->width) + (cB->height * cB->height));

						if (d < dA + dB)
						{
							// Test to see if they're platforms. We don't want two platforms colliding.
							if (!cA->platform || !cB->platform)
							{
								// Deal with triggers (which don't need to resolve their collisions).

								if (cA->trigger || cB->trigger)
								{
									if (TestCollision(cA, posA, physA, cB, posB, physB))
									{
										if (cA->trigger && cA->doesDamage)
										{
											DamageComponent* aDamage = (DamageComponent*)cA->entity->componentIDMap[damageComponentID];

											if (cB->takesDamage)
											{
												if (cB->entityClass == EntityClass::player && aDamage->damagesPlayers ||
													cB->entityClass == EntityClass::enemy && aDamage->damagesEnemies ||
													cB->entityClass == EntityClass::object && aDamage->damagesObjects)
												{
													HealthComponent* bHealth = (HealthComponent*)cB->entity->componentIDMap[healthComponentID];
													bHealth->health -= aDamage->damage;
													aDamage->uses -= 1;
												}
											}
											else
											{
												aDamage->uses -= 1;
											}

											if (aDamage->uses <= 0)
											{
												cA->active = false;
												ECS::main.AddDeadEntity(aDamage->entity);
											}
										}

										if (cB->trigger && cB->doesDamage)
										{
											DamageComponent* bDamage = (DamageComponent*)cB->entity->componentIDMap[damageComponentID];

											if (cA->takesDamage)
											{
												if (cA->entityClass == EntityClass::player && bDamage->damagesPlayers ||
													cA->entityClass == EntityClass::enemy && bDamage->damagesEnemies ||
													cA->entityClass == EntityClass::object && bDamage->damagesObjects)
												{
													HealthComponent* aHealth = (HealthComponent*)cA->entity->componentIDMap[healthComponentID];
													aHealth->health -= bDamage->damage;
													bDamage->uses -= 1;
												}
											}
											else
											{
												bDamage->uses -= 1;
											}

											if (bDamage->uses <= 0)
											{
												cB->active = false;
												ECS::main.AddDeadEntity(bDamage->entity);
											}
										}
									}
								}
								else
								{
									// Test to see if one is a platform and the other is coming from the right direction.

									float aBot = posA->y - (cA->height / 2.0f) + cA->offsetY;
									float aTop = posA->y + (cA->height / 2.0f) + cA->offsetY;

									float bBot = posB->y - (cB->height / 2.0f) + cB->offsetY;
									float bTop = posB->y + (cB->height / 2.0f) + cB->offsetY;


									// I'm an idiot.
									// What is going on is this:
									// Let's say A is above B. B is a platform. A is not.
									// A will only collide with B if the following rules apply:

									/*if (!cA->platform && !cB->platform ||
										cA->platform && bBot > aTop ||
										cB->platform && aBot > bTop)*/

										// This won't work properly because that's not how collisions work.
										// When A collides with B, even if most of it is above B, it won't
										// properly collide because the bottom of A is below the top of B.
										// This is a really simple mistake, and I feel kinda dumb;
										// I've caught myself thinking about collisions as if they actually
										// take place in continuous time.
										// We need to add a little leeway
										// (which will also probably require me to deal with tunneling).

									float platformLeeway = 2.0f;

									if (!cA->platform && !cB->platform ||
										cA->platform && !cA->onewayPlatform ||
										cB->platform && !cB->onewayPlatform ||
										cA->platform && cA->onewayPlatform && bBot > aTop - platformLeeway ||
										cB->platform && cB->onewayPlatform && aBot > bTop - platformLeeway)
									{
										/*float tentativeBDX = (physB->velocityX - physB->drag) * deltaTime;
										float tentativeBDY = ((physB->velocityY - physB->drag) + (physB->gravityMod * deltaTime)) * deltaTime;*/

										// This does not yet (really) solve for tunneling.
										if (TestAndResolveCollision(cA, posA, physA, cB, posB, physB, deltaTime))
										{
											cA->collidedLastTick = true;
											cB->collidedLastTick = true;

											if (cB->climbable && aTop > bBot && aBot < bTop && cA->entity->componentIDMap[movementComponentID] != nullptr)
											{
												MovementComponent* moveA = (MovementComponent*)cA->entity->componentIDMap[movementComponentID];

												if (moveA->canClimb && moveA->shouldClimb)
												{
													if (!keepVelocityClimbing)
													{
														// If you just started climbing, stop all other velocity.
														physA->velocityX = 0;
														physA->velocityY = 0;
													}

													moveA->maxClimbHeight = bTop;
													moveA->minClimbHeight = bBot;
													moveA->climbing = true;
												}
											}

											if (cA->platform && bBot > aTop - platformLeeway)
											{
												cB->onPlatform = true;
											}
											else if (cB->platform && aBot > bTop - platformLeeway)
											{
												cA->onPlatform = true;
											}
										}
										else if (cB->platform && aBot > bTop - platformLeeway)
										{
											if (RaycastDown(1, 0.1f, cA, posA, cB, posB))
											{
												cA->onPlatform = true;
											}
										}
									}
								}
							} // Bin gar keine Russin, stamm’ aus Litauen, echt deutsch.
						} // And when we were children, staying at the arch-duke's,
					} // My cousin's, he took me out on a sled,
				} // And I was frightened. He said, Marie,
			} // Marie, hold on tight. And down we went.
		} // In the mountains, there you feel free.
	} // I read, much of the night,
} // and go south in the winter.

bool ColliderSystem::RaycastDown(float size, float distance, ColliderComponent* colA, PositionComponent* posA, ColliderComponent* colB, PositionComponent* posB)
{
	float ryC = colA->height / 2.0f;

	float nR = -size;
	float r = size;

	float bCX = colB->offsetX;
	float bCY = colB->offsetY;

	float bLX = -(colB->width / 2.0f) + colB->offsetX;
	float bBY = -(colB->height / 2.0f) + colB->offsetY;

	float bRX = (colB->width / 2.0f) + colB->offsetX;
	float bTY = (colB->height / 2.0f) + colB->offsetY;

	glm::vec2 aCenter = glm::vec2(posA->x, posA->y);
	glm::vec2 aTopLeft = glm::vec2(posA->x, posA->y) + posA->Rotate(glm::vec2(nR, r - ryC));
	glm::vec2 aBottomLeft = glm::vec2(posA->x, posA->y - distance) + posA->Rotate(glm::vec2(nR, nR - size - ryC));
	glm::vec2 aTopRight = glm::vec2(posA->x, posA->y) + posA->Rotate(glm::vec2(r, r - ryC));
	glm::vec2 aBottomRight = glm::vec2(posA->x, posA->y - distance) + posA->Rotate(glm::vec2(r, nR - size - ryC));

	std::array<glm::vec2, 4> colliderOne = { aTopLeft, aTopRight, aBottomRight, aBottomLeft };

	glm::vec2 bCenter = glm::vec2(posB->x, posB->y) + posB->Rotate(glm::vec2(bCX, bCY));
	glm::vec2 bTopLeft = glm::vec2(posB->x, posB->y) + posB->Rotate(glm::vec2(bLX, bTY));
	glm::vec2 bBottomLeft = glm::vec2(posB->x, posB->y) + posB->Rotate(glm::vec2(bLX, bBY));
	glm::vec2 bTopRight = glm::vec2(posB->x, posB->y) + posB->Rotate(glm::vec2(bRX, bTY));
	glm::vec2 bBottomRight = glm::vec2(posB->x, posB->y) + posB->Rotate(glm::vec2(bRX, bBY));

	std::array<glm::vec2, 4> colliderTwo = { bTopLeft, bTopRight, bBottomRight, bBottomLeft };

	for (int s = 0; s < 2; s++)
	{
		if (s == 0)
		{
			// Diagonals
			for (int p = 0; p < colliderOne.size(); p++)
			{
				glm::vec2 lineA = aCenter;
				glm::vec2 lineB = colliderOne[p];

				// Edges
				for (int q = 0; q < colliderTwo.size(); q++)
				{
					glm::vec2 edgeA = colliderTwo[q];
					glm::vec2 edgeB = colliderTwo[(q + 1) % colliderTwo.size()];

					float h = (edgeB.x - edgeA.x) * (lineA.y - lineB.y) - (lineA.x - lineB.x) * (edgeB.y - edgeA.y);
					float t1 = ((edgeA.y - edgeB.y) * (lineA.x - edgeA.x) + (edgeB.x - edgeA.x) * (lineA.y - edgeA.y)) / h;
					float t2 = ((lineA.y - lineB.y) * (lineA.x - edgeA.x) + (lineB.x - lineA.x) * (lineA.y - edgeA.y)) / h;

					if (t1 >= 0.0f && t1 < 1.0f && t2 >= 0.0f && t2 < 1.0f)
					{
						// Game::main.renderer->prepareQuad(aTopRight, aBottomRight, aBottomLeft, aTopLeft, glm::vec4(1.0f, 0.0f, 0.0f, 0.5f), Game::main.textureMap["blank"]->ID);
						return true;
					}
				}
			}
		}
		else
		{
			// Diagonals
			for (int p = 0; p < colliderTwo.size(); p++)
			{
				glm::vec2 lineA = bCenter;
				glm::vec2 lineB = colliderTwo[p];

				// Edges
				for (int q = 0; q < colliderOne.size(); q++)
				{
					glm::vec2 edgeA = colliderOne[q];
					glm::vec2 edgeB = colliderOne[(q + 1) % colliderOne.size()];

					float h = (edgeB.x - edgeA.x) * (lineA.y - lineB.y) - (lineA.x - lineB.x) * (edgeB.y - edgeA.y);
					float t1 = ((edgeA.y - edgeB.y) * (lineA.x - edgeA.x) + (edgeB.x - edgeA.x) * (lineA.y - edgeA.y)) / h;
					float t2 = ((lineA.y - lineB.y) * (lineA.x - edgeA.x) + (lineB.x - lineA.x) * (lineA.y - edgeA.y)) / h;

					if (t1 >= 0.0f && t1 < 1.0f && t2 >= 0.0f && t2 < 1.0f)
					{
						// Game::main.renderer->prepareQuad(aTopRight, aBottomRight, aBottomLeft, aTopLeft, glm::vec4(1.0f, 0.0f, 0.0f, 0.5f), Game::main.textureMap["blank"]->ID);
						return true;
					}
				}
			}
		}
	}

	// Game::main.renderer->prepareQuad(aTopRight, aBottomRight, aBottomLeft, aTopLeft, glm::vec4(0.0f, 0.0f, 0.0f, 0.5f), Game::main.textureMap["blank"]->ID);
	return false;
}

bool ColliderSystem::TestCollision(ColliderComponent* colA, PositionComponent* posA, PhysicsComponent* physA, ColliderComponent* colB, PositionComponent* posB, PhysicsComponent* physB)
{
	float aCX = colA->offsetX;
	float aCY = colA->offsetY;

	float aLX = -(colA->width / 2.0f) + colA->offsetX;
	float aBY = -(colA->height / 2.0f) + colA->offsetY;

	float aRX = (colA->width / 2.0f) + colA->offsetX;
	float aTY = (colA->height / 2.0f) + colA->offsetY;


	float bCX = colB->offsetX;
	float bCY = colB->offsetY;

	float bLX = -(colB->width / 2.0f) + colB->offsetX;
	float bBY = -(colB->height / 2.0f) + colB->offsetY;

	float bRX = (colB->width / 2.0f) + colB->offsetX;
	float bTY = (colB->height / 2.0f) + colB->offsetY;

	glm::vec2 aCenter = glm::vec2(posA->x, posA->y) + posA->Rotate(glm::vec2(aCX, aCY));
	glm::vec2 aTopLeft = glm::vec2(posA->x, posA->y) + posA->Rotate(glm::vec2(aLX, aTY));
	glm::vec2 aBottomLeft = glm::vec2(posA->x, posA->y) + posA->Rotate(glm::vec2(aLX, aBY));
	glm::vec2 aTopRight = glm::vec2(posA->x, posA->y) + posA->Rotate(glm::vec2(aRX, aTY));
	glm::vec2 aBottomRight = glm::vec2(posA->x, posA->y) + posA->Rotate(glm::vec2(aRX, aBY));

	std::array<glm::vec2, 4> colliderOne = { aTopLeft, aTopRight, aBottomRight, aBottomLeft };

	glm::vec2 bCenter = glm::vec2(posB->x, posB->y) + posB->Rotate(glm::vec2(bCX, bCY));
	glm::vec2 bTopLeft = glm::vec2(posB->x, posB->y) + posB->Rotate(glm::vec2(bLX, bTY));
	glm::vec2 bBottomLeft = glm::vec2(posB->x, posB->y) + posB->Rotate(glm::vec2(bLX, bBY));
	glm::vec2 bTopRight = glm::vec2(posB->x, posB->y) + posB->Rotate(glm::vec2(bRX, bTY));
	glm::vec2 bBottomRight = glm::vec2(posB->x, posB->y) + posB->Rotate(glm::vec2(bRX, bBY));

	std::array<glm::vec2, 4> colliderTwo = { bTopLeft, bTopRight, bBottomRight, bBottomLeft };

	float totalMass = colA->mass + colB->mass;
	bool collided = false;

	// Game::main.renderer->prepareQuad(aTopRight, aBottomRight, aBottomLeft, aTopLeft, glm::vec4(1.0f, 0.0f, 0.0f, 0.5f), Game::main.textureMap["blank"]->ID);

	for (int s = 0; s < 2; s++)
	{
		if (s == 0)
		{
			// Diagonals
			for (int p = 0; p < colliderOne.size(); p++)
			{
				glm::vec2 lineA = aCenter;
				glm::vec2 lineB = colliderOne[p];

				// Edges
				for (int q = 0; q < colliderTwo.size(); q++)
				{
					glm::vec2 edgeA = colliderTwo[q];
					glm::vec2 edgeB = colliderTwo[(q + 1) % colliderTwo.size()];

					float h = (edgeB.x - edgeA.x) * (lineA.y - lineB.y) - (lineA.x - lineB.x) * (edgeB.y - edgeA.y);
					float t1 = ((edgeA.y - edgeB.y) * (lineA.x - edgeA.x) + (edgeB.x - edgeA.x) * (lineA.y - edgeA.y)) / h;
					float t2 = ((lineA.y - lineB.y) * (lineA.x - edgeA.x) + (lineB.x - lineA.x) * (lineA.y - edgeA.y)) / h;

					if (t1 >= 0.0f && t1 < 1.0f && t2 >= 0.0f && t2 < 1.0f)
					{
						collided = true;
					}
				}
			}
		}
		else
		{
			// Diagonals
			for (int p = 0; p < colliderTwo.size(); p++)
			{
				glm::vec2 lineA = bCenter;
				glm::vec2 lineB = colliderTwo[p];

				// Edges
				for (int q = 0; q < colliderOne.size(); q++)
				{
					glm::vec2 edgeA = colliderOne[q];
					glm::vec2 edgeB = colliderOne[(q + 1) % colliderOne.size()];

					float h = (edgeB.x - edgeA.x) * (lineA.y - lineB.y) - (lineA.x - lineB.x) * (edgeB.y - edgeA.y);
					float t1 = ((edgeA.y - edgeB.y) * (lineA.x - edgeA.x) + (edgeB.x - edgeA.x) * (lineA.y - edgeA.y)) / h;
					float t2 = ((lineA.y - lineB.y) * (lineA.x - edgeA.x) + (lineB.x - lineA.x) * (lineA.y - edgeA.y)) / h;

					if (t1 >= 0.0f && t1 < 1.0f && t2 >= 0.0f && t2 < 1.0f)
					{
						collided = true;
					}
				}
			}
		}
	}

	return collided;
}

bool ColliderSystem::TestAndResolveCollision(ColliderComponent* colA, PositionComponent* posA, PhysicsComponent* physA, ColliderComponent* colB, PositionComponent* posB, PhysicsComponent* physB, float deltaTime)
{
	float aCX = colA->offsetX;
	float aCY = colA->offsetY;

	float aLX = -(colA->width / 2.0f) + colA->offsetX;
	float aBY = -(colA->height / 2.0f) + colA->offsetY;

	float aRX = (colA->width / 2.0f) + colA->offsetX;
	float aTY = (colA->height / 2.0f) + colA->offsetY;

	float bCX = colB->offsetX;
	float bCY = colB->offsetY;

	float bLX = -(colB->width / 2.0f) + colB->offsetX;
	float bBY = -(colB->height / 2.0f) + colB->offsetY;

	float bRX = (colB->width / 2.0f) + colB->offsetX;
	float bTY = (colB->height / 2.0f) + colB->offsetY;
	bool collided = false;

	float dT = (int)(deltaTime * 100 + 0.5);
	dT = max(0.1f, 5.0f * ((float)dT / 100));
	// std::cout << std::to_string(deltaTime) + "/" + std::to_string(dT) + "\n";

	if (dT != 0)
	{
		for (float it = 0.9f; it > -0.1f; it -= dT)
		{
			glm::vec2 aCenter = (glm::vec2(posA->x, posA->y) + glm::vec2(physA->velocityX * it * deltaTime, physA->velocityY * it * deltaTime)) + posA->Rotate(glm::vec2(aCX, aCY));
			glm::vec2 aTopLeft = (glm::vec2(posA->x, posA->y) + glm::vec2(physA->velocityX * it * deltaTime, physA->velocityY * it * deltaTime)) + posA->Rotate(glm::vec2(aLX, aTY));
			glm::vec2 aBottomLeft = (glm::vec2(posA->x, posA->y) + glm::vec2(physA->velocityX * it * deltaTime, physA->velocityY * it * deltaTime)) + posA->Rotate(glm::vec2(aLX, aBY));
			glm::vec2 aTopRight = (glm::vec2(posA->x, posA->y) + glm::vec2(physA->velocityX * it * deltaTime, physA->velocityY * it * deltaTime)) + posA->Rotate(glm::vec2(aRX, aTY));
			glm::vec2 aBottomRight = (glm::vec2(posA->x, posA->y) + glm::vec2(physA->velocityX * it * deltaTime, physA->velocityY * it * deltaTime)) + posA->Rotate(glm::vec2(aRX, aBY));

			glm::vec2 bCenter = (glm::vec2(posB->x, posB->y) + glm::vec2(physB->velocityX * it * deltaTime, physB->velocityY * it * deltaTime)) + posB->Rotate(glm::vec2(bCX, bCY));
			glm::vec2 bTopLeft = (glm::vec2(posB->x, posB->y) + glm::vec2(physB->velocityX * it * deltaTime, physB->velocityY * it * deltaTime)) + posB->Rotate(glm::vec2(bLX, bTY));
			glm::vec2 bBottomLeft = (glm::vec2(posB->x, posB->y) + glm::vec2(physB->velocityX * it * deltaTime, physB->velocityY * it * deltaTime)) + posB->Rotate(glm::vec2(bLX, bBY));
			glm::vec2 bTopRight = (glm::vec2(posB->x, posB->y) + glm::vec2(physB->velocityX * it * deltaTime, physB->velocityY * it * deltaTime)) + posB->Rotate(glm::vec2(bRX, bTY));
			glm::vec2 bBottomRight = (glm::vec2(posB->x, posB->y) + glm::vec2(physB->velocityX * it * deltaTime, physB->velocityY * it * deltaTime)) + posB->Rotate(glm::vec2(bRX, bBY));

			std::array<glm::vec2, 4> colliderOne = { aTopLeft, aTopRight, aBottomRight, aBottomLeft };

			std::array<glm::vec2, 4> colliderTwo = { bTopLeft, bTopRight, bBottomRight, bBottomLeft };

			float totalMass = colA->mass + colB->mass;

			// Game::main.renderer->prepareQuad(aTopRight, aBottomRight, aBottomLeft, aTopLeft, glm::vec4(1.0f, 0.0f, 0.0f, 0.5f), Game::main.textureMap["blank"]->ID);

			for (int s = 0; s < 2; s++)
			{
				if (s == 0)
				{
					// Diagonals
					for (int p = 0; p < colliderOne.size(); p++)
					{
						glm::vec2 displacement = { 0, 0 };
						glm::vec2 collEdge = { 0, 0 };

						glm::vec2 lineA = aCenter;
						glm::vec2 lineB = colliderOne[p];

						// Edges
						for (int q = 0; q < colliderTwo.size(); q++)
						{
							glm::vec2 edgeA = colliderTwo[q];
							glm::vec2 edgeB = colliderTwo[(q + 1) % colliderTwo.size()];

							float h = (edgeB.x - edgeA.x) * (lineA.y - lineB.y) - (lineA.x - lineB.x) * (edgeB.y - edgeA.y);
							float t1 = ((edgeA.y - edgeB.y) * (lineA.x - edgeA.x) + (edgeB.x - edgeA.x) * (lineA.y - edgeA.y)) / h;
							float t2 = ((lineA.y - lineB.y) * (lineA.x - edgeA.x) + (lineB.x - lineA.x) * (lineA.y - edgeA.y)) / h;

							if (t1 >= 0.0f && t1 < 1.0f && t2 >= 0.0f && t2 < 1.0f)
							{
								if (collEdge.x == 0 && collEdge.y == 0)
								{
									collEdge = edgeB - edgeA;
								}

								collided = true;
								displacement.x += (1.0f - t1) * (lineB.x - lineA.x);
								displacement.y += (1.0f - t1) * (lineB.y - lineA.y);
							}
						}

						if (displacement.x != 0 || displacement.y != 0)
						{
							glm::vec2 right = Normalize(collEdge);
							glm::vec2 up = { right.y, right.x };

							//glm::vec2 aDispNormal = up;
							//glm::vec2 bDispNormal = -up;

							glm::vec2 aDispNormal = Normalize(glm::vec2(displacement.x, displacement.y));
							glm::vec2 bDispNormal = -aDispNormal;

							// We want to apply "bounce" to perpendicular velocity
							// and apply "friction" to parallel velocity.

							glm::vec2 aVel = glm::vec2(physA->velocityX, physA->velocityY);
							glm::vec2 bVel = glm::vec2(physB->velocityX, physB->velocityY);

							if (colA->bounce != 0)
							{
								glm::vec2 aNewVelocity = (aVel + (Project(aVel, aDispNormal) * -2.0f) * colA->bounce) * (1 - (colA->mass / totalMass)) * deltaTime;
								physA->velocityX = aNewVelocity.x;
								physA->velocityY = aNewVelocity.y;
							}

							if (colB->bounce != 0)
							{
								glm::vec2 bNewVelocity = (bVel + (Project(bVel, bDispNormal) * -2.0f) * colB->bounce) * (1 - (colB->mass / totalMass)) * deltaTime;
								physB->velocityX = bNewVelocity.x;
								physB->velocityY = bNewVelocity.y;
							}

							/*if (colA->platform)
							{
								glm::vec2 midTopA = (aTopLeft + aTopRight) / 2.0f;
								glm::vec2 midBotB = (bBottomLeft + bBottomRight) / 2.0f;
								glm::vec2 upA = Normalize(midTopA - aCenter);

								if (midBotB.y * upA.y < midTopA.y * upA.y && physB->velocityX >= 0 && aCenter.x > bCenter.x
									|| midBotB.y * upA.y < midTopA.y * upA.y && physB->velocityX <= 0 && aCenter.x < bCenter.x)
								{
									posB->x += displacement.x * upA.x * -1;
									posB->y += displacement.y * upA.y * -1;
								}
								else
								{
									posB->x += displacement.x * 1;
									posB->y += displacement.y * 1;
								}
							}*/

							if (colB->platform)
							{
								// Baba Booey
							}
							else if (!posA->stat && !posB->stat)
							{
								posA->x += displacement.x * -1 * (1 - (colA->mass / totalMass));
								posA->y += displacement.y * -1 * (1 - (colA->mass / totalMass));

								posB->x += displacement.x * 1 * (1 - (colB->mass / totalMass));
								posB->y += displacement.y * 1 * (1 - (colB->mass / totalMass));
							}
							else if (posA->stat && !posB->stat)
							{
								posB->x += displacement.x * 1;
								posB->y += displacement.y * 1;
							}
							else if (!posA->stat && posB->stat)
							{
								posA->x += displacement.x * -1;
								posA->y += displacement.y * -1;
							}
						}
					}
				}
				else
				{
					// Diagonals
					for (int p = 0; p < colliderTwo.size(); p++)
					{
						glm::vec2 displacement = { 0, 0 };
						glm::vec2 collEdge = { 0, 0 };

						glm::vec2 lineA = bCenter;
						glm::vec2 lineB = colliderTwo[p];

						// Edges
						for (int q = 0; q < colliderOne.size(); q++)
						{
							glm::vec2 edgeA = colliderOne[q];
							glm::vec2 edgeB = colliderOne[(q + 1) % colliderOne.size()];

							float h = (edgeB.x - edgeA.x) * (lineA.y - lineB.y) - (lineA.x - lineB.x) * (edgeB.y - edgeA.y);
							float t1 = ((edgeA.y - edgeB.y) * (lineA.x - edgeA.x) + (edgeB.x - edgeA.x) * (lineA.y - edgeA.y)) / h;
							float t2 = ((lineA.y - lineB.y) * (lineA.x - edgeA.x) + (lineB.x - lineA.x) * (lineA.y - edgeA.y)) / h;

							if (t1 >= 0.0f && t1 < 1.0f && t2 >= 0.0f && t2 < 1.0f)
							{
								if (collEdge.x == 0 && collEdge.y == 0)
								{
									collEdge = edgeB - edgeA;
								}

								collided = true;
								displacement.x += (1.0f - t1) * (lineB.x - lineA.x);
								displacement.y += (1.0f - t1) * (lineB.y - lineA.y);
							}
						}

						if (displacement.x != 0 || displacement.y != 0)
						{
							float oldVelocity = physA->velocityY;

							glm::vec2 right = Normalize(collEdge);
							glm::vec2 up = { right.y, right.x };

							//glm::vec2 aDispNormal = -up;
							//glm::vec2 bDispNormal = up;

							glm::vec2 aDispNormal = Normalize(glm::vec2(displacement.x, displacement.y));
							glm::vec2 bDispNormal = -aDispNormal;

							// We want to apply "bounce" to perpendicular velocity
							// and apply "friction" to parallel velocity.

							glm::vec2 aVel = glm::vec2(physA->velocityX, physA->velocityY);
							glm::vec2 bVel = glm::vec2(physB->velocityX, physB->velocityY);

							if (colA->bounce != 0)
							{
								glm::vec2 aNewVelocity = (aVel + (Project(aVel, aDispNormal) * -2.0f) * colA->bounce) * (1 - (colA->mass / totalMass)) * deltaTime;
								// physA->velocityX = aNewVelocity.x;
								physA->velocityY = aNewVelocity.y;
							}

							if (colB->bounce != 0)
							{
								glm::vec2 bNewVelocity = (bVel + (Project(bVel, bDispNormal) * -2.0f) * colB->bounce) * (1 - (colB->mass / totalMass)) * deltaTime;
								// physB->velocityX = bNewVelocity.x;
								physB->velocityY = bNewVelocity.y;
							}

							if (colB->platform)
							{
								glm::vec2 midTopB = (bTopLeft + bTopRight) / 2.0f;
								glm::vec2 midBotA = (aBottomLeft + aBottomRight) / 2.0f;
								glm::vec2 upB = Normalize(midTopB - bCenter);

								std::cout << std::to_string(abs(physA->velocityX)) + "/" + std::to_string(abs(physA->velocityY) - 50.0f);

								if (midBotA.y - oldVelocity > midTopB.y && glm::length2(aCenter - bTopLeft) > 0.5f && abs(physA->velocityX) < abs(physA->velocityY) - 50.0f ||
									midBotA.y - oldVelocity > midTopB.y && glm::length2(aCenter - bTopRight) > 0.5f && abs(physA->velocityX) < abs(physA->velocityY) - 50.0f)
								{
									std::cout << "Fits.\n";
									posA->x += displacement.x * upB.x * 1;
									posA->y += displacement.y * upB.y * 1;
								}
								else
								{
									std::cout << "Doesn't fit.\n";
									posA->x += displacement.x * 1;
									posA->y += displacement.y * 1;
								}
							}
							else if (!posA->stat && !posB->stat)
							{
								posA->x += displacement.x * 1 * (1 - (colA->mass / totalMass));
								posA->y += displacement.y * 1 * (1 - (colA->mass / totalMass));

								posB->x += displacement.x * -1 * (1 - (colB->mass / totalMass));
								posB->y += displacement.y * -1 * (1 - (colB->mass / totalMass));
							}
							else if (posA->stat && !posB->stat)
							{
								posB->x += displacement.x * -1;
								posB->y += displacement.y * -1;
							}
							else if (!posA->stat && posB->stat)
							{
								posA->x += displacement.x * 1;
								posA->y += displacement.y * 1;
							}
						}
					}
				}
			}
		}
	}

	return collided;
}

float ColliderSystem::Dot(glm::vec2 a, glm::vec2 b)
{
	return a.x * b.x + a.y * b.y;
}

glm::vec2 ColliderSystem::Project(glm::vec2 v, glm::vec2 a)
{
	return Normalize(a) * (Dot(v, a) / Norm(a));
}

glm::vec2 ColliderSystem::Bounce(glm::vec2 v, glm::vec2 n)
{
	return v + (Project(v, n) * -2.0f);
}

void ColliderSystem::AddComponent(Component* component)
{
	colls.push_back((ColliderComponent*)component);
}

void ColliderSystem::PurgeEntity(Entity* e)
{
	for (int i = 0; i < colls.size(); i++)
	{
		if (colls[i]->entity == e)
		{
			ColliderComponent* s = colls[i];
			colls.erase(std::remove(colls.begin(), colls.end(), s), colls.end());
			delete s;
		}
	}
}

#pragma endregion

#pragma region Input System

void InputSystem::Update(float deltaTime)
{
	for (int i = 0; i < move.size(); i++)
	{
		InputComponent* m = move[i];

		if (m->active)
		{
			MovementComponent* move = (MovementComponent*)m->entity->componentIDMap[movementComponentID];
			PhysicsComponent* phys = (PhysicsComponent*)m->entity->componentIDMap[physicsComponentID];
			ColliderComponent* col = (ColliderComponent*)m->entity->componentIDMap[colliderComponentID];
			HealthComponent* health = (HealthComponent*)m->entity->componentIDMap[healthComponentID];
			DuelistComponent* duel = (DuelistComponent*)m->entity->componentIDMap[duelistComponentID];

			m->lastTick += deltaTime;

			if (!health->dead)
			{
				if (move->jumping && col->onPlatform && phys->velocityY > 0)
				{
					col->onPlatform = false;
				}

				float aBot = phys->pos->y - (col->height / 2.0f) + col->offsetY;
				float aTop = phys->pos->y + (col->height / 2.0f) + col->offsetY;
				if (move->climbing && !move->shouldClimb ||
					move->climbing && aBot > move->maxClimbHeight ||
					move->climbing && aTop < move->minClimbHeight)
				{
					move->climbing = false;
				}

				if (move->jumping && col->onPlatform ||
					move->jumping && move->climbing)
				{
					move->jumping = false;
					m->jumps = 0;
					m->coyoteTime = 0.0f;
				}

				if (glfwGetMouseButton(Game::main.window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS && m->lastProjectile >= m->projectileDelay)
				{

					float screenLeft = (Game::main.camX - (Game::main.windowWidth * Game::main.zoom / 1.0f));
					float screenRight = (Game::main.camX + (Game::main.windowWidth * Game::main.zoom / 1.0f));
					float screenBottom = (Game::main.camY - (Game::main.windowHeight * Game::main.zoom / 1.0f));
					float screenTop = (Game::main.camY + (Game::main.windowHeight * Game::main.zoom / 1.0f));
					float screenElev = Game::main.camZ;
					
					glm::vec2 projPos = glm::vec2(phys->pos->x, phys->pos->y);
					glm::vec2 projVel = Normalize(glm::vec2(Game::main.mouseX - projPos.x, Game::main.mouseY - projPos.y)) * m->projectileSpeed;

					if (projPos.x > screenLeft && projPos.x < screenRight &&
						projPos.y > screenBottom && projPos.y < screenTop)
					{
						Texture2D* s = Game::main.textureMap["aether_bullet"];
						m->lastProjectile = 0.0f;

						Entity* projectile = ECS::main.CreateEntity("Bullet");

						ECS::main.RegisterComponent(new PositionComponent(projectile, true, false, phys->pos->x, phys->pos->y, 0.0f), projectile);
						ECS::main.RegisterComponent(new PhysicsComponent(projectile, true, (PositionComponent*)projectile->componentIDMap[positionComponentID], projVel.x, projVel.y, 0.0f, 0.0f, 0.0f), projectile);
						ECS::main.RegisterComponent(new ColliderComponent(projectile, true, (PositionComponent*)projectile->componentIDMap[positionComponentID], false, false, false, true, false, true, EntityClass::object, 1.0f, 0.0f, 0.0f, 5.0f, 5.0f, 0.0f, 0.0f), projectile);
						ECS::main.RegisterComponent(new DamageComponent(projectile, true, true, 20.0f, true, 1, 10.0f, false, true, true), projectile);
						ECS::main.RegisterComponent(new StaticSpriteComponent(projectile, true, (PositionComponent*)projectile->componentIDMap[positionComponentID], s->width / 4.0f, s->height / 4.0f, s), projectile);
						ECS::main.RegisterComponent(new ParticleComponent(projectile, true, 0.01f, 0.0f, 0.0f, 10, Element::aether, 5.0f, 20.0f), projectile);
					}
				}
				else
				{
					m->lastProjectile += deltaTime;
				}

				if (glfwGetKey(Game::main.window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && move->canClimb)
				{
					move->shouldClimb = true;
				}
				else
				{
					move->shouldClimb = false;
				}

				if (glfwGetKey(Game::main.window, GLFW_KEY_T) == GLFW_PRESS && duel->hasSword && !duel->isAttacking && m->lastTick > 0.5f)
				{
					m->lastTick = 0.0f;

					if (duel->isDrawn)
					{
						duel->isDrawn = false;
					}
					else
					{
						duel->isDrawn = true;
					}
				}

				if (glfwGetMouseButton(Game::main.window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS && !duel->isAttacking && duel->hasSword && duel->isDrawn && !move->preparingToJump)
				{
					m->lastTick = 0.0f;

					if (Game::main.mouseX < phys->pos->x)
					{
						if (abs(phys->velocityX) < 0.5f)
						{
							phys->velocityX -= move->stabDepth;
							duel->isAttacking = true;
							move->canMove = false;
						}
						else if (move->jumping)
						{
							duel->isAttacking = true;
							move->canMove = false;
						}
					}
					else
					{
						if (abs(phys->velocityX) < 0.5f)
						{
							phys->velocityX += move->stabDepth;
							duel->isAttacking = true;
							move->canMove = false;
						}
						else if (move->jumping)
						{
							duel->isAttacking = true;
							move->canMove = false;
						}
					}
				}

				/*if (move->preparingToJump && m->projectionTime >= m->projectionDelay && abs(phys->velocityX) < 0.5f)
				{
					m->projecting = true;
					CalculateProjection(phys, m, move);
				}
				else if (move->preparingToJump && m->projectionTime < m->projectionDelay)
				{
					m->projectionTime += deltaTime;
				}*/

				/*if (glfwGetKey(Game::main.window, GLFW_KEY_SPACE) == GLFW_PRESS && move->canMove && !move->preparingToJump && col->onPlatform ||
					glfwGetKey(Game::main.window, GLFW_KEY_SPACE) == GLFW_PRESS && move->canMove && !move->preparingToJump && move->climbing)
				{
					move->preparingToJump = true;
				}*/

				if (m->coyoteTime < m->maxCoyoteTime && !col->onPlatform)
				{
					m->coyoteTime += deltaTime;
				}

				if (glfwGetKey(Game::main.window, GLFW_KEY_SPACE) != GLFW_PRESS && !m->releasedJump)
				{
					m->releasedJump = true;
				}

				if (glfwGetKey(Game::main.window, GLFW_KEY_SPACE) == GLFW_PRESS && move->canMove && col->onPlatform && m->releasedJump
					|| glfwGetKey(Game::main.window, GLFW_KEY_SPACE) == GLFW_PRESS && move->canMove && m->releasedJump && m->coyoteTime < m->maxCoyoteTime
					|| glfwGetKey(Game::main.window, GLFW_KEY_SPACE) == GLFW_PRESS && move->canMove && m->releasedJump && !col->onPlatform && m->maxJumps > 1 && m->jumps < m->maxJumps)
				{
					if (!col->onPlatform && m->jumps == 0 && m->coyoteTime > m->maxCoyoteTime)
					{
						m->jumps += 2;
					}
					else
					{
						m->jumps++;
					}

					if (phys->velocityY < 0)
					{
						phys->velocityY = 0;
					}

					ParticleEngine::main.AddParticles(25, phys->pos->x, phys->pos->y, Element::aether, rand() % 40 + 1);

					m->releasedJump = false;
					m->coyoteTime = m->maxCoyoteTime;

					m->projectionTime = 0;
					move->canMove = true;
					move->jumping = true;
					move->preparingToJump = false;

					move->shouldClimb = false;
					phys->velocityY += 250 * move->maxJumpHeight;

					/*if (!m->projecting)
					{
						phys->velocityY += 200 * move->maxJumpHeight;
					}
					else
					{
						m->projecting = false;
						float leapXVel, leapYVel;

						if (Game::main.mouseX < phys->pos->x)
						{
							leapXVel = max(-400 * move->maxJumpHeight, (Game::main.mouseX - phys->pos->x) * move->maxJumpHeight);
						}
						else
						{
							leapXVel = min(400 * move->maxJumpHeight, (Game::main.mouseX - phys->pos->x) * move->maxJumpHeight);
						}

						if (Game::main.mouseY < phys->pos->y)
						{
							leapYVel = max(-400 * move->maxJumpHeight, (Game::main.mouseY - phys->pos->y) * move->maxJumpHeight);
						}
						else
						{
							leapYVel = min(400 * move->maxJumpHeight, (Game::main.mouseY - phys->pos->y) * move->maxJumpHeight);
						}

						phys->velocityX += leapXVel;
						phys->velocityY += leapYVel;
					}*/
				}

				if (!m->releasedJump && move->jumping && phys->velocityY > 0)
				{
					phys->gravityMod = phys->baseGravityMod * 0.6f;
				}
				else
				{
					phys->gravityMod = phys->baseGravityMod;
				}

				if (glfwGetKey(Game::main.window, GLFW_KEY_W) == GLFW_PRESS && move->canMove && move->climbing)
				{
					if (phys->velocityY < move->maxSpeed)
					{
						phys->velocityY += move->acceleration * deltaTime;
					}
				}
				else if (glfwGetKey(Game::main.window, GLFW_KEY_S) == GLFW_PRESS && move->canMove && move->climbing)
				{
					if (phys->velocityY > -move->maxSpeed)
					{
						phys->velocityY -= move->acceleration * deltaTime;
					}
				}

				if (glfwGetKey(Game::main.window, GLFW_KEY_D) == GLFW_PRESS && move->canMove && !move->climbing)
				{
					if (phys->velocityX < move->maxSpeed)
					{
						if (abs(phys->velocityX) < 0.5f && col->onPlatform)
						{
							ParticleEngine::main.AddParticles(10, phys->pos->x, phys->pos->y - 30.0f, Element::dust, rand() % 10 + 1);
						}

						phys->velocityX += move->acceleration * deltaTime;
					}
				}
				else if (glfwGetKey(Game::main.window, GLFW_KEY_A) == GLFW_PRESS && move->canMove && !move->climbing)
				{
					if (phys->velocityX > -move->maxSpeed)
					{
						if (abs(phys->velocityX) < 0.5f && col->onPlatform)
						{
							ParticleEngine::main.AddParticles(10, phys->pos->x, phys->pos->y - 30.0f, Element::dust, rand() % 10 + 1);
						}

						phys->velocityX -= move->acceleration * deltaTime;
					}
				}

			}
			else
			{
				// You're dead, pal.
			}
		}
	}
}

void InputSystem::CalculateProjection(PhysicsComponent* phys, InputComponent* m, MovementComponent* move)
{
	float dT = 0.0025f;

	Texture2D* s = Game::main.textureMap["dot"];

	float screenLeft = (Game::main.camX - (Game::main.windowWidth * Game::main.zoom / 1.0f));
	float screenRight = (Game::main.camX + (Game::main.windowWidth * Game::main.zoom / 1.0f));
	float screenBottom = (Game::main.camY - (Game::main.windowHeight * Game::main.zoom / 1.0f));
	float screenTop = (Game::main.camY + (Game::main.windowHeight * Game::main.zoom / 1.0f));
	float screenElev = Game::main.camZ;

	float leapXVel, leapYVel;

	if (Game::main.mouseX < phys->pos->x)
	{
		leapXVel = max(-400 * move->maxJumpHeight, (Game::main.mouseX - phys->pos->x) * move->maxJumpHeight);
	}
	else
	{
		leapXVel = min(400 * move->maxJumpHeight, (Game::main.mouseX - phys->pos->x) * move->maxJumpHeight);
	}

	if (Game::main.mouseY < phys->pos->y)
	{
		leapYVel = max(-400 * move->maxJumpHeight, (Game::main.mouseY - phys->pos->y) * move->maxJumpHeight);
	}
	else
	{
		leapYVel = min(400 * move->maxJumpHeight, (Game::main.mouseY - phys->pos->y) * move->maxJumpHeight);
	}

	glm::vec2 projVel = glm::vec2(leapXVel, leapYVel);
	glm::vec2 projPos = glm::vec2(phys->pos->x, phys->pos->y);

	for (int i = 0; i < m->projectionDepth; i++)
	{
		projVel.y -= phys->gravityMod * dT;

		projPos += (projVel * dT);

		if (i % 25 == 0 && i != 0)
		{
			if (projPos.x + (s->width / 2.0f) > screenLeft && projPos.x - (s->width / 2.0f) < screenRight &&
				projPos.y + (s->height / 2.0f) > screenBottom && projPos.y - (s->height / 2.0f) < screenTop)
			{

				Game::main.renderer->prepareQuad(projPos, s->width / 2.0f, s->height / 2.0f, glm::vec4(1.0f, 1.0f, 1.0f, 0.5f), s->ID);
			}
		}
	}
}

void InputSystem::AddComponent(Component* component)
{
	move.push_back((InputComponent*)component);
}

void InputSystem::PurgeEntity(Entity* e)
{
	for (int i = 0; i < move.size(); i++)
	{
		if (move[i]->entity == e)
		{
			InputComponent* s = move[i];
			move.erase(std::remove(move.begin(), move.end(), s), move.end());
			delete s;
		}
	}
}

#pragma endregion

#pragma region Camera Follow System

void CameraFollowSystem::Update(float deltaTime)
{
	for (int i = 0; i < folls.size(); i++)
	{
		CameraFollowComponent* f = folls[i];

		if (f->active)
		{
			PositionComponent* pos = (PositionComponent*)f->entity->componentIDMap[positionComponentID];

			Game::main.camX = Lerp(Game::main.camX, pos->x, f->speed * deltaTime);
			Game::main.camY = Lerp(Game::main.camY, pos->y, f->speed * deltaTime);
		}
	}
}

float CameraFollowSystem::Lerp(float a, float b, float t)
{
	return (1 - t) * a + t * b;
}

void CameraFollowSystem::AddComponent(Component* component)
{
	folls.push_back((CameraFollowComponent*)component);
}

void CameraFollowSystem::PurgeEntity(Entity* e)
{
	for (int i = 0; i < folls.size(); i++)
	{
		if (folls[i]->entity == e)
		{
			CameraFollowComponent* s = folls[i];
			folls.erase(std::remove(folls.begin(), folls.end(), s), folls.end());
			delete s;
		}
	}
}

#pragma endregion

#pragma region Animation Controller System

void AnimationControllerSystem::Update(float deltaTime)
{
	for (int i = 0; i < controllers.size(); i++)
	{
		AnimationControllerComponent* c = controllers[i];

		if (c->subID == dragonriderAnimControllerSubID)
		{
			// I'm thinking what we'll do is just hard code the various animation conditions
			// into the animation controller; this will serve as the animation controller
			// for the player and other dragon riders.

			// We are going to assume that any entity with a dragon rider animation controller component
			// (that is a long-ass name) also has a physics and collider component.
			// I think I can safely assume this because dragonriders should basically always
			// have the same set of components, aside from the player's.

			PlayerAnimationControllerComponent* d = (PlayerAnimationControllerComponent*)c;
			PhysicsComponent* p = (PhysicsComponent*)d->entity->componentIDMap[physicsComponentID];
			ColliderComponent* col = (ColliderComponent*)d->entity->componentIDMap[colliderComponentID];
			MovementComponent* move = (MovementComponent*)d->entity->componentIDMap[movementComponentID];
			HealthComponent* health = (HealthComponent*)d->entity->componentIDMap[healthComponentID];
			DuelistComponent* duel = (DuelistComponent*)d->entity->componentIDMap[duelistComponentID];

			std::string s = "";

			if (duel->hasSword && duel->isDrawn)
			{
				s = "sword_";
			}

			if (!health->dead)
			{
				if (p->velocityX < -10.0f)
				{
					c->animator->flipped = true;
				}
				else if (p->velocityX > 10.0f)
				{
					c->animator->flipped = false;
				}

				if (d->entity->componentIDMap[inputComponentID] != nullptr)
				{
					if (move->preparingToJump && Game::main.mouseX < p->pos->x)
					{
						c->animator->flipped = true;
					}
					else if (move->preparingToJump && Game::main.mouseX > p->pos->x)
					{
						c->animator->flipped = false;
					}
				}

				if (duel->isAttacking)
				{
					if (c->animator->activeAnimation != s + "stab" && !move->jumping)
					{
						c->animator->SetAnimation(s + "stab");
					}
					else if (c->animator->activeAnimation != s + "aerialOne" && move->jumping)
					{
						c->animator->SetAnimation(s + "aerialOne");
					}
				}
				else if (abs(p->velocityY) > 100.0f && !col->onPlatform)
				{
					if (c->animator->activeAnimation != s + "jumpUp" && p->velocityY > 0)
					{
						c->animator->SetAnimation(s + "jumpUp");
					}
					else if (c->animator->activeAnimation != s + "jumpDown" && p->velocityY < 0)
					{
						c->animator->SetAnimation(s + "jumpDown");
					}
				}
				else if (move->preparingToJump && abs(p->velocityX) < 0.5f && c->animator->activeAnimation != s + "jumpPrep")
				{
					c->animator->SetAnimation(s + "jumpPrep");
				}
				else if (abs(p->velocityX) > 25.0f && col->onPlatform && move->canMove && c->animator->activeAnimation != s + "walk")
				{
					c->animator->SetAnimation(s + "walk");
				}
				else if (abs(p->velocityX) < 10.0f && !col->collidedLastTick && col->onPlatform && !move->preparingToJump && move->canMove && c->animator->activeAnimation != s + "idle")
				{
					c->animator->SetAnimation(s + "idle");
				}
			}
			else if (c->animator->activeAnimation != s + "dead")
			{
				c->animator->SetAnimation(s + "dead");
			}
		}
	}
}

void AnimationControllerSystem::AddComponent(Component* component)
{
	controllers.push_back((AnimationControllerComponent*)component);
}

void AnimationControllerSystem::PurgeEntity(Entity* e)
{
	for (int i = 0; i < controllers.size(); i++)
	{
		if (controllers[i]->entity == e)
		{
			AnimationControllerComponent* s = controllers[i];
			controllers.erase(std::remove(controllers.begin(), controllers.end(), s), controllers.end());
			delete s;
		}
	}
}

#pragma endregion

#pragma region Animation System

void AnimationSystem::Update(float deltaTime)
{
	float screenLeft = (Game::main.camX - (Game::main.windowWidth * Game::main.zoom / 1.0f));
	float screenRight = (Game::main.camX + (Game::main.windowWidth * Game::main.zoom / 1.0f));
	float screenBottom = (Game::main.camY - (Game::main.windowHeight * Game::main.zoom / 1.0f));
	float screenTop = (Game::main.camY + (Game::main.windowHeight * Game::main.zoom / 1.0f));
	float screenElev = Game::main.camZ;

	for (int i = 0; i < anims.size(); i++)
	{
		// Animations work by taking a big-ass spritesheet
		// and moving through the uvs by increments equal
		// to one divided by the width and height of each sprite;
		// this means we need to know how many such cells are in
		// the whole sheet (for both rows and columns), so that
		// we can feed the right cell coordinates into the
		// renderer. This shouldn't be too difficult; the real
		// question is how we'll manage conditions for different
		// animations.
		// We could just have a map containing strings and animations
		// and set the active animation by calling some function, sending
		// to that the name of the requested animation in the form of that
		// string, but that doesn't seem like the ideal way to do it.
		// We might try that first and then decide later whether
		// there isn't a better way to handle this.

		AnimationComponent* a = anims[i];

		if (a->active)
		{
			a->lastTick += deltaTime;

			Animation2D* activeAnimation = a->animations[a->activeAnimation];

			int cellX = a->activeX, cellY = a->activeY;

			if (activeAnimation->speed < a->lastTick)
			{
				a->lastTick = 0;

				if (a->activeX + 1 < activeAnimation->rowsToCols[cellY])
				{
					cellX = a->activeX += 1;
				}
				else
				{
					if (activeAnimation->loop ||
						a->activeY > 0)
					{
						cellX = a->activeX = 0;
					}

					if (a->activeY - 1 >= 0)
					{
						cellY = a->activeY -= 1;
					}
					else if (activeAnimation->loop)
					{
						cellX = a->activeX = 0;
						cellY = a->activeY = activeAnimation->rows - 1;
					}
				}
			}

			PositionComponent* pos = a->pos;

			if (pos->x + ((activeAnimation->width / activeAnimation->columns) / 2.0f) > screenLeft && pos->x - ((activeAnimation->width / activeAnimation->columns) / 2.0f) < screenRight &&
				pos->y + ((activeAnimation->height / activeAnimation->rows) / 2.0f) > screenBottom && pos->y - ((activeAnimation->height / activeAnimation->rows) / 2.0f) < screenTop &&
				pos->z < screenElev)
			{
				// std::cout << std::to_string(activeAnimation->width) + "/" + std::to_string(activeAnimation->height) + "\n";
				Game::main.renderer->prepareQuad(pos, activeAnimation->width, activeAnimation->height, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), activeAnimation->ID, cellX, cellY, activeAnimation->columns, activeAnimation->rows, a->flipped);
			}

		}
	}
}

void AnimationSystem::AddComponent(Component* component)
{
	anims.push_back((AnimationComponent*)component);
}

void AnimationSystem::PurgeEntity(Entity* e)
{
	for (int i = 0; i < anims.size(); i++)
	{
		if (anims[i]->entity == e)
		{
			AnimationComponent* s = anims[i];
			anims.erase(std::remove(anims.begin(), anims.end(), s), anims.end());
			delete s;
		}
	}
}

#pragma endregion

#pragma region Health System

void HealthSystem::Update(float deltaTime)
{
	for (int i = 0; i < healths.size(); i++)
	{
		HealthComponent* h = healths[i];

		if (h->health <= 0.0f)
		{
			// You're dead.
			h->dead = true;

			h->active = false;
			ECS::main.AddDeadEntity(h->entity);
		}
	}
}

void HealthSystem::AddComponent(Component* component)
{
	healths.push_back((HealthComponent*)component);
}

void HealthSystem::PurgeEntity(Entity* e)
{
	for (int i = 0; i < healths.size(); i++)
	{
		if (healths[i]->entity == e)
		{
			HealthComponent* s = healths[i];
			healths.erase(std::remove(healths.begin(), healths.end(), s), healths.end());
			delete s;
		}
	}
}

#pragma endregion

#pragma region Duelist System

void DuellingSystem::Update(float deltaTime)
{
	for (int i = 0; i < duels.size(); i++)
	{
		DuelistComponent* d = duels[i];

		if (d->active)
		{
			if (d->isAttacking)
			{
				d->lastTick += deltaTime;
				PhysicsComponent* p = (PhysicsComponent*)d->entity->componentIDMap[physicsComponentID];
				MovementComponent* m = (MovementComponent*)d->entity->componentIDMap[movementComponentID];

				if (!d->hasSword || !d->isDrawn)
				{
					// if (!m->jumping && !m->preparingToJump && abs(p->velocityX) < 0.5f && abs(p->velocityY) < 0.5f)
					if (!m->preparingToJump && !m->climbing)
					{
						d->isAttacking = false;
						m->canMove = true;
					}
				}

				if (d->lastTick > 0.5f)
				{
					// if (!m->jumping && !m->preparingToJump && abs(p->velocityX) < 0.5f && abs(p->velocityY) < 0.5f)
					if (!m->preparingToJump && !m->climbing)
					{
						d->lastTick = 0;
						d->isAttacking = false;
						m->canMove = true;
					}
				}
			}
		}
	}
}

void DuellingSystem::AddComponent(Component* component)
{
	duels.push_back((DuelistComponent*)component);
}

void DuellingSystem::PurgeEntity(Entity* e)
{
	for (int i = 0; i < duels.size(); i++)
	{
		if (duels[i]->entity == e)
		{
			DuelistComponent* s = duels[i];
			duels.erase(std::remove(duels.begin(), duels.end(), s), duels.end());
			delete s;
		}
	}
}

#pragma endregion

#pragma region Particle System

void ParticleSystem::Update(float deltaTime)
{
	float screenLeft = (Game::main.camX - (Game::main.windowWidth * Game::main.zoom / 1.0f));
	float screenRight = (Game::main.camX + (Game::main.windowWidth * Game::main.zoom / 1.0f));
	float screenBottom = (Game::main.camY - (Game::main.windowHeight * Game::main.zoom / 1.0f));
	float screenTop = (Game::main.camY + (Game::main.windowHeight * Game::main.zoom / 1.0f));
	float screenElev = Game::main.camZ;

	for (int i = 0; i < particles.size(); i++)
	{
		ParticleComponent* p = particles[i];

		if (p->active)
		{
			if (p->lastTick >= p->tickRate)
			{
				p->lastTick = 0.0f;
				PositionComponent* pos = (PositionComponent*)p->entity->componentIDMap[positionComponentID];
				glm::vec2 pPos = glm::vec2(pos->x + p->xOffset, pos->y + p->yOffset);

				if (pPos.x > screenLeft && pPos.x < screenRight &&
					pPos.y > screenBottom && pPos.y < screenTop)
				{
					float lifetime = p->minLifetime + static_cast<float>(rand()) * static_cast<float>(p->maxLifetime - p->minLifetime) / RAND_MAX;

					ParticleEngine::main.AddParticles(p->number, pPos.x, pPos.y, p->element, lifetime);
				}
			}
			else
			{
				p->lastTick += deltaTime;
			}
		}
	}
}

void ParticleSystem::AddComponent(Component* component)
{
	particles.push_back((ParticleComponent*)component);
}

void ParticleSystem::PurgeEntity(Entity* e)
{
	for (int i = 0; i < particles.size(); i++)
	{
		if (particles[i]->entity == e)
		{
			ParticleComponent* s = particles[i];
			particles.erase(std::remove(particles.begin(), particles.end(), s), particles.end());
			delete s;
		}
	}
}

#pragma endregion

#pragma endregion
