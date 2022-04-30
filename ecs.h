#ifndef ECS_H
#define ECS_H

#include <vector>
#include <map>

using namespace std;

// This might seem a little strange, so we'll have to explain why this is set up
// the way it is...

class Entity;
class System;
class Component;

class ComponentBlock
{
public:
	System* system;
	int componentID;

	void Update(float deltaTime);
	void AddComponent(Component* c);
	void PurgeEntity(Entity* e);
	ComponentBlock(System* system, int componentID);
};

class ECS
{
private:
	uint32_t entityIDCounter = 0;
	int round = 0;

public:
	static ECS main;
	vector<Entity*> entities;
	vector<Entity*> dyingEntities;

	vector<ComponentBlock*> componentBlocks;

	uint32_t GetID();
	void Init();
	void Update(float deltaTime);
	Entity* CreateEntity(int scene, std::string name);
	void DeleteEntity(Entity* e);
	void AddDeadEntity(Entity* e);
	void PurgeDeadEntities();
	void RegisterComponent(Component* component, Entity* entity);
};

#endif
