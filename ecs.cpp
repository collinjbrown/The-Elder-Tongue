// ecs.cpp

#include "particleengine.h"
#include "ecs.h"
#include "system.h"
#include "component.h"
#include "entity.h"
#include <algorithm>

#pragma region Utility

float Norm(glm::vec2 a)
{
	return sqrt(a.x * a.x + a.y * a.y);
}

glm::vec2 Normalize(glm::vec2 a)
{
	return a * (1 / Norm(a));
}

bool PointOverlapRect(glm::vec2 point, glm::vec2 rectCenter, float rWidth, float rHeight)
{
	glm::vec2 rBL = glm::vec2(rectCenter.x - (rWidth / 2.0f), rectCenter.y - (rHeight / 2.0f));
	glm::vec2 rTR = glm::vec2(rectCenter.x + (rWidth / 2.0f), rectCenter.y + (rHeight / 2.0f));

	return (point.x >= rBL.x && point.y >= rBL.y && point.x < rTR.x&& point.y < rTR.y);
}

bool RectOverlapRect(glm::vec2 rectA, float aWidth, float aHeight, glm::vec2 rectB, float bWidth, float bHeight)
{
	glm::vec2 aBL = glm::vec2(rectA.x - (aWidth / 2.0f), rectA.y - (aHeight / 2.0f));
	glm::vec2 aTR = glm::vec2(rectA.x + (aWidth / 2.0f), rectA.y + (aHeight / 2.0f));
	glm::vec2 bBL = glm::vec2(rectA.x - (aWidth / 2.0f), rectA.y - (aHeight / 2.0f));
	glm::vec2 bTR = glm::vec2(rectB.x + (bWidth / 2.0f), rectB.y + (bHeight / 2.0f));

	return (aBL.x < bTR.x&& aTR.x > bBL.x && aBL.y < bTR.y&& aTR.y > bBL.y);
}

bool RayOverlapRect(glm::vec2 rayOrigin, glm::vec2 rayDir, glm::vec2 rectCenter, float rWidth, float rHeight,
					glm::vec2& contactPoint, glm::vec2& contactNormal, float& tHitNear)
{
	glm::vec2 invertDir = 1.0f / rayDir;

	glm::vec2 rBL = glm::vec2(rectCenter.x - (rWidth / 2.0f), rectCenter.y - (rHeight / 2.0f));
	glm::vec2 rTR = glm::vec2(rectCenter.x + (rWidth / 2.0f), rectCenter.y + (rHeight / 2.0f));

	glm::vec2 tNear = (rBL - rayOrigin) * invertDir;
	glm::vec2 tFar = (rTR - rayOrigin) * invertDir;

	if (std::isnan(tFar.y) || std::isnan(tFar.x))
	{
		return false;
	}
	if (std::isnan(tNear.y) || std::isnan(tNear.x))
	{
		return false;
	}

	if (tNear.x > tFar.x)
	{
		std::swap(tNear.x, tFar.x);
	}
	if (tNear.y > tFar.y)
	{
		std::swap(tNear.y, tFar.y);
	}

	if (tNear.x > tFar.y || tNear.y > tFar.x)
	{
		return false;
	}

	tHitNear = std::max(tNear.x, tNear.y);
	float tHitFar = std::min(tFar.x, tFar.y);

	if (tHitFar < 0)
	{
		return false;
	}

	contactPoint = rayOrigin + tHitNear * rayDir;

	if (tNear.x > tNear.y)
	{
		if (invertDir.x < 0)
		{
			contactNormal = glm::vec2(1, 0);
		}
		else
		{
			contactNormal = glm::vec2(-1, 0);
		}
	}
	else
	{
		if (invertDir.y < 0)
		{
			contactNormal = glm::vec2(0, 1);
		}
		else
		{
			contactNormal = glm::vec2(0, -1);
		}
	}

	/*Texture2D* t = Game::main.textureMap["blank"];
	Game::main.renderer->prepareQuad(contactPoint, t->width, t->height, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), t->ID);
	Game::main.renderer->prepareQuad(contactPoint + contactNormal, t->width / 2.0f, t->height * 2.0f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), t->ID);
	Game::main.renderer->prepareQuad(rayOrigin + rayDir, t->width / 2.0f, t->height / 2.0f, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), t->ID);*/

	return true;
}

glm::vec2 lerp(glm::vec2 pos, glm::vec2 tar, float step)
{
	return (pos * (1.0f - step) + (tar * step));
}

float Dot(glm::vec2 a, glm::vec2 b)
{
	return a.x * b.x + a.y * b.y;
}
#pragma endregion

#pragma region Entities

int Entity::Get_ID() { return ID; }
int Entity::Get_Scene() { return scene; }
std::string Entity::Get_Name() { return name; }

void Entity::Set_ID(int newID) { ID = newID; }
void Entity::Set_Scene(int newScene) { scene = newScene; }
void Entity::Set_Name(std::string newName) { name = newName; }

Entity:: Entity(int ID, int scene, std::string name)
{
	this->ID = ID;
	this->scene = scene;
	this->name = name;
};

#pragma endregion

#pragma region Component Blocks
void ComponentBlock::Update(int activeScene, float deltaTime)
{
	system->Update(activeScene, deltaTime);
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

	AISystem* aiSystem = new AISystem();
	ComponentBlock* aiBlock = new ComponentBlock(aiSystem, aiComponentID);
	componentBlocks.push_back(aiBlock);

	InputSystem* inputSystem = new InputSystem();
	ComponentBlock* inputBlock = new ComponentBlock(inputSystem, inputComponentID);
	componentBlocks.push_back(inputBlock);

	BladeSystem* bladeSystem = new BladeSystem();
	ComponentBlock* bladeBlock = new ComponentBlock(bladeSystem, bladeComponentID);
	componentBlocks.push_back(bladeBlock);

	PhysicsSystem* physicsSystem = new PhysicsSystem();
	ComponentBlock* physicsBlock = new ComponentBlock(physicsSystem, physicsComponentID);
	componentBlocks.push_back(physicsBlock);

	ParticleSystem* particleSystem = new ParticleSystem();
	ComponentBlock* particleBlock = new ComponentBlock(particleSystem, particleComponentID);
	componentBlocks.push_back(particleBlock);

	ColliderSystem* colliderSystem = new ColliderSystem();
	ComponentBlock* colliderBlock = new ComponentBlock(colliderSystem, colliderComponentID);
	componentBlocks.push_back(colliderBlock);

	DamageSystem* damageSystem = new DamageSystem();
	ComponentBlock* damageBlock = new ComponentBlock(damageSystem, damageComponentID);
	componentBlocks.push_back(damageBlock);

	HealthSystem* healthSystem = new HealthSystem();
	ComponentBlock* healthBlock = new ComponentBlock(healthSystem, healthComponentID);
	componentBlocks.push_back(healthBlock);

	PositionSystem* positionSystem = new PositionSystem();
	ComponentBlock* positionBlock = new ComponentBlock(positionSystem, positionComponentID);
	componentBlocks.push_back(positionBlock);

	ImageSystem* imageSystem = new ImageSystem();
	ComponentBlock* imageBlock = new ComponentBlock(imageSystem, imageComponentID);
	componentBlocks.push_back(imageBlock);

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
		#pragma region UI Instantiation

		Entity* alphaWatermark = CreateEntity(0, "Alpha Watermark");
		Texture2D* watermark = Game::main.textureMap["watermark"];
		Texture2D* watermarkMap = Game::main.textureMap["watermarkMap"];

		ECS::main.RegisterComponent(new PositionComponent(alphaWatermark, true, true, 0, 0, 100, 0), alphaWatermark);
		ECS::main.RegisterComponent(new StaticSpriteComponent(alphaWatermark, true, (PositionComponent*)alphaWatermark->componentIDMap[positionComponentID], watermark->width, watermark->height, 1.0f, watermark, watermarkMap, false, false), alphaWatermark);
		ECS::main.RegisterComponent(new ImageComponent(alphaWatermark, true, Anchor::topRight, 0, 0), alphaWatermark);

		#pragma endregion

		#pragma region Moonlight Blade Instantiation

		player = CreateEntity(0, "The Player");
		Entity* moonlightBlade = CreateEntity(0, "Moonlight Blade");
		Texture2D* moonlightBladeTex = Game::main.textureMap["moonlightBlade"];
		Texture2D* moonlightBladeMap = Game::main.textureMap["moonlightBladeMap"];
		Texture2D* moonlightBladeIncorporealMap = Game::main.textureMap["moonlightBladeIncorporealMap"];

		ECS::main.RegisterComponent(new PositionComponent(moonlightBlade, true, false, 0, 0, 0, 0.0f), moonlightBlade);
		ECS::main.RegisterComponent(new PhysicsComponent(moonlightBlade, true, (PositionComponent*)moonlightBlade->componentIDMap[positionComponentID], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f), moonlightBlade);
		ECS::main.RegisterComponent(new StaticSpriteComponent(moonlightBlade, true, (PositionComponent*)moonlightBlade->componentIDMap[positionComponentID], moonlightBladeTex->width, moonlightBladeTex->height, 1.0f, moonlightBladeTex, moonlightBladeMap, false, false), moonlightBlade);
		// ECS::main.RegisterComponent(new AIComponent(moonlightBlade, true, false, 1010.0f, 1000.0f, 0.5f, 0.0f, 0.0f, AIType::moonlight_blade), moonlightBlade);
		ECS::main.RegisterComponent(new ColliderComponent(moonlightBlade, false, (PositionComponent*)moonlightBlade->componentIDMap[positionComponentID], false, false, true, false, true, false, true, EntityClass::object, 1.0f, 0.0f, 0.0f, 5.0f, 5.0f, 0.0f, 0.0f), moonlightBlade);
		ECS::main.RegisterComponent(new DamageComponent(moonlightBlade, true, player, false, 0.0f, true, false, 100, 20.0f, false, true, true, true), moonlightBlade);

		Entity* hilt = CreateEntity(0, "Moonlight Blade Hilt");
		ECS::main.RegisterComponent(new PositionComponent(hilt, true, false, 0, 0, 0, 0.0f), hilt);
		ECS::main.RegisterComponent(new PhysicsComponent(hilt, true, (PositionComponent*)hilt->componentIDMap[positionComponentID], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f), hilt);
		ColliderComponent* platformCollider = new ColliderComponent(hilt, false, (PositionComponent*)hilt->componentIDMap[positionComponentID], true, true, true, false, false, false, false, EntityClass::object, 1.0f, 0.0f, 0.0f, 35.0f, 5.0f, 0.0f, 0.0f);
		ECS::main.RegisterComponent(platformCollider, hilt);
		ECS::main.RegisterComponent(new BladeComponent(moonlightBlade, true, 1010.0f, 1000.0f, 30000.0f, 0.5f, 1000.0f, platformCollider, moonlightBladeMap, moonlightBladeIncorporealMap, 0.5f), moonlightBlade);

		#pragma endregion

		#pragma region Player Instantiation
		Texture2D* lilyMap = Game::main.textureMap["lilyMap"];
		Animation2D* anim1 = Game::main.animationMap["baseIdle"];

		ECS::main.RegisterComponent(new PositionComponent(player, true, false, 0, 100, 0, 0.0f), player);
		ECS::main.RegisterComponent(new PhysicsComponent(player, true, (PositionComponent*)player->componentIDMap[positionComponentID], 0.0f, 0.0f, 0.0f, 5000.0f, 2000.0f), player);
		ECS::main.RegisterComponent(new ColliderComponent(player, true, (PositionComponent*)player->componentIDMap[positionComponentID], false, false, false, false, false, true, false, EntityClass::player, 1.0f, 1.0f, 10.0f, 20.0f, 50.0f, 0.0f, -7.75f), player);
		ECS::main.RegisterComponent(new MovementComponent(player, true, 4000.0f, 500.0f, 2.5f, 0.5f, true, 0.7f, true, false, 0.9f, 2.0f, glm::vec2(100.0f, 0), 50.0f, 20.0f, 1.5f, 0.1f, 0.35f, 5, 3.0f), player);
		ECS::main.RegisterComponent(new InputComponent(player, true, moonlightBlade, true, 0.5f, 5000.0f, 0.5f, 2, 0.5f, 2.0f, 500.0f), player);
		ECS::main.RegisterComponent(new CameraFollowComponent(player, true, 10.0f), player);
		ECS::main.RegisterComponent(new HealthComponent(player, true, 1000.0f, false), player);
		ECS::main.RegisterComponent(new AnimationComponent(player, true, (PositionComponent*)player->componentIDMap[positionComponentID], anim1, "idle", lilyMap, 1.0f), player);
		AnimationComponent* a = (AnimationComponent*)player->componentIDMap[animationComponentID];
		ECS::main.RegisterComponent(new PlayerAnimationControllerComponent(player, true, a), player);
		a->AddAnimation("walk", Game::main.animationMap["baseWalk"]);
		a->AddAnimation("crouch", Game::main.animationMap["baseCrouch"]);
		a->AddAnimation("crouchWalk", Game::main.animationMap["baseCrouchWalk"]);
		a->AddAnimation("jumpUp", Game::main.animationMap["baseJumpUp"]);
		a->AddAnimation("wallRun", Game::main.animationMap["baseWallRun"]);
		a->AddAnimation("slide", Game::main.animationMap["baseSlide"]);
		a->AddAnimation("slideDown", Game::main.animationMap["baseSlideDown"]);
		a->AddAnimation("jumpDown", Game::main.animationMap["baseJumpDown"]);
		a->AddAnimation("slashOne", Game::main.animationMap["baseSlashOne"]);
		a->AddAnimation("slashTwo", Game::main.animationMap["baseSlashTwo"]);
		a->AddAnimation("dead", Game::main.animationMap["baseDeath"]);
		#pragma endregion

		//#pragma region Test Character Instantiation
		//Animation2D* anim7 = Game::main.animationMap["testIdle"];
		//Animation2D* anim8 = Game::main.animationMap["testWalk"];
		//Animation2D* anim10 = Game::main.animationMap["testJumpUp"];
		//Animation2D* anim11 = Game::main.animationMap["testJumpDown"];
		//Animation2D* anim12 = Game::main.animationMap["testDeath"];

		//Entity* character = CreateEntity("Test Character");
		//ECS::main.RegisterComponent(new PositionComponent(character, true, false, 100, 100, 0.0f), character);
		//ECS::main.RegisterComponent(new PhysicsComponent(character, true, (PositionComponent*)character->componentIDMap[positionComponentID], 0.0f, 0.0f, 0.0f, 200.0f, 1000.0f), character);
		//ECS::main.RegisterComponent(new ColliderComponent(character, true, (PositionComponent*)character->componentIDMap[positionComponentID], false, false, false, false, true, false, EntityClass::enemy, 1.0f, 1.0f, 1.0f, 25.0f, 55.0f, 0.0f, 0.0f), character);
		//ECS::main.RegisterComponent(new MovementComponent(character, true, 1000.0f, 500.0f, 2.5f, 1.0f, 0.25f, true, true, false), character);
		//ECS::main.RegisterComponent(new AnimationComponent(character, true, (PositionComponent*)character->componentIDMap[positionComponentID], anim7, "idle"), character);
		//ECS::main.RegisterComponent(new DuelistComponent(character, true, false, false), character);
		//ECS::main.RegisterComponent(new HealthComponent(character, true, 100.0f, false), character);
		//AnimationComponent* a2 = (AnimationComponent*)character->componentIDMap[animationComponentID];
		//ECS::main.RegisterComponent(new PlayerAnimationControllerComponent(character, true, a2), character);
		//a2->AddAnimation("walk", anim8);
		//a2->AddAnimation("jumpUp", anim10);
		//a2->AddAnimation("jumpDown", anim11);
		//a2->AddAnimation("dead", anim12);
		//#pragma endregion

		/*Texture2D* wallTex = Game::main.textureMap["wall"];
		Texture2D* wallMap = Game::main.textureMap["wallMap"];

		Entity* wall = CreateEntity(0, "wall");
		ECS::main.RegisterComponent(new PositionComponent(wall, true, true, 0, 0, -100, 0), wall);
		ECS::main.RegisterComponent(new StaticSpriteComponent(wall, true, (PositionComponent*)wall->componentIDMap[positionComponentID], wallTex->width, wallTex->height, wallTex, wallMap, false, false), wall);*/

		Texture2D* tex3 = Game::main.textureMap["blank"];
		Texture2D* tex3Map = Game::main.textureMap["base_map"];

		for (int i = 0; i < 25; i++)
		{
			float width = rand() % 1000 + 300;
			float height = rand() % 1000 + 300;

			Entity* platform = CreateEntity(0, "floor");
			ECS::main.RegisterComponent(new PositionComponent(platform, true, true, rand() % 5000, rand() % 5000, 0, 0), platform);
			ECS::main.RegisterComponent(new PhysicsComponent(platform, true, (PositionComponent*)platform->componentIDMap[positionComponentID], 0.0f, 0.0f, 0.0f, 0.1f, 0.0f), platform);
			ECS::main.RegisterComponent(new ColliderComponent(platform, true, (PositionComponent*)platform->componentIDMap[positionComponentID], true, false, false, true, false, false, false, EntityClass::object, 1000.0f, 0.0f, 1.0f, width, height, 0.0f, 0.0f), platform);
			ECS::main.RegisterComponent(new StaticSpriteComponent(platform, true, (PositionComponent*)platform->componentIDMap[positionComponentID], width, height, 1.0f, tex3, tex3Map, false, false), platform);
		}

		for (int i = 0; i < 50; i++)
		{
			Entity* floor = CreateEntity(0, "floor");
			ECS::main.RegisterComponent(new PositionComponent(floor, true, true, i * 500, -200, 0, 0.0f), floor);
			ECS::main.RegisterComponent(new PhysicsComponent(floor, true, (PositionComponent*)floor->componentIDMap[positionComponentID], 0.0f, 0.0f, 0.0f, 0.1f, 0.0f), floor);
			ECS::main.RegisterComponent(new ColliderComponent(floor, true, (PositionComponent*)floor->componentIDMap[positionComponentID], true, false, false, true, false, false, false, EntityClass::object, 1000.0f, 0.0f, 1.0f, 540.0f, 80.0f, 0.0f, 0.0f), floor);
			ECS::main.RegisterComponent(new StaticSpriteComponent(floor, true, (PositionComponent*)floor->componentIDMap[positionComponentID], 540.0f, 80.0f, 1.0f, tex3, tex3Map, false, false), floor);

			Entity* earth = CreateEntity(0, "floor");
			ECS::main.RegisterComponent(new PositionComponent(earth, true, true, i * 500, -1000, 0, 0.0f), earth);
			ECS::main.RegisterComponent(new StaticSpriteComponent(earth, true, (PositionComponent*)earth->componentIDMap[positionComponentID], tex3->width * 35, tex3->height * 100.0f, 1.0f, tex3, tex3Map, false, false), earth);
		}

		// Do this after we instantiate objects so that it can properly sort out which nodes
		// are blocked and which aren't.
		// CreateNodeMap();
	}

	for (int i = 0; i < componentBlocks.size(); i++)
	{
		componentBlocks[i]->Update(activeScene, deltaTime);
	}

	PurgeDeadEntities();
}

// We probably aren't actually gonna use this, but I'll leave it here just in case.
void ECS::CreateNodeMap()
{
	for (int x = 0; x < mWidth; x++)
	{
		for (int y = 0; y < mHeight; y++)
		{
			bool blocked = false;
			Node* n = new Node(x, y);

			for (int e = 0; e < entities.size(); e++)
			{
				PositionComponent* pos = (PositionComponent*)entities[e]->componentIDMap[positionComponentID];
				ColliderComponent* col = (ColliderComponent*)entities[e]->componentIDMap[colliderComponentID];

				blocked = (PointOverlapRect(glm::vec2(x * nodeSize, y * nodeSize), glm::vec2(pos->x, pos->y) + glm::vec2(col->offsetX, col->offsetY), col->width, col->height) && !col->trigger && pos->stat);
				
				if (blocked)
				{
					n->col = col;
				}

				nodeMap[x][y] = n;
			}
		}
	}
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

Entity* ECS::CreateEntity(int scene, std::string name)
{
	Entity* e = new Entity(GetID(), scene, name);
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

PositionComponent::PositionComponent(Entity* entity, bool active, bool stat, float x, float y, float z, float rotation)
{
	ID = positionComponentID;
	this->active = active;
	this->entity = entity;
	this->stat = stat;
	this->x = x;
	this->y = y;
	this->z = z;
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
	this->baseDrag = drag;
	this->gravityMod = gravityMod;
	this->baseGravityMod = gravityMod;
}

#pragma endregion

#pragma region Static Sprite Component

StaticSpriteComponent::StaticSpriteComponent(Entity* entity, bool active, PositionComponent* pos, float width, float height, float scale, Texture2D* sprite, Texture2D* map, bool flipped, bool tiled)
{
	ID = spriteComponentID;
	this->active = active;
	this->entity = entity;
	this->pos = pos;

	this->width = width;
	this->height = height;
	
	this->scale = scale;

	this->sprite = sprite;
	this->map = map;

	this->flipped = flipped;
	this->tiled = tiled;
}

#pragma endregion

#pragma region Collider Component

ColliderComponent::ColliderComponent(Entity* entity, bool active, PositionComponent* pos, bool platform, bool onewayPlatform, bool ignoreOnewayPlatforms, bool climbable, bool trigger, bool takesDamage, bool doesDamage, EntityClass entityClass, float mass, float bounce, float friction, float width, float height, float offsetX, float offsetY)
{
	ID = colliderComponentID;
	this->active = active;
	this->entity = entity;
	this->pos = pos;

	this->platform = platform;
	this->onewayPlatform = onewayPlatform;
	this->ignoreOnewayPlatforms = ignoreOnewayPlatforms;

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
	this->baseHeight = height;

	this->offsetX = offsetX;
	this->offsetY = offsetY;
	this->baseOffsetY = offsetY;
}

#pragma endregion

#pragma region Input Component

InputComponent::InputComponent(Entity* entity, bool active, Entity* moonlightBlade, bool acceptInput, float projectionDelay, float projectionDepth, float maxCoyoteTime, int maxJumps, float projectileDelay, float slashSpeed, float projectileSpeed)
{
	this->ID = inputComponentID;
	this->active = active;
	this->entity = entity;

	this->moonlightBlade = moonlightBlade;
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

	this->slashSpeed = slashSpeed;
	this->projectileSpeed = projectileSpeed;
}

#pragma endregion

#pragma region Movement Component

MovementComponent::MovementComponent(Entity* entity, bool active, float acceleration, float maxSpeed, float maxJumpHeight, float airControl, bool canMove, float crouchMod, bool canClimb, bool shouldClimb, float climbMod, float maxWallRun,
	glm::vec2 attackThrust, float slashSpeed, float damage, float attackMultiplier, float minAttackDelay, float maxAttackDelay, int maxFlurry, float flurryDelay)
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
	this->airControl = airControl;

	this->crouching = false;
	this->crouchMod = crouchMod;

	this->wallRunning = false;
	this->maxWallRun = maxWallRun;

	this->canClimb = canClimb;
	this->shouldClimb = shouldClimb;
	this->climbing = false;
	this->climbMod = climbMod;

	this->maxClimbHeight = 0.0f;
	this->minClimbHeight = 0.0f;

	this->slashSpeed = slashSpeed;
	this->attackThrust = attackThrust;

	this->attackNumber = 0;
	this->damage = damage;
	this->attackMultiplier = attackMultiplier;

	this->minAttackDelay = minAttackDelay;
	this->maxAttackDelay = maxAttackDelay;

	this->lastFlurry = 0.0f;
	this->maxFlurry = maxFlurry;
	this->flurryDelay = flurryDelay;

	this->isAttacking = false;
	this->lastAttack = 0.0f;
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

AnimationComponent::AnimationComponent(Entity* entity, bool active, PositionComponent* pos, Animation2D* idleAnimation, std::string animationName, Texture2D* map, float scale)
{
	this->ID = animationComponentID;
	this->entity = entity;
	this->active = active;

	lastTick = 0;
	activeX = 0;
	activeY = 0;
	flipped = false;

	this->pos = pos;

	this->scale = scale;

	this->map = map;
	activeAnimation = animationName;
	animations.emplace(animationName, idleAnimation);
	activeY = animations[activeAnimation]->rows - 1;
}

#pragma endregion

#pragma region Player Animation Controller Component

PlayerAnimationControllerComponent::PlayerAnimationControllerComponent(Entity* entity, bool active, AnimationComponent* animator)
{
	this->ID = animationControllerComponentID;
	this->subID = lilyAnimControllerSubID;
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

#pragma region Damage Component

DamageComponent::DamageComponent(Entity* entity, bool active, Entity* creator, bool hasLifetime, float lifetime, bool showAfterUses, bool limitedUses, int uses, float damage, bool damagesPlayers, bool damagesEnemies, bool damagesObjects, bool lodges)
{
	this->ID = damageComponentID;
	this->entity = entity;
	this->active = active;

	this->creator = creator;

	this->hasLifetime = hasLifetime;
	this->lifetime = lifetime;
	this->showAfterUses = showAfterUses;
	this->limitedUses = limitedUses;
	this->uses = uses;
	this->damage = damage;

	this->damagesPlayers = damagesPlayers;
	this->damagesEnemies = damagesEnemies;
	this->damagesObjects = damagesObjects;

	this->lodges = lodges;
	this->lodged = false;
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

#pragma region AI Component

AIComponent::AIComponent(Entity* entity, bool active, bool proc, float procRange, float chaseRange, float movementSpeed, float projectileSpeed, float attackRate, AIType aiType)
{
	this->ID = aiComponentID;
	this->entity = entity;
	this->active = active;

	this->proc = proc;
	this->procRange = procRange;
	this->chaseRange = chaseRange;

	this->movementSpeed = movementSpeed;
	this->projectileSpeed = projectileSpeed;

	this->lastAttack = 0.0f;
	this->attackRate = attackRate;

	this->aiType = aiType;
}

#pragma endregion

#pragma region Blade Component

BladeComponent::BladeComponent(Entity* entity, bool active, float rushRange, float slowRange, float throwRange, float followSpeed, float projectileSpeed, ColliderComponent* platformCollider, Texture2D* corporealMap, Texture2D* incorporealMap, float minTargetSetDelay)
{
	this->ID = bladeComponentID;
	this->entity = entity;
	this->active = active;

	this->attacking = false;
	this->thrown = false;
	this->lodged = false;

	this->rushRange = rushRange;
	this->slowRange = slowRange;
	this->throwRange = throwRange;

	this->followSpeed = followSpeed;
	this->projectileSpeed = projectileSpeed;

	this->platformCollider = platformCollider;

	this->corporealMap = corporealMap;
	this->incorporealMap = incorporealMap;

	manualTarget = glm::vec2(0, 0);

	this->lastTargetSet = 0.0f;
	this->minTargetSetDelay = minTargetSetDelay;
}

#pragma endregion

#pragma region Image Component

ImageComponent::ImageComponent(Entity* entity, bool active, Anchor anchor, float x, float y)
{
	this->ID = imageComponentID;
	this->entity = entity;
	this->active = active;
	
	this->anchor = anchor;
	this->x = x;
	this->y = y;
}

#pragma endregion


#pragma endregion

#pragma region Systems

#pragma region Static Rendering System

void StaticRenderingSystem::Update(int activeScene, float deltaTime)
{
	std::sort(sprites.begin(), sprites.end(), [](StaticSpriteComponent* a, StaticSpriteComponent* b)
		{
			return a->pos->z < b->pos->z;
		});

	for (int i = 0; i < sprites.size(); i++)
	{
		StaticSpriteComponent* s = sprites[i];

		if (s->active && s->entity->Get_Scene() == activeScene ||
			s->active && s->entity->Get_Scene() == 0)
		{
			PositionComponent* pos = s->pos;

			if (pos->x + (s->width / 2.0f) > Game::main.leftX && pos->x - (s->width / 2.0f) < Game::main.rightX &&
				pos->y + (s->height / 2.0f) > Game::main.bottomY && pos->y - (s->height / 2.0f) < Game::main.topY &&
				pos->z < Game::main.camZ)
			{
				Game::main.renderer->prepareQuad(pos, s->width, s->height, s->sprite->width, s->sprite->height, s->scale, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), s->sprite->ID, s->map->ID, s->tiled, s->flipped);
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

void PhysicsSystem::Update(int activeScene, float deltaTime)
{
	for (int i = 0; i < phys.size(); i++)
	{
		PhysicsComponent* p = phys[i];

		if (p->active && p->entity->Get_Scene() == activeScene ||
			p->active && p->entity->Get_Scene() == 0)
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

						if (!move->climbing && !col->onPlatform)
						{
							p->velocityY -= p->gravityMod * deltaTime;
						}
						else if (move->climbing)
						{
							if (p->velocityY > 0)
							{
								p->velocityY -= (p->drag / 4.0f) * deltaTime;

								if (p->velocityY < 0)
								{
									p->velocityY = 0;
								}
							}
							else if (p->velocityY < 0)
							{
								p->velocityY += (p->drag / 4.0f) * deltaTime;

								if (p->velocityY > 0)
								{
									p->velocityY = 0;
								}
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

						if (p->velocityX < 0)
						{
							p->velocityX = 0;
						}
					}
					else if (p->velocityX < 0 && col->onPlatform)
					{
						p->velocityX += p->drag * deltaTime;\

						if (p->velocityX > 0)
						{
							p->velocityX = 0;
						}
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
					if (p->velocityY > 0)
					{
						p->velocityY -= p->drag * deltaTime;
					}
					else if (p->velocityY < 0)
					{
						p->velocityY += p->drag * deltaTime;
					}

					if (p->rotVelocity > 0)
					{
						p->rotVelocity -= p->drag * deltaTime;
					}
					else if (p->rotVelocity < 0)
					{
						p->rotVelocity += p->drag * deltaTime;
					}

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

#pragma region Position System

void PositionSystem::Update(int activeScene, float deltaTime)
{
	for (int i = 0; i < pos.size(); i++)
	{
		PositionComponent* p = pos[i];
		PhysicsComponent* phys = (PhysicsComponent*)p->entity->componentIDMap[physicsComponentID];

		if (p->active && phys != nullptr && p->entity->Get_Scene() == activeScene ||
			p->active && phys != nullptr && p->entity->Get_Scene() == 0)
		{
			p->x += phys->velocityX * deltaTime;
			p->y += phys->velocityY * deltaTime;
			p->rotation += phys->rotVelocity * deltaTime;
		}
	}
}

void PositionSystem::AddComponent(Component* component)
{
	pos.push_back((PositionComponent*)component);
}

void PositionSystem::PurgeEntity(Entity* e)
{
	for (int i = 0; i < pos.size(); i++)
	{
		if (pos[i]->entity == e)
		{
			PositionComponent* s = pos[i];
			pos.erase(std::remove(pos.begin(), pos.end(), s), pos.end());
			delete s;
		}
	}
}

#pragma endregion

#pragma region Collider System

void ColliderSystem::Update(int activeScene, float deltaTime)
{
	for (int i = 0; i < colls.size(); i++)
	{
		ColliderComponent* cA = colls[i];

		if (cA->active && cA->entity->Get_Scene() == activeScene ||
			cA->active && cA->entity->Get_Scene() == 0)
		{
			cA->onPlatform = false;
			cA->collidedLastTick = false;

			PositionComponent* posA = cA->pos;
			PhysicsComponent* physA = (PhysicsComponent*)cA->entity->componentIDMap[physicsComponentID];

			/*Texture2D* t = Game::main.textureMap["blank"];
			Texture2D* tMap = Game::main.textureMap["base_map"];
			Game::main.renderer->prepareQuad(glm::vec2(posA->x + cA->offsetX, posA->y + cA->offsetY), cA->width, cA->height, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), t->ID, tMap->ID);*/
			
			std::vector<std::pair<Collision*, float>> z;

			for (int j = 0; j < colls.size(); j++)
			{
				ColliderComponent* cB = colls[j];

				if (cB->active && cB->entity->Get_ID() != cA->entity->Get_ID())
				{
					PositionComponent* posB = (PositionComponent*)cB->entity->componentIDMap[positionComponentID];
					PhysicsComponent* physB = (PhysicsComponent*)cB->entity->componentIDMap[physicsComponentID];

					float combVel = glm::length2(glm::vec2(physA->velocityX + physB->velocityX, physA->velocityY + physB->velocityY));
					float combSize = glm::length2(glm::vec2((cA->width + cB->width) / 2.0f, (cA->height + cB->height) / 2.0f));
					float dist = glm::length2(glm::vec2(posA->x + cA->offsetX, posA->y + cA->offsetY) - glm::vec2(posB->x + cB->offsetX, posB->y + cB->offsetY));

					if (!cA->platform && dist <= combVel + combSize)
					{
						glm::vec2 contactPoint, contactNormal;
						float time;

						Collision* c = DynamicArbitraryRectangleCollision(cA, posA, physA, cB, posB, physB, deltaTime);

						if (c != nullptr)
						{
							cA->collidedLastTick = true;
							cB->collidedLastTick = true;

							z.push_back(std::pair(c, c->time));
						}
					}
				}
			}

			// Sort the collisions for distance.
			std::sort(z.begin(), z.end(), [](const std::pair<Collision*, float>& a, const std::pair<Collision*, float>& b)
				{
					return a.second < b.second;
				});

			// Resolve all the collisions we just made.
			for (auto j : z)
			{
				ColliderComponent* cB = j.first->colB;
				PositionComponent* posB = (PositionComponent*)cB->entity->componentIDMap[positionComponentID];
				PhysicsComponent* physB = (PhysicsComponent*)cB->entity->componentIDMap[physicsComponentID];

				Collision* c = nullptr;
				c = DynamicArbitraryRectangleCollision(cA, posA, physA, cB, posB, physB, deltaTime);

				if (c != nullptr)
				{
					if (c->resolve && !cB->onewayPlatform ||
						c->resolve && cB->platform && cB->onewayPlatform && c->contactNormal.y == 1 && !cA->ignoreOnewayPlatforms)
					{
						glm::vec2 vMod = c->contactNormal * glm::vec2(abs(physA->velocityX), abs(physA->velocityY)) * (1.0f - c->time);

						glm::vec2 velAdd = glm::vec2(physA->velocityX, physA->velocityY) + vMod;
						physA->velocityX = velAdd.x;
						physA->velocityY = velAdd.y;
					}

					MovementComponent* moveA = (MovementComponent*)cA->entity->componentIDMap[movementComponentID];
					if (cB->platform && c->contactNormal.y == 1)
					{
						cA->onPlatform = true;
					}
					
					if (moveA != nullptr && cB->platform && c->contactNormal.x != 0 && physA->velocityY > physA->velocityX)
					{
						if (!moveA->wallRunning)
						{
							moveA->wallRunning = true;
							moveA->maxWallRun = cB->pos->y + (cB->height / 2.0f);
							// physA->velocityY += physA->velocityX * 0.5f;
							physA->velocityX = 0;
						}
					}

					if (cB->climbable && c->contactNormal.x != 0 && moveA != nullptr)
					{

						if (moveA->canClimb && moveA->shouldClimb)
						{
							if (!moveA->climbing)
							{
								// If you just started climbing, stop all other velocity.
								physA->velocityX = 0;
								physA->velocityY = 0;

								moveA->maxClimbHeight = cA->pos->y;
								moveA->minClimbHeight = cB->pos->y - (cB->height / 2.0f);
							}

							moveA->climbing = true;
						}
					}

					if (cA->trigger && cA->doesDamage)
					{
						DamageComponent* aDamage = (DamageComponent*)cA->entity->componentIDMap[damageComponentID];

						if (aDamage->creator != cB->entity)
						{
							if (aDamage->lodges)
							{
								ParticleEngine::main.AddParticles(5, physA->pos->x, physA->pos->y, Element::dust, rand() % 10 + 1);
								aDamage->lodged = true;

								cA->active = false;

								physA->velocityX = 0.0f;
								physA->velocityY = 0.0f;
								physA->gravityMod = 0.0f;
							}

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

								if (!aDamage->showAfterUses)
								{
									ECS::main.AddDeadEntity(aDamage->entity);
								}
							}

							aDamage->lifetime -= deltaTime;
						}
					}
					if (cB->trigger && cB->doesDamage)
					{
						DamageComponent* bDamage = (DamageComponent*)cB->entity->componentIDMap[damageComponentID];

						if (bDamage->creator != cA->entity)
						{
							if (bDamage->lodges)
							{
								bDamage->lodged = true;

								ParticleEngine::main.AddParticles(5, physB->pos->x, physB->pos->y, Element::dust, rand() % 10 + 1);
								cB->active = false;

								physB->velocityX = 0.0f;
								physB->velocityY = 0.0f;
								physB->gravityMod = 0.0f;
							}

							if (bDamage->creator != cA->entity)
							{
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

									if (!bDamage->showAfterUses)
									{
										ECS::main.AddDeadEntity(bDamage->entity);
									}
								}

								bDamage->lifetime -= deltaTime;
							}
						}	// Bin gar keine Russin, stamm’ aus Litauen, echt deutsch.
					}	// And when we were children, staying at the arch-duke's,
				}	// My cousin's, he took me out on a sled,
			}	// And I was frightened. He said, Marie,
			 // Marie, hold on tight. And down we went.
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

	// Game::main.renderer->prepareQuad(aTopRight, aBottomRight, aBottomLeft, aTopLeft, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), Game::main.textureMap["blank"]->ID);
	return false;
}

bool ColliderSystem::TestCollision(ColliderComponent* colA, PositionComponent* posA, PhysicsComponent* physA, ColliderComponent* colB, PositionComponent* posB, PhysicsComponent* physB, float deltaTime)
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

	float dT = (int)(deltaTime * 100 + 0.5);
	dT = max(0.1f, 5.0f * ((float)dT / 100));
	// std::cout << std::to_string(deltaTime) + "/" + std::to_string(dT) + "\n";

	if (dT != 0)
	{
		float it = 1.0f;
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

		// Texture2D* t = Game::main.textureMap["test"];
		// Game::main.renderer->prepareQuad(posA, abs(aTopRight.x - aTopLeft.y), abs(aTopRight.y - aBottomRight.y), t->width, t->height, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), Game::main.textureMap["test"]->ID, false);
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

							return true;
						}
					}
				}
			}
		}
	}

	return false;
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

			// Texture2D* t = Game::main.textureMap["test"];
			// Game::main.renderer->prepareQuad(posA, abs(aTopRight.x - aTopLeft.y), abs(aTopRight.y - aBottomRight.y), t->width, t->height, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), Game::main.textureMap["test"]->ID, false);
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
							glm::vec2 displacementVector = Normalize(displacement);

							if (!posA->stat)
							{
								std::cout << "A.\n";

								if (physA->velocityX > 0)
								{
									physA->velocityX += displacementVector.x * physA->velocityX;
								}
								physA->velocityX += displacementVector.x * physA->velocityX;
								physA->velocityY += displacementVector.y * physA->velocityY;

								posA->x -= displacement.x;
								posA->y -= displacement.y;
							}
							else if (!posB->stat && !colA->platform)
							{
								std::cout << "B.\n";

								physB->velocityX -= displacementVector.x * physB->velocityX;
								physB->velocityY -= displacementVector.y * physB->velocityY;

								posB->x += displacement.x;
								posB->y += displacement.y;
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
							glm::vec2 displacementVector = Normalize(displacement);

							if (!posA->stat && !colB->platform)
							{
								std::cout << "C.\n";

								physA->velocityX += displacementVector.x * physA->velocityX;
								physA->velocityY += displacementVector.y * physA->velocityY;

								posA->x += displacement.x;
								posA->y += displacement.y;
							}
							else if (!posB->stat)
							{
								std::cout << "D.\n";

								physB->velocityX -= displacementVector.x * physB->velocityX;
								physB->velocityY -= displacementVector.y * physB->velocityY;

								posB->x -= displacement.x;
								posB->y -= displacement.y;
							}
						}
					}
				}
			}
		}
	}

	return collided;
}

Collision* ColliderSystem::ArbitraryRectangleCollision(ColliderComponent* colA, PositionComponent* posA, PhysicsComponent* physA, ColliderComponent* colB, PositionComponent* posB, PhysicsComponent* physB, float deltaTime)
{
	glm::vec2 rayOrigin = glm::vec2(posA->x, posA->y);
	glm::vec2 rayDir = glm::vec2(physA->velocityX, physA->velocityY);

	glm::vec2 rectPos = glm::vec2(posB->x + colB->offsetX, posB->y + colB->offsetY);
	float rectWidth = colB->width + colA->width;
	float rectHeight = colB->height + colA->height;

	glm::vec2 contactPoint = glm::vec2(0,0);
	glm::vec2 contactNormal = glm::vec2(0, 0);
	float time = 0.0f;

	if (RayOverlapRect(rayOrigin, rayDir * deltaTime, rectPos, rectWidth, rectHeight, contactPoint, contactNormal, time))
	{
		if (time < 1.0f && time >= 0.0f)
		{
			return new Collision(contactPoint, contactNormal, time, colB, (!colA->trigger && !colB->trigger));
		}
	}

	return nullptr;
}

Collision* ColliderSystem::DynamicArbitraryRectangleCollision(ColliderComponent* colA, PositionComponent* posA, PhysicsComponent* physA, ColliderComponent* colB, PositionComponent* posB, PhysicsComponent* physB, float deltaTime)
{
	glm::vec2 rayOrigin = glm::vec2(posA->x + colA->offsetX, posA->y + colA->offsetY);
	glm::vec2 rayDir =  glm::vec2(physA->velocityX, physA->velocityY) - glm::vec2(physB->velocityX, physB->velocityY);

	glm::vec2 rectPos = glm::vec2(posB->x + colB->offsetX, posB->y + colB->offsetY);
	float rectWidth = colB->width + colA->width;
	float rectHeight = colB->height + colA->height;

	glm::vec2 contactPoint = glm::vec2(0, 0);
	glm::vec2 contactNormal = glm::vec2(0, 0);
	float time = 0.0f;

	if (RayOverlapRect(rayOrigin, rayDir * deltaTime, rectPos, rectWidth, rectHeight, contactPoint, contactNormal, time))
	{
		if (time < 1.0f && time >= 0.0f)
		{
			return new Collision(contactPoint, contactNormal, time, colB, (!colA->trigger && !colB->trigger));
		}
	}

	return nullptr;
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

void InputSystem::Update(int activeScene, float deltaTime)
{
	for (int i = 0; i < move.size(); i++)
	{
		InputComponent* m = move[i];

		if (m->active && m->entity->Get_Scene() == activeScene ||
			m->active && m->entity->Get_Scene() == 0)
		{
			MovementComponent* move = (MovementComponent*)m->entity->componentIDMap[movementComponentID];
			PhysicsComponent* phys = (PhysicsComponent*)m->entity->componentIDMap[physicsComponentID];
			ColliderComponent* col = (ColliderComponent*)m->entity->componentIDMap[colliderComponentID];
			HealthComponent* health = (HealthComponent*)m->entity->componentIDMap[healthComponentID];
			AnimationControllerComponent* animator = (AnimationControllerComponent*)m->entity->componentIDMap[animationComponentID];

			glm::vec3 playerPos = glm::vec3(phys->pos->x, phys->pos->y, phys->pos->z);

			m->lastTick += deltaTime;

			if (!health->dead)
			{
				float aBot = phys->pos->y - (col->height / 2.0f) + col->offsetY;
				float aTop = phys->pos->y + (col->height / 2.0f) + col->offsetY;

				if (move->wallRunning && phys->velocityY <= 0.0f || move->wallRunning && playerPos.y >= move->maxWallRun)
				{
					move->wallRunning = false;
					phys->gravityMod = phys->baseGravityMod;
				}
				else if (move->wallRunning)
				{
					phys->gravityMod = phys->baseGravityMod * 0.6f;
				}

				if (move->climbing && !move->shouldClimb ||
					move->climbing && aBot > move->maxClimbHeight ||
					move->climbing && aTop < move->minClimbHeight)
				{
					move->climbing = false;
				}

				if (col->onPlatform)
				{
					move->jumping = false;
					m->jumps = 0;
					m->coyoteTime = 0.0f;
				}
				else if (move->climbing)
				{
					move->jumping = false;
				}

				BladeComponent* blade = (BladeComponent*)m->moonlightBlade->componentIDMap[bladeComponentID];
				PositionComponent* bladePosComp = (PositionComponent*)m->moonlightBlade->componentIDMap[positionComponentID];
				PhysicsComponent* bladePhys = (PhysicsComponent*)blade->entity->componentIDMap[physicsComponentID];
				BladeComponent* bladeComponent = (BladeComponent*)blade->entity->componentIDMap[bladeComponentID];
				glm::vec2 bladePos = glm::vec2(bladePosComp->x, bladePosComp->y);

				blade->lastTargetSet += deltaTime;
				if (glfwGetMouseButton(Game::main.window, GLFW_MOUSE_BUTTON_3) == GLFW_PRESS && blade->manualTarget.x == 0 && blade->manualTarget.y == 0 && blade->lastTargetSet > blade->minTargetSetDelay)
				{
					blade->lastTargetSet = 0.0f;
					blade->manualTarget = glm::vec2(Game::main.mouseX, Game::main.mouseY);
				}
				else if (glfwGetMouseButton(Game::main.window, GLFW_MOUSE_BUTTON_3) == GLFW_PRESS && blade->lastTargetSet > blade->minTargetSetDelay)
				{
					blade->lastTargetSet = 0.0f;
					blade->manualTarget = glm::vec2(0, 0);
				}

				float bladeDist = glm::length2(bladePos - glm::vec2(playerPos.x, playerPos.y));

				if (glfwGetMouseButton(Game::main.window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS && m->lastProjectile >= m->projectileDelay && !blade->thrown && !bladeComponent->attacking)
				{
					glm::vec2 projVel = Normalize(glm::vec2(Game::main.mouseX - bladePos.x, Game::main.mouseY - bladePos.y)) * bladeComponent->projectileSpeed;
					m->lastProjectile = 0.0f;
					blade->thrown = true;

					bladePhys->velocityX = projVel.x;
					bladePhys->velocityY = projVel.y;
				}
				else if (glfwGetMouseButton(Game::main.window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS && m->lastProjectile >= m->projectileDelay && blade->thrown && !bladeComponent->attacking)
				{
					bladePhys->velocityX = 0.0f;
					bladePhys->velocityY = 0.0f;
					bladePhys->gravityMod = bladePhys->baseGravityMod;

					m->lastProjectile = 0.0f;

					DamageComponent* bladeDamage = (DamageComponent*)blade->entity->componentIDMap[damageComponentID];

					bladeDamage->lodged = false;
					blade->lodged = false;
					blade->thrown = false;

					blade->manualTarget = glm::vec2(0, 0);
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

				StaticSpriteComponent* bladeSprite = (StaticSpriteComponent*)blade->entity->componentIDMap[spriteComponentID];
				if (glfwGetMouseButton(Game::main.window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS && move->lastAttack > move->minAttackDelay && move->lastFlurry > move->flurryDelay && move->attackNumber < move->maxFlurry && !blade->thrown && bladeDist <= blade->throwRange)
				{
					bladeSprite->active = false;
					bladeComponent->attacking = true;

					move->isAttacking = true;
					move->attackNumber++;

					if (move->attackNumber >= move->maxFlurry)
					{
						move->lastFlurry = 0.0f;
					}

					move->climbing = false;
					move->shouldClimb = false;

					move->lastAttack = 0.0f;

					glm::vec2 projVel = Normalize(glm::vec2(Game::main.mouseX - playerPos.x, Game::main.mouseY - playerPos.y)) * move->slashSpeed;

					if (playerPos.x > Game::main.leftX && playerPos.x < Game::main.rightX &&
						playerPos.y > Game::main.bottomY && playerPos.y < Game::main.topY)
					{
						Texture2D* sMap = Game::main.textureMap["moonlightSlashMap"];
						Animation2D* anim;

						if (projVel.x > 0)
						{
							phys->velocityX += move->attackThrust.x;

						}
						else
						{
							phys->velocityX -= move->attackThrust.x;
						}

						phys->velocityY += move->attackThrust.y;

						if (move->attackNumber % 2 == 0)
						{
							anim = Game::main.animationMap["slashDown"];
						}
						else
						{
							anim = Game::main.animationMap["slashUp"];
						}

						float t = anim->speed * (anim->columns * anim->rows);

						m->lastProjectile = 0.0f;

						Entity* projectile = ECS::main.CreateEntity(0, "Slash");

						ECS::main.RegisterComponent(new PositionComponent(projectile, true, false, playerPos.x, playerPos.y, playerPos.z, 0.0f), projectile);
						ECS::main.RegisterComponent(new PhysicsComponent(projectile, true, (PositionComponent*)projectile->componentIDMap[positionComponentID], phys->velocityX + projVel.x, phys->velocityY + projVel.y, 0.0f, 0.0f, 0.0f), projectile);
						ECS::main.RegisterComponent(new ColliderComponent(projectile, true, (PositionComponent*)projectile->componentIDMap[positionComponentID], false, false, true, false, true, false, true, EntityClass::object, 1.0f, 0.0f, 0.0f, 5.0f, 5.0f, 0.0f, 0.0f), projectile);
						ECS::main.RegisterComponent(new DamageComponent(projectile, true, move->entity, true, t, true, true, 1, 20.0f, false, true, true, false), projectile);
						ECS::main.RegisterComponent(new AnimationComponent(projectile, true, (PositionComponent*)projectile->componentIDMap[positionComponentID], anim, "default", sMap, 1.0f), projectile);

						PhysicsComponent* p = (PhysicsComponent*)projectile->componentIDMap[physicsComponentID];
						if (p->velocityX < 0)
						{
							AnimationComponent* a = (AnimationComponent*)projectile->componentIDMap[animationComponentID];

							a->flipped = true;
						}
					}
				}
				else
				{
					move->lastFlurry += deltaTime;
					move->lastAttack += deltaTime;
				}

				if (move->lastAttack > move->maxAttackDelay)
				{
					bladeSprite->active = true;
					bladeComponent->attacking = false;
					move->isAttacking = false;

					if (col->onPlatform)
					{
						move->attackNumber = 0;
					}
				}

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
					|| glfwGetKey(Game::main.window, GLFW_KEY_SPACE) == GLFW_PRESS && move->canMove && m->releasedJump && !col->onPlatform && m->maxJumps > 1 && m->jumps < m->maxJumps
					|| glfwGetKey(Game::main.window, GLFW_KEY_SPACE) == GLFW_PRESS && move->canMove && m->releasedJump && move->wallRunning)
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

					AnimationComponent* anComp = (AnimationComponent*)m->entity->componentIDMap[animationComponentID];

					if (move->wallRunning && anComp->flipped)
					{
						phys->velocityX += 250 * move->maxJumpHeight;
					}
					else if (move->wallRunning)
					{
						phys->velocityX -= 250 * move->maxJumpHeight;
					}

					move->wallRunning = false;
				}

				if (!m->releasedJump && move->jumping && phys->velocityY > 0)
				{
					phys->gravityMod = phys->baseGravityMod * 0.6f;
				}
				else
				{
					phys->gravityMod = phys->baseGravityMod;
				}


				if (glfwGetKey(Game::main.window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
				{
					if (col->height == col->baseHeight && col->offsetY == col->baseOffsetY)
					{
						phys->pos->y += 5.0f;
					}

					move->crouching = true;
					col->height = col->baseHeight * 0.75f;
					col->offsetY = col->baseOffsetY - 5.0f;

					col->ignoreOnewayPlatforms = true;
				}
				else
				{
					if (col->height == col->baseHeight * 0.75f && col->offsetY == col->baseOffsetY - 5.0f)
					{
						phys->pos->y += 5.0f;
					}

					move->crouching = false;
					col->height = col->baseHeight;
					col->offsetY = col->baseOffsetY;

					col->ignoreOnewayPlatforms = false;
				}

				if (move->crouching && abs(phys->velocityX) - 10.0f > move->maxSpeed * move->crouchMod && col->onPlatform)
				{
					ParticleEngine::main.AddParticles(1, phys->pos->x, phys->pos->y - 30.0f, Element::dust, rand() % 10 + 1);

					phys->drag = phys->baseDrag * 0.1f;
				}
				else
				{
					phys->drag = phys->baseDrag;
				}

				float mod = 1.0f;

				if (move->crouching)
				{
					mod = move->crouchMod;
				}
				
				if (move->climbing)
				{
					mod = move->climbMod;
				}
				else if (move->jumping || !col->onPlatform && abs(phys->velocityY) > 100.0f)
				{
					mod = move->airControl;
				}

				if (glfwGetKey(Game::main.window, GLFW_KEY_S) == GLFW_PRESS && move->canMove && move->climbing)
				{
					ParticleEngine::main.AddParticles(1, phys->pos->x, phys->pos->y + 20.0f, Element::dust, rand() % 10 + 1);
					if (phys->velocityY > -move->maxSpeed * mod)
					{
						phys->velocityY -= move->acceleration * deltaTime * mod;
					}
				}

				if (glfwGetKey(Game::main.window, GLFW_KEY_D) == GLFW_PRESS && move->canMove && !move->climbing)
				{
					if (phys->velocityX < move->maxSpeed * mod)
					{
						if (abs(phys->velocityX) < 0.5f && col->onPlatform)
						{
							ParticleEngine::main.AddParticles(10, phys->pos->x, phys->pos->y - 30.0f, Element::dust, rand() % 10 + 1);
						}

						phys->velocityX += move->acceleration * deltaTime * mod;
					}
				}
				else if (glfwGetKey(Game::main.window, GLFW_KEY_A) == GLFW_PRESS && move->canMove && !move->climbing)
				{
					if (phys->velocityX > -move->maxSpeed * mod)
					{
						if (abs(phys->velocityX) < 0.5f && col->onPlatform)
						{
							ParticleEngine::main.AddParticles(10, phys->pos->x, phys->pos->y - 30.0f, Element::dust, rand() % 10 + 1);
						}

						phys->velocityX -= move->acceleration * deltaTime * mod;
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
			if (projPos.x + (s->width / 2.0f) > Game::main.leftX && projPos.x - (s->width / 2.0f) < Game::main.rightX &&
				projPos.y + (s->height / 2.0f) > Game::main.bottomY && projPos.y - (s->height / 2.0f) < Game::main.topY)
			{

				Game::main.renderer->prepareQuad(projPos, s->width / 2.0f, s->height / 2.0f, 1.0f, glm::vec4(1.0f, 1.0f, 1.0f, 0.5f), s->ID, Game::main.textureMap["blank"]->ID);
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

void CameraFollowSystem::Update(int activeScene, float deltaTime)
{
	for (int i = 0; i < folls.size(); i++)
	{
		CameraFollowComponent* f = folls[i];

		if (f->active && f->entity->Get_Scene() == activeScene ||
			f->active && f->entity->Get_Scene() == 0)
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

void AnimationControllerSystem::Update(int activeScene, float deltaTime)
{
	for (int i = 0; i < controllers.size(); i++)
	{
		AnimationControllerComponent* c = controllers[i];

		if (c->active && c->entity->Get_Scene() == activeScene ||
			c->active && c->entity->Get_Scene() == 0)
		{

			if (c->subID == lilyAnimControllerSubID)
			{
				// I'm thinking what we'll do is just hard code the various animation conditions
				// into the animation controller; this will serve as the animation controller
				// for the player and other humans.

				// We are going to assume that any entity with an animation controller component
				// also has a physics and collider component.
				// I think I can safely assume this because characters should basically always
				// have the same set of components, aside from the player's.

				PlayerAnimationControllerComponent* d = (PlayerAnimationControllerComponent*)c;
				PhysicsComponent* p = (PhysicsComponent*)d->entity->componentIDMap[physicsComponentID];
				ColliderComponent* col = (ColliderComponent*)d->entity->componentIDMap[colliderComponentID];
				MovementComponent* move = (MovementComponent*)d->entity->componentIDMap[movementComponentID];
				HealthComponent* health = (HealthComponent*)d->entity->componentIDMap[healthComponentID];

				if (!health->dead)
				{
					if (p->velocityX < -100.0f || move->isAttacking && Game::main.mouseX < p->pos->x)
					{
						c->animator->flipped = true;
					}
					else if (p->velocityX > 100.0f || move->isAttacking && Game::main.mouseX >= p->pos->x)
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

					// Add climbing, sliding, crouching, and crouch-walking.

					if (move->isAttacking && move->attackNumber % 2 == 0)
					{
						if (c->animator->activeAnimation != "slashTwo")
						{
							c->animator->SetAnimation("slashTwo");
						}
					}
					else if (move->isAttacking && move->attackNumber % 2 != 0)
					{
						if (c->animator->activeAnimation != "slashOne")
						{
							c->animator->SetAnimation("slashOne");
						}
					}
					else if (abs(p->velocityY) > 200.0f && !col->onPlatform && !move->climbing && !move->wallRunning)
					{
						if (c->animator->activeAnimation != "jumpUp" && p->velocityY > 0)
						{
							c->animator->SetAnimation("jumpUp");
						}
						else if (c->animator->activeAnimation != "jumpDown" && p->velocityY < 0)
						{
							c->animator->SetAnimation("jumpDown");
						}
					}
					else if (move->wallRunning && abs(p->velocityX) < 100.0f && c->animator->activeAnimation != "wallRun")
					{
						c->animator->SetAnimation("wallRun");
					}
					else if (p->velocityY <= 0.0f && move->climbing && c->animator->activeAnimation != "slideDown")
					{
						c->animator->SetAnimation("slideDown");
					}
					else if (abs(p->velocityX) - 10.0f > move->maxSpeed * move->crouchMod && move->crouching && col->onPlatform && move->canMove && c->animator->activeAnimation != "slide")
					{
						c->animator->SetAnimation("slide");
					}
					else if (abs(p->velocityX) - 10.0f < move->maxSpeed * move->crouchMod && abs(p->velocityX) > 25.0f && move->crouching && col->onPlatform && move->canMove && c->animator->activeAnimation != "crouchWalk")
					{
						c->animator->SetAnimation("crouchWalk");
					}
					else if (abs(p->velocityX) < 25.0f && move->crouching && col->onPlatform && move->canMove && c->animator->activeAnimation != "crouch")
					{
						c->animator->SetAnimation("crouch");
					}
					else if (abs(p->velocityX) > 100.0f && !move->crouching && col->onPlatform && move->canMove && c->animator->activeAnimation != "walk")
					{
						c->animator->SetAnimation("walk");
					}
					else if (abs(p->velocityX) < 100.0f && !move->crouching && col->onPlatform && move->canMove && c->animator->activeAnimation != "idle")
					{
						c->animator->SetAnimation("idle");
					}
				}
				else if (c->animator->activeAnimation != "dead")
				{
					c->animator->SetAnimation("dead");
				}
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

void AnimationSystem::Update(int activeScene, float deltaTime)
{
	std::sort(anims.begin(), anims.end(), [](AnimationComponent* a, AnimationComponent* b)
		{
			return a->pos->z < b->pos->z;
		});

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

		if (a->active && a->entity->Get_Scene() == activeScene ||
			a->active && a->entity->Get_Scene() == 0)
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

			if (pos->x + ((activeAnimation->width / activeAnimation->columns) / 2.0f) > Game::main.leftX && pos->x - ((activeAnimation->width / activeAnimation->columns) / 2.0f) < Game::main.rightX &&
				pos->y + ((activeAnimation->height / activeAnimation->rows) / 2.0f) > Game::main.bottomY && pos->y - ((activeAnimation->height / activeAnimation->rows) / 2.0f) < Game::main.topY &&
				pos->z < Game::main.camZ)
			{
				// std::cout << std::to_string(activeAnimation->width) + "/" + std::to_string(activeAnimation->height) + "\n";
				Game::main.renderer->prepareQuad(pos, activeAnimation->width, activeAnimation->height, a->scale, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), activeAnimation->ID, a->map->ID, cellX, cellY, activeAnimation->columns, activeAnimation->rows, a->flipped);
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

void HealthSystem::Update(int activeScene, float deltaTime)
{
	for (int i = 0; i < healths.size(); i++)
	{
		HealthComponent* h = healths[i];

		if (h->active && h->entity->Get_Scene() == activeScene ||
			h->active && h->entity->Get_Scene() == 0)
		{
			if (h->health <= 0.0f)
			{
				// You're dead.
				h->dead = true;

				h->active = false;
				ECS::main.AddDeadEntity(h->entity);
			}
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

#pragma region Particle System

void ParticleSystem::Update(int activeScene, float deltaTime)
{
	float screenLeft = (Game::main.camX - (Game::main.windowWidth * Game::main.zoom / 1.0f));
	float screenRight = (Game::main.camX + (Game::main.windowWidth * Game::main.zoom / 1.0f));
	float screenBottom = (Game::main.camY - (Game::main.windowHeight * Game::main.zoom / 1.0f));
	float screenTop = (Game::main.camY + (Game::main.windowHeight * Game::main.zoom / 1.0f));
	float screenElev = Game::main.camZ;

	for (int i = 0; i < particles.size(); i++)
	{
		ParticleComponent* p = particles[i];

		if (p->active && p->entity->Get_Scene() == activeScene ||
			p->active && p->entity->Get_Scene() == 0)
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

#pragma region Damage System

void DamageSystem::Update(int activeScene, float deltaTime)
{
	for (int i = 0; i < damagers.size(); i++)
	{
		DamageComponent* d = damagers[i];

		if (d->active && d->entity->Get_Scene() == activeScene ||
			d->active && d->entity->Get_Scene() == 0)
		{
			if (d->hasLifetime && d->lifetime < 0.0f)
			{
				ECS::main.AddDeadEntity(d->entity);
			}
			else if (d->hasLifetime)
			{
				d->lifetime -= deltaTime;
			}
		}
	}
}

void DamageSystem::AddComponent(Component* component)
{
	damagers.push_back((DamageComponent*)component);
}

void DamageSystem::PurgeEntity(Entity* e)
{
	for (int i = 0; i < damagers.size(); i++)
	{
		if (damagers[i]->entity == e)
		{
			DamageComponent* s = damagers[i];
			damagers.erase(std::remove(damagers.begin(), damagers.end(), s), damagers.end());
			delete s;
		}
	}
}

#pragma endregion

#pragma region AI System

void AISystem::Update(int activeScene, float deltaTime)
{
	for (int i = 0; i < ai.size(); i++)
	{
		AIComponent* a = ai[i];

		if (a->active && a->entity->Get_Scene() == activeScene ||
			a->active && a->entity->Get_Scene() == 0)
		{
			Entity* player = ECS::main.player;
			
			if (a->aiType == AIType::aerial)
			{
				PositionComponent* posA = (PositionComponent*)a->entity->componentIDMap[positionComponentID];
				PositionComponent* posB = (PositionComponent*)player->componentIDMap[positionComponentID];

				glm::vec2 aCoor = glm::vec2(posA->x, posA->y);
				glm::vec2 lookRay = aCoor - glm::vec2(posB->x, posB->y);

				float dist = glm::length2(lookRay);

				// std::cout << std::to_string(dist) + "\n";
				if (dist <= a->procRange || dist <= a->chaseRange && a->proc)
				{
					bool blocked = false;
					if (!a->proc)
					{
						int myID = a->entity->Get_ID();
						int playerID = player->Get_ID();

						for (int en = 0; en < ECS::main.entities.size(); en++)
						{
							PositionComponent* posC = (PositionComponent*)ECS::main.entities[en]->componentIDMap[positionComponentID];
							ColliderComponent* colC = (ColliderComponent*)ECS::main.entities[en]->componentIDMap[colliderComponentID];

							if (colC != nullptr)
							{
								int colID = colC->entity->Get_ID();

								if (colC->active && !colC->trigger && colID != playerID && colID != myID)
								{
									glm::vec2 cp = glm::vec2(0, 0);
									glm::vec2 cn = glm::vec2(0, 0);
									float t = 0.0f;

									if (RayOverlapRect(aCoor, lookRay, glm::vec2(posC->x + colC->offsetX, posC->y + colC->offsetY), colC->width, colC->height, cp, cn, t))
									{
										blocked = true;
										break;
									}
								}
							}
						}
					}

					if (!blocked || a->proc)
					{
						a->proc = true;

						if (a->lastAttack >= a->attackRate)
						{
							a->lastAttack = 0.0f;

							Texture2D* s = Game::main.textureMap["bullet"];
							Texture2D* sMap = Game::main.textureMap["aether_bullet"];

							Entity* projectile = ECS::main.CreateEntity(0, "Bullet");

							glm::vec2 vel = -Normalize(lookRay) * a->projectileSpeed;

							ECS::main.RegisterComponent(new PositionComponent(projectile, true, false, posA->x, posA->y, posA->z, 0.0f), projectile);
							ECS::main.RegisterComponent(new PhysicsComponent(projectile, true, (PositionComponent*)projectile->componentIDMap[positionComponentID], vel.x, vel.y, 0.0f, 0.0f, 0.0f), projectile);
							ECS::main.RegisterComponent(new ColliderComponent(projectile, true, (PositionComponent*)projectile->componentIDMap[positionComponentID], false, false, true, false, true, false, true, EntityClass::object, 1.0f, 0.0f, 0.0f, 5.0f, 5.0f, 0.0f, 0.0f), projectile);
							ECS::main.RegisterComponent(new DamageComponent(projectile, true, a->entity, true, 10.0f, false, true, 1, 10.0f, true, true, true, false), projectile);
							ECS::main.RegisterComponent(new StaticSpriteComponent(projectile, true, (PositionComponent*)projectile->componentIDMap[positionComponentID], s->width, s->height, 1.0f, s, sMap, false, false), projectile);
						}
						else
						{
							a->lastAttack += deltaTime;
						}
					}
				}
			}
		}
	}
}

void AISystem::AddComponent(Component* component)
{
	ai.push_back((AIComponent*)component);
}

void AISystem::PurgeEntity(Entity* e)
{
	for (int i = 0; i < ai.size(); i++)
	{
		if (ai[i]->entity == e)
		{
			AIComponent* s = ai[i];
			ai.erase(std::remove(ai.begin(), ai.end(), s), ai.end());
			delete s;
		}
	}
}

#pragma endregion

#pragma region Blade System

void BladeSystem::Update(int activeScene, float deltaTime)
{
	for (int i = 0; i < blades.size(); i++)
	{
		BladeComponent* b = blades[i];

		if (b->active && b->entity->Get_Scene() == activeScene ||
			b->active && b->entity->Get_Scene() == 0)
		{
			ColliderComponent* colA = (ColliderComponent*)b->entity->componentIDMap[colliderComponentID];
			DamageComponent* damA = (DamageComponent*)b->entity->componentIDMap[damageComponentID];
			PhysicsComponent* physA = (PhysicsComponent*)b->entity->componentIDMap[physicsComponentID];
			PositionComponent* posA = (PositionComponent*)b->entity->componentIDMap[positionComponentID];
			StaticSpriteComponent* sprite = (StaticSpriteComponent*)b->entity->componentIDMap[spriteComponentID];
			Entity* player = ECS::main.player;

			if (damA->lodged)
			{
				b->lodged = true;
			}

			if (!b->thrown)
			{
				sprite->map = b->incorporealMap;
				colA->active = false;
				damA->active = false;
				b->platformCollider->active = false;

				PositionComponent* posB = (PositionComponent*)player->componentIDMap[positionComponentID];
				PhysicsComponent* physB = (PhysicsComponent*)player->componentIDMap[physicsComponentID];
				ColliderComponent* colB = (ColliderComponent*)player->componentIDMap[colliderComponentID];
				MovementComponent* moveB = (MovementComponent*)player->componentIDMap[movementComponentID];

				glm::vec2 position = glm::vec2(posA->x, posA->y);
				glm::vec2 mouse = glm::vec2(Game::main.mouseX, Game::main.mouseY);
				glm::vec2 target;

				if (physB->velocityX >= 0 && !moveB->climbing || moveB->climbing && !sprite->flipped)
				{
					target = glm::vec2(posB->x - (colB->width), posB->y + (colB->height));
				}
				else
				{
					target = glm::vec2(posB->x + (colB->width), posB->y + (colB->height));
				}

				if (moveB->climbing)
				{
					target.y = posB->y - colB->height + colB->offsetY - 10.0f;
				}

				if (b->manualTarget.x != 0 && b->manualTarget.y != 0)
				{
					target = b->manualTarget;
				}

				// Look
				// I'll need to change this later if I want this to handle animations.
				float r = std::atan2(mouse.y - position.y, mouse.x - position.x) * (180 / M_PI);

				// std::cout << std::to_string(r) + "\n";

				if (r > 100 || r < -100)
				{
					sprite->flipped = true;
					r += 180;
				}
				else
				{
					sprite->flipped = false;
				}

				posA->rotation = r;
				// std::cout << std::to_string(r) + "\n";

				// Move
				float dist = glm::length2(position - target);
				if (dist > b->slowRange)
				{
					glm::vec2 vel = -Normalize(position - lerp(position, target, deltaTime));

					if (dist > b->rushRange)
					{
						posA->x += vel.x * (1.0f / b->followSpeed) * (200 * deltaTime);
						posA->y += vel.y * (1.0f / b->followSpeed) * (200 * deltaTime);
					}
					else
					{
						physA->velocityX += vel.x * (1.0f / b->followSpeed) * (100 * deltaTime);
						physA->velocityY += vel.y * (1.0f / b->followSpeed) * (100 * deltaTime);
					}
				}
			}
			else if (b->lodged)
			{

				colA->active = false;
				damA->active = false;

				physA->velocityX = 0;
				physA->velocityY = 0;
				physA->gravityMod = 0;
				// Act as a grappling hook?

				if (b->platformCollider->active == false && posA->rotation < 15.0f && posA->rotation > -15.0f ||
					b->platformCollider->active == false && posA->rotation > 345.0f)
				{
					sprite->map = b->corporealMap;
					PositionComponent* hiltPos = (PositionComponent*)b->platformCollider->entity->componentIDMap[positionComponentID];
					hiltPos->x = posA->x;
					hiltPos->y = posA->y;
					b->platformCollider->active = true;

					b->platformCollider->width = 35.0f;
					b->platformCollider->height = 5.0f;
					b->platformCollider->platform = true;
					b->platformCollider->onewayPlatform = true;

					if (sprite->flipped)
					{
						b->platformCollider->offsetX = 20.0f;
					}
					else
					{
						b->platformCollider->offsetX = -20.0f;
					}
				}
				else if (b->platformCollider->active == false && posA->rotation < -75.0f ||
						 b->platformCollider->active == false && sprite->flipped && posA->rotation > 75.0f)
				{
					sprite->map = b->corporealMap;
					PositionComponent* hiltPos = (PositionComponent*)b->platformCollider->entity->componentIDMap[positionComponentID];
					hiltPos->x = posA->x;
					hiltPos->y = posA->y;
					b->platformCollider->active = true;

					b->platformCollider->platform = true;
					b->platformCollider->onewayPlatform = false;
					b->platformCollider->width = 5.0f;
					b->platformCollider->height = 70.0f;

					b->platformCollider->offsetX = 0.0f;
					b->platformCollider->offsetY = 0.0f;
				}
			}
			else
			{
				// We are in flight.
				ParticleEngine::main.AddParticles(1, physA->pos->x, physA->pos->y, Element::aether, rand() % 10 + 1);
				b->platformCollider->active = false;
				colA->active = true;
				damA->active = true;
			}
		}
	}
}

void BladeSystem::AddComponent(Component* component)
{
	blades.push_back((BladeComponent*)component);
}

void BladeSystem::PurgeEntity(Entity* e)
{
	for (int i = 0; i < blades.size(); i++)
	{
		if (blades[i]->entity == e)
		{
			BladeComponent* s = blades[i];
			blades.erase(std::remove(blades.begin(), blades.end(), s), blades.end());
			delete s;
		}
	}
}

#pragma endregion

#pragma region Image System

void ImageSystem::Update(int activeScene, float deltaTime)
{
	for (int i = 0; i < images.size(); i++)
	{
		ImageComponent* img = images[i];

		if (img->active && img->entity->Get_Scene() == activeScene ||
			img->active && img->entity->Get_Scene() == 0)
		{
			PositionComponent* pos = (PositionComponent*)img->entity->componentIDMap[positionComponentID];
			StaticSpriteComponent* sprite = (StaticSpriteComponent*)img->entity->componentIDMap[spriteComponentID];

			glm::vec2 anchorPos;

			if (img->anchor == Anchor::topLeft)
			{
				anchorPos = glm::vec2(Game::main.leftX, Game::main.topY) - glm::vec2(-sprite->sprite->width, sprite->sprite->height);
			}
			else if (img->anchor == Anchor::topRight)
			{
				anchorPos = glm::vec2(Game::main.rightX, Game::main.topY) - glm::vec2(sprite->sprite->width, sprite->sprite->height);;
			}
			else if (img->anchor == Anchor::bottomLeft)
			{
				anchorPos = glm::vec2(Game::main.leftX, Game::main.bottomY) + glm::vec2(sprite->sprite->width, sprite->sprite->height);;
			}
			else // if (img->anchor == Anchor::bottomRight)
			{
				anchorPos = glm::vec2(Game::main.rightX, Game::main.bottomY) + glm::vec2(-sprite->sprite->width, sprite->sprite->height);;
			}

			pos->x = anchorPos.x + img->x;
			pos->y = anchorPos.y + img->y;
		}
	}
}

void ImageSystem::AddComponent(Component* component)
{
	images.push_back((ImageComponent*)component);
}

void ImageSystem::PurgeEntity(Entity* e)
{
	for (int i = 0; i < images.size(); i++)
	{
		if (images[i]->entity == e)
		{
			ImageComponent* s = images[i];
			images.erase(std::remove(images.begin(), images.end(), s), images.end());
			delete s;
		}
	}
}

#pragma endregion

#pragma endregion
