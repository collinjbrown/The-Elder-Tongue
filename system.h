#ifndef SYSTEM_H
#define SYSTEM_H

#include "component.h"
#include "game.h"
#include <vector>
#include <array>

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
			Game::main.renderer->prepareQuad(pos, s->width, s->height, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), s->sprite->ID);
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

			if (!pos->stat)
			{
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

				if (p->rotVelocity > 0)
				{
					p->rotVelocity -= p->drag * deltaTime;
				}
				else if (p->rotVelocity < 0)
				{
					p->rotVelocity += p->drag * deltaTime;
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

					// This does not yet solve for tunneling.
					TestAndResolveCollision(cA, posA, physA, cB, posB, physB);
				}
			}
		}
	}

	bool AreOverlapping(ColliderComponent* colA, PositionComponent* posA, ColliderComponent* colB, PositionComponent* posB)
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

		glm::vec2 bCenter = glm::vec2(posB->x, posB->y) + posA->Rotate(glm::vec2(bCX, bCY));
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
							Game::main.renderer->prepareQuad(aTopRight, aBottomRight, aBottomLeft, aTopLeft, glm::vec4(1.0f, 0.0f, 0.0f, 0.5f), Game::main.textureMap["blank"]->ID);
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
							Game::main.renderer->prepareQuad(aTopRight, aBottomRight, aBottomLeft, aTopLeft, glm::vec4(1.0f, 0.0f, 0.0f, 0.5f), Game::main.textureMap["blank"]->ID);
							return true;
						}
					}
				}
			}
		}

		Game::main.renderer->prepareQuad(aTopRight, aBottomRight, aBottomLeft, aTopLeft, glm::vec4(0.0f, 0.0f, 0.0f, 0.5f), Game::main.textureMap["blank"]->ID);
		return false;
	}

	void TestAndResolveCollision(ColliderComponent* colA, PositionComponent* posA, PhysicsComponent* physA, ColliderComponent* colB, PositionComponent* posB, PhysicsComponent* physB)
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

		glm::vec2 bCenter = glm::vec2(posB->x, posB->y) + posA->Rotate(glm::vec2(bCX, bCY));
		glm::vec2 bTopLeft = glm::vec2(posB->x, posB->y) + posB->Rotate(glm::vec2(bLX, bTY));
		glm::vec2 bBottomLeft = glm::vec2(posB->x, posB->y) + posB->Rotate(glm::vec2(bLX, bBY));
		glm::vec2 bTopRight = glm::vec2(posB->x, posB->y) + posB->Rotate(glm::vec2(bRX, bTY));
		glm::vec2 bBottomRight = glm::vec2(posB->x, posB->y) + posB->Rotate(glm::vec2(bRX, bBY));

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

							displacement.x += (1.0f - t1) * (lineB.x - lineA.x);
							displacement.y += (1.0f - t1) * (lineB.y - lineA.y);
						}
					}

					if (displacement.x != 0 || displacement.y != 0)
					{
						glm::vec2 right = Normalize(collEdge);
						glm::vec2 up = { right.y, right.x };

						glm::vec2 aDispNormal = up;
						glm::vec2 bDispNormal = -up;

						// glm::vec2 aDispNormal = Normalize(glm::vec2(displacement.x, displacement.y));
						//glm::vec2 bDispNormal = -aDispNormal;

						// We want to apply "bounce" to perpendicular velocity
						// and apply "friction" to parallel velocity.

						glm::vec2 aVel = glm::vec2(physA->velocityX, physA->velocityY);
						glm::vec2 bVel = glm::vec2(physB->velocityX, physB->velocityY);

						if (colA->bounce != 0)
						{
							glm::vec2 aNewVelocity = (aVel + (Project(aVel, aDispNormal) * -2.0f) * colA->bounce) * (1 - (colA->mass / totalMass));
							physA->velocityX = aNewVelocity.x;
							physA->velocityY = aNewVelocity.y;
						}

						if (colB->bounce != 0)
						{
							glm::vec2 bNewVelocity = (bVel + (Project(bVel, bDispNormal) * -2.0f) * colB->bounce) * (1 - (colB->mass / totalMass));
							physB->velocityX = bNewVelocity.x;
							physB->velocityY = bNewVelocity.y;
						}

						/*if (physA->velocityX > 0)
						{
							physA->velocityX -= colB->friction * aDispNormal.y * (1 - (colA->mass / totalMass));
						}
						else if (physA->velocityX < 0)
						{
							physA->velocityX += colB->friction * aDispNormal.y * (1 - (colA->mass / totalMass));

						}

						if (physA->velocityY > 0)
						{
							physA->velocityY -= colB->friction * aDispNormal.x * (1 - (colA->mass / totalMass));
						}
						else if (physA->velocityY < 0)
						{
							physA->velocityY += colB->friction * aDispNormal.x * (1 - (colA->mass / totalMass));
						}

						if (physB->velocityX > 0)
						{
							physB->velocityX -= colA->friction * bDispNormal.y * (1 - (colA->mass / totalMass));
						}
						else if (physB->velocityX < 0)
						{
							physB->velocityX += colA->friction * bDispNormal.y * (1 - (colA->mass / totalMass));
						}

						if (physB->velocityY > 0)
						{
							physB->velocityY -= colA->friction * bDispNormal.x * (1 - (colA->mass / totalMass));
						}
						else if (physB->velocityY < 0)
						{
							physB->velocityY += colA->friction * bDispNormal.x * (1 - (colA->mass / totalMass));
						}*/
						
						if (!posA->stat && !posB->stat)
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

							displacement.x += (1.0f - t1) * (lineB.x - lineA.x);
							displacement.y += (1.0f - t1) * (lineB.y - lineA.y);
						}
					}

					if (displacement.x != 0 || displacement.y != 0)
					{
						glm::vec2 right = Normalize(collEdge);
						glm::vec2 up = { right.y, right.x };

						glm::vec2 aDispNormal = -up;
						glm::vec2 bDispNormal = up;

						// glm::vec2 aDispNormal = Normalize(glm::vec2(displacement.x, displacement.y));
						// glm::vec2 bDispNormal = -aDispNormal;

						// We want to apply "bounce" to perpendicular velocity
						// and apply "friction" to parallel velocity.

						glm::vec2 aVel = glm::vec2(physA->velocityX, physA->velocityY);
						glm::vec2 bVel = glm::vec2(physB->velocityX, physB->velocityY);

						if (colA->bounce != 0)
						{
							glm::vec2 aNewVelocity = (aVel + (Project(aVel, aDispNormal) * -2.0f) * colA->bounce) * (1 - (colA->mass / totalMass));
							physA->velocityX = aNewVelocity.x;
							physA->velocityY = aNewVelocity.y;
						}

						if (colB->bounce != 0)
						{
							glm::vec2 bNewVelocity = (bVel + (Project(bVel, bDispNormal) * -2.0f) * colB->bounce) * (1 - (colB->mass / totalMass));
							physB->velocityX = bNewVelocity.x;
							physB->velocityY = bNewVelocity.y;
						}

						/*if (physA->velocityX > 0)
						{
							physA->velocityX -= colB->friction * aDispNormal.y * (1 - (colA->mass / totalMass));
						}
						else if (physA->velocityX < 0)
						{
							physA->velocityX += colB->friction * aDispNormal.y * (1 - (colA->mass / totalMass));

						}

						if (physA->velocityY > 0)
						{
							physA->velocityY -= colB->friction * aDispNormal.x * (1 - (colA->mass / totalMass));
						}
						else if (physA->velocityY < 0)
						{
							physA->velocityY += colB->friction * aDispNormal.x * (1 - (colA->mass / totalMass));
						}

						if (physB->velocityX > 0)
						{
							physB->velocityX -= colA->friction * bDispNormal.y * (1 - (colA->mass / totalMass));
						}
						else if (physB->velocityX < 0)
						{
							physB->velocityX += colA->friction * bDispNormal.y * (1 - (colA->mass / totalMass));
						}

						if (physB->velocityY > 0)
						{
							physB->velocityY -= colA->friction * bDispNormal.x * (1 - (colA->mass / totalMass));
						}
						else if (physB->velocityY < 0)
						{
							physB->velocityY += colA->friction * bDispNormal.x * (1 - (colA->mass / totalMass));
						}*/

						if (!posA->stat && !posB->stat)
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

	float Norm(glm::vec2 a)
	{
		return sqrt(a.x * a.x + a.y * a.y);
	}

	float Dot(glm::vec2 a, glm::vec2 b)
	{
		return a.x * b.x + a.y * b.y;
	}

	glm::vec2 Normalize(glm::vec2 a)
	{
		return a * (1 / Norm(a));
	}

	glm::vec2 Project(glm::vec2 v, glm::vec2 a)
	{
		return Normalize(a) * (Dot(v, a) / Norm(a));
	}

	glm::vec2 Bounce(glm::vec2 v, glm::vec2 n)
	{
		return v + (Project(v, n) * -2.0f);
	}

	void AddComponent(Component* component)
	{
		colls.push_back((ColliderComponent*)component);
	}
};

class MovementSystem : public System
{
public:
	vector<MovementComponent*> move;

	void Update(float deltaTime)
	{
		for (int i = 0; i < move.size(); i++)
		{
			MovementComponent* m = move[i];
			PhysicsComponent* phys = (PhysicsComponent*)m->entity->componentIDMap[physicsComponentID];

			

			if (glfwGetKey(Game::main.window, GLFW_KEY_W) == GLFW_PRESS)
			{
				phys->velocityY += 1.0f;
			}
			else if (glfwGetKey(Game::main.window, GLFW_KEY_S) == GLFW_PRESS)
			{
				phys->velocityY -= 1.0f;
			}

			if (glfwGetKey(Game::main.window, GLFW_KEY_D) == GLFW_PRESS)
			{
				phys->velocityX += 1.0f;
			}
			else if (glfwGetKey(Game::main.window, GLFW_KEY_A) == GLFW_PRESS)
			{

				phys->velocityX -= 1.0f;
			}
		}
	}

	void AddComponent(Component* component)
	{
		move.push_back((MovementComponent*)component);
	}
};

#endif
