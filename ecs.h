#ifndef ECS_H
#define ECS_H

#include <vector>
#include <map>
#include "system.h"
#include "component.h"

using namespace std;

// This might seem a little strange, so we'll have to explain why this is set up
// the way it is...

class ComponentBlock
{
public:
	System* system;
	int componentID;

	void Update(float deltaTime)
	{
		system->Update(deltaTime);
	}
	void AddComponent(Component* c)
	{
		system->AddComponent(c);
	}

	ComponentBlock(System* system, int componentID)
	{
		this->system = system;
		this->componentID = componentID;
	}
};


class ECS
{
private:
	uint32_t entityIDCounter = 0;
	int round = 0;

public:
	static ECS main;
	vector<Entity*> entities;

	vector<ComponentBlock*> componentBlocks;

	uint32_t GetID()
	{
		return ++entityIDCounter;
	}

	void Init()
	{
		// I think we're going to have to initiate every component block
		// at the beginning of the game. This might be long.

		InputSystem* inputSystem = new InputSystem();
		ComponentBlock* inputBlock = new ComponentBlock(inputSystem, inputComponentID);
		componentBlocks.push_back(inputBlock);

		PhysicsSystem* physicsSystem = new PhysicsSystem();
		ComponentBlock* physicsBlock = new ComponentBlock(physicsSystem, physicsComponentID);
		componentBlocks.push_back(physicsBlock);

		ColliderSystem* colliderSystem = new ColliderSystem();
		ComponentBlock* colliderBlock = new ComponentBlock(colliderSystem, colliderComponentID);
		componentBlocks.push_back(colliderBlock);

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

	void Update(float deltaTime)
	{
		round++;

		if (round == 1)
		{
			#pragma region Player Instantiation
			Entity* player = CreateEntity("The Player");
			Animation2D* anim1 = Game::main.animationMap["baseIdle"];

			ECS::main.RegisterComponent(new PositionComponent(player, true, false, 0, 100, 0.0f), player);
			ECS::main.RegisterComponent(new PhysicsComponent(player, true, (PositionComponent*)player->componentIDMap[positionComponentID], 0.0f, 0.0f, 0.0f, 1000.0f, 2000.0f), player);
			ECS::main.RegisterComponent(new ColliderComponent(player, true, (PositionComponent*)player->componentIDMap[positionComponentID], false, false, 1.0f, 0.2f, 1.0f, 25.0f, 55.0f, 0.0f, 0.0f), player);
			ECS::main.RegisterComponent(new MovementComponent(player, true, 2000.0f, 500.0f, 2.5f, 100.0f, 0.1f, true, true, false), player);
			ECS::main.RegisterComponent(new InputComponent(player, true, true, 0.5f, 5000, 0.5f, 2), player);
			ECS::main.RegisterComponent(new CameraFollowComponent(player, true, 10.0f), player);
			ECS::main.RegisterComponent(new HealthComponent(player, true, 1000.0f, 1000.0f, 1000.0f, 0.0f, 10.0f, false), player);
			ECS::main.RegisterComponent(new DuelistComponent(player, true, true, true), player);
			ECS::main.RegisterComponent(new AnimationComponent(player, true, (PositionComponent*)player->componentIDMap[positionComponentID], anim1, "idle"), player);
			AnimationComponent* a = (AnimationComponent*)player->componentIDMap[animationComponentID];
			ECS::main.RegisterComponent(new DragonriderAnimationControllerComponent(player, true, a), player);
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
			Entity* character = CreateEntity("Test Character");
			Animation2D* anim7 = Game::main.animationMap["testIdle"];
			Animation2D* anim8 = Game::main.animationMap["testWalk"];
			Animation2D* anim9 = Game::main.animationMap["testJumpPrep"];
			Animation2D* anim10 = Game::main.animationMap["testJumpUp"];
			Animation2D* anim11 = Game::main.animationMap["testJumpDown"];
			Animation2D* anim12 = Game::main.animationMap["testDeath"];

			ECS::main.RegisterComponent(new PositionComponent(character, true, false, 100, 100, 0.0f), character);
			ECS::main.RegisterComponent(new PhysicsComponent(character, true, (PositionComponent*)character->componentIDMap[positionComponentID], 0.0f, 0.0f, 0.0f, 200.0f, 1000.0f), character);
			ECS::main.RegisterComponent(new ColliderComponent(character, true, (PositionComponent*)character->componentIDMap[positionComponentID], false, false, 1.0f, 0.2f, 1.0f, 25.0f, 55.0f, 0.0f, 0.0f), character);
			ECS::main.RegisterComponent(new MovementComponent(character, true, 1000.0f, 500.0f, 2.5f, 1.0f, 0.25f, true, true, false), character);
			ECS::main.RegisterComponent(new AnimationComponent(character, true, (PositionComponent*)character->componentIDMap[positionComponentID], anim7, "idle"), character);
			ECS::main.RegisterComponent(new DuelistComponent(character, true, false, false), character);
			ECS::main.RegisterComponent(new HealthComponent(character, true, 1000.0f, 1000.0f, 1000.0f, 0.0f, 1.0f, false), character);
			AnimationComponent* a2 = (AnimationComponent*)character->componentIDMap[animationComponentID];
			ECS::main.RegisterComponent(new DragonriderAnimationControllerComponent(character, true, a2), character);
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
				ECS::main.RegisterComponent(new ColliderComponent(platform, true, (PositionComponent*)platform->componentIDMap[positionComponentID], true, false, 1000.0f, 0.0f, 1.0f, 540.0f, 80.0f, 0.0f, 0.0f), platform);
				ECS::main.RegisterComponent(new StaticSpriteComponent(platform, true, (PositionComponent*)platform->componentIDMap[positionComponentID], tex3->width * 35, tex3->height * 5.0f, tex3), platform);
			}

			for (int i = 0; i < 50; i++)
			{
				Entity* floor = CreateEntity("floor");
				ECS::main.RegisterComponent(new PositionComponent(floor, true, true, i * 500, -200, 0.0f), floor);
				ECS::main.RegisterComponent(new PhysicsComponent(floor, true, (PositionComponent*)floor->componentIDMap[positionComponentID], 0.0f, 0.0f, 0.0f, 0.1f, 0.0f), floor);
				ECS::main.RegisterComponent(new ColliderComponent(floor, true, (PositionComponent*)floor->componentIDMap[positionComponentID], true, false, 1000.0f, 0.0f, 1.0f, 540.0f, 80.0f, 0.0f, 0.0f), floor);
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
	}

	Entity* CreateEntity(std::string name)
	{
		Entity* e = new Entity(GetID(), name);
		return e;
	}

	void RegisterComponent(Component* component, Entity* entity)
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
};

#endif
