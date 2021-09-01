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

class ColliderSystem : public System
{
	vector<ColliderComponent*> colls;

	void Update(float deltaTime)
	{
		for (int i = 0; i < colls.size(); i++)
		{
			ColliderComponent* cA = colls[i];
			PositionComponent* posA = (PositionComponent*)cA->entity->componentIDMap[positionComponentID];
			PhysicsComponent* physA = (PhysicsComponent*)cA->entity->componentIDMap[physicsComponentID];

			float tentativeADX = (physA->velocityX - physA->drag) * deltaTime;
			float tentativeADY = ((physA->velocityY - physA->drag) + (physA->gravityMod * deltaTime)) * deltaTime;

			// Change this later so that it isn't so slow.
			// I'm assuming this is slow.
			for (int j = 0; j < colls.size(); j++)
			{
				ColliderComponent* cB = colls[j];

				if (cB->entity->Get_ID() != cA->entity->Get_ID())
				{
					PositionComponent* posB = (PositionComponent*)cB->entity->componentIDMap[positionComponentID];
					PhysicsComponent* physB = (PhysicsComponent*)cB->entity->componentIDMap[physicsComponentID];

					float tentativeBDX = (physB->velocityX - physB->drag) * deltaTime;
					float tentativeBDY = ((physB->velocityY - physB->drag) + (physB->gravityMod * deltaTime)) * deltaTime;

					if (AreOverlapping(cA, glm::vec2(posA->x, posA->y), cB, glm::vec2(posB->x, posB->y)))
					{
						// Counteract a collision that has already occurred.

						physA->velocityY *= -1;

						/*glm::vec2 colA = glm::vec2(posA->x + (cA->width / 2) + cA->offsetX, posA->y + (cA->height / 2) + cA->offsetY);
						glm::vec2 colB = glm::vec2(posB->x + (cB->width / 2) + cB->offsetX, posB->y + (cB->height / 2) + cB->offsetY);


						glm::vec2 normal = (colB - colA) / abs(colB - colA);*/


					}
					//else if (AreOverlapping(cA, glm::vec2(posA->x + tentativeADX, posA->y + tentativeADX), cB, glm::vec2(posB->x + tentativeBDX, posB->y + tentativeBDY)))
					//{
					//	// Counteract a collision that will soon (probably) occur.


					//}
				}
			}
		}
	}

	glm::vec2 ResolveCollision(ColliderComponent* colA, glm::vec2 posA, ColliderComponent* colB, glm::vec2 posB)
	{
		float aLX = posA.x - (colA->width / 2) + colA->offsetX;
		float aLY = posA.y + (colA->height / 2) + colA->offsetY;

		float aRX = posA.x + (colA->width / 2) + colA->offsetX;
		float aRY = posA.y - (colA->height / 2) + colA->offsetY;

		float bLX = posB.x - (colB->width / 2) + colB->offsetX;
		float bLY = posB.y + (colB->height / 2) + colB->offsetY;

		float bRX = posB.x + (colB->width / 2) + colB->offsetX;
		float bRY = posB.y - (colB->height / 2) + colB->offsetY;


	}

	bool AreOverlapping(ColliderComponent* colA, glm::vec2 posA, ColliderComponent* colB, glm::vec2 posB)
	{
		float aLX = posA.x - (colA->width / 2) + colA->offsetX;
		float aLY = posA.y + (colA->height / 2) + colA->offsetY;

		float aRX = posA.x + (colA->width / 2) + colA->offsetX;
		float aRY = posA.y - (colA->height / 2) + colA->offsetY;

		float bLX = posB.x - (colB->width / 2) + colB->offsetX;
		float bLY = posB.y + (colB->height / 2) + colB->offsetY;

		float bRX = posB.x + (colB->width / 2) + colB->offsetX;
		float bRY = posB.y - (colB->height / 2) + colB->offsetY;


		return (aLX < bRX && aRX > bLX &&
				aLY < bRY && aRY > bLY);
	}

	void AddComponent(Component* component)
	{
		colls.push_back((ColliderComponent*)component);
	}
};

#endif
