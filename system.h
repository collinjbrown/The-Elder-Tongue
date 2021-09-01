#ifndef SYSTEM_H
#define SYSTEM_H

#include "component.h"
#include "game.h"
#include <vector>

using namespace std;

class System
{
public:
	virtual void Update(float deltaTime) = 0;
	virtual void AddComponent(Component* component) = 0;
};

class RenderingSystem : public System
{
public:
	vector<SpriteComponent*> sprites;

	void Update(float deltaTime)
	{
		for (int i = 0; i < sprites.size(); i++)
		{
			SpriteComponent* s = sprites[i];
			PositionComponent* pos = (PositionComponent*)s->entity->componentIDMap[positionComponentID];
			Game::main.renderer->prepareQuad(glm::vec3(pos->x, pos->y, pos->z), s->width, s->height, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), s->sprite->ID);
		}
	}

	void AddComponent(Component* component)
	{
		sprites.push_back((SpriteComponent*)component);
	}
};

class PhysicsSystem : public System
{
public:
	vector<PhysicsComponent*> phys;

	void Update(float deltaTime)
	{
		for (int i = 0; i < phys.size(); i++)
		{
			PhysicsComponent* p = phys[i];
			PositionComponent* pos = (PositionComponent*)p->entity->componentIDMap[positionComponentID];

			p->velocityY -= p->gravityMod * deltaTime;

			if (p->velocityX > 0)
			{
				p->velocityX -= p->drag * deltaTime;
			}
			else if (p->velocityX < 0)
			{
				p->velocityX += p->drag * deltaTime;
			}

			if (p->velocityY > 0)
			{
				p->velocityY -= p->drag * deltaTime;
			}
			else if (p->velocityY < 0)
			{
				p->velocityY += p->drag * deltaTime;
			}

			pos->x += p->velocityX * deltaTime;
			pos->y += p->velocityY * deltaTime;
		}
	}

	void AddComponent(Component* component)
	{
		phys.push_back((PhysicsComponent*)component);
	}
};

#endif
