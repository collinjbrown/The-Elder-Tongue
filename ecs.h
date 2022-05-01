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

#pragma region Nodes

struct Node
{
	int x;
	int y;
	int parentX;
	int parentY;

	bool blocked;
	bool ground;

	ColliderComponent* col;

	float g;
	float h;
	float F() { return g + h; }

	Node(int x, int y)
	{
		this->x = x;
		this->y = y;

		this->parentX = 0;
		this->parentY = 0;

		blocked = false;
		ground = false;

		this->g = INFINITY;
		this->h = 0.0f;
	}
};

inline bool operator < (const Node& lhs, const Node& rhs)
{
	return (lhs.g + lhs.h) < (rhs.g + rhs.h);
}

#pragma endregion

class ComponentBlock
{
public:
	System* system;
	int componentID;

	void Update(int activeScene, float deltaTime);
	void AddComponent(Component* c);
	void PurgeEntity(Entity* e);
	ComponentBlock(System* system, int componentID);
};

class ECS
{
private:
	uint32_t entityIDCounter = 0;
	int round = 0;
	static const int mWidth = 100;
	static const int mHeight = 100;

public:
	static ECS main;
	int activeScene;
	Entity* player;

	vector<Entity*> entities;
	vector<Entity*> dyingEntities;

	float nodeSize = 5.0f;
	Node* nodeMap[mWidth][mHeight];

	vector<ComponentBlock*> componentBlocks;

	uint32_t GetID();
	void Init();
	void Update(float deltaTime);
	void CreateNodeMap();
	Entity* CreateEntity(int scene, std::string name);
	void DeleteEntity(Entity* e);
	void AddDeadEntity(Entity* e);
	void PurgeDeadEntities();
	void RegisterComponent(Component* component, Entity* entity);
};

#endif
