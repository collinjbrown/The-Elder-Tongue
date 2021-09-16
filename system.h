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

class StaticRenderingSystem : public System
{
public:
	vector<StaticSpriteComponent*> sprites;

	void Update(float deltaTime)
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

	void AddComponent(Component* component)
	{
		sprites.push_back((StaticSpriteComponent*)component);
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

			if (p->active)
			{
				PositionComponent* pos = p->pos;
				ColliderComponent* col = (ColliderComponent*)p->entity->componentIDMap[colliderComponentID];

				if (!pos->stat)
				{
					if (col != nullptr)
					{
						if (!col->onPlatform)
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
			cA->onPlatform = false;

			if (cA->active)
			{
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
										cA->platform && bBot > aTop - platformLeeway ||
										cB->platform && aBot > bTop - platformLeeway)
									{
										PhysicsComponent* physB = (PhysicsComponent*)cB->entity->componentIDMap[physicsComponentID];

										/*float tentativeBDX = (physB->velocityX - physB->drag) * deltaTime;
										float tentativeBDY = ((physB->velocityY - physB->drag) + (physB->gravityMod * deltaTime)) * deltaTime;*/

										// This does not yet solve for tunneling.
										if (TestAndResolveCollision(cA, posA, physA, cB, posB, physB))
										{
											if (cA->platform)
											{
												cB->onPlatform = true;
											}
											else if (cB->platform)
											{
												cA->onPlatform = true;
											}
										}
										else if (cB->platform)
										{
											if (RaycastDown(1, 10, cA, posA, cB, posB))
											{
												cA->onPlatform = true;
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

	bool RaycastDown(float size, float distance, ColliderComponent* colA, PositionComponent* posA, ColliderComponent* colB, PositionComponent* posB)
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
		glm::vec2 aBottomLeft = glm::vec2(posA->x, posA->y) + posA->Rotate(glm::vec2(nR, nR - size - ryC));
		glm::vec2 aTopRight = glm::vec2(posA->x, posA->y) + posA->Rotate(glm::vec2(r, r - ryC));
		glm::vec2 aBottomRight = glm::vec2(posA->x, posA->y) + posA->Rotate(glm::vec2(r, nR - size - ryC));

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

	bool TestAndResolveCollision(ColliderComponent* colA, PositionComponent* posA, PhysicsComponent* physA, ColliderComponent* colB, PositionComponent* posB, PhysicsComponent* physB)
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

							collided = true;
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

		return collided;
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

class InputSystem : public System
{
public:
	vector<InputComponent*> move;

	void Update(float deltaTime)
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
				
				if (!health->dead)
				{
					if (move->jumping && col->onPlatform)
					{
						move->jumping = false;
					}

					if (glfwGetMouseButton(Game::main.window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS && !duel->isAttacking && duel->hasSword && duel->isDrawn && col->onPlatform && abs(phys->velocityX) < 10.0f)
					{
						phys->velocityX = 0;

						if (Game::main.mouseX < phys->pos->x)
						{
							phys->velocityX -= move->stabDepth;
							duel->isAttacking = true;
							move->canMove = false;
						}
						else
						{
							phys->velocityX += move->stabDepth;
							duel->isAttacking = true;
							move->canMove = false;
						}
					}
					else if (move->preparingToJump)
					{
						CalculateProjection(phys, m, move);
					}

					if (glfwGetMouseButton(Game::main.window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS && move->canMove && !move->preparingToJump && col->onPlatform && abs(phys->velocityX) < 0.5f)
					{
						move->canMove = false;
						move->preparingToJump = true;
					}
					else if (glfwGetMouseButton(Game::main.window, GLFW_MOUSE_BUTTON_2) != GLFW_PRESS && move->preparingToJump)
					{
						move->canMove = true;
						move->jumping = true;
						move->preparingToJump = false;

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
					}

					if (glfwGetKey(Game::main.window, GLFW_KEY_D) == GLFW_PRESS && move->canMove && !move->jumping && !move->preparingToJump && col->onPlatform)
					{
						if (phys->velocityX < move->maxSpeed)
						{
							phys->velocityX += move->acceleration * deltaTime;
						}
					}
					else if (glfwGetKey(Game::main.window, GLFW_KEY_A) == GLFW_PRESS && move->canMove && !move->jumping && !move->preparingToJump && col->onPlatform)
					{
						if (phys->velocityX > -move->maxSpeed)
						{
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

	void CalculateProjection(PhysicsComponent* phys, InputComponent* m, MovementComponent* move)
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


	void AddComponent(Component* component)
	{
		move.push_back((InputComponent*)component);
	}
};

class CameraFollowSystem : public System
{
public:
	vector<CameraFollowComponent*> folls;

	void Update(float deltaTime)
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

	float Lerp(float a, float b, float t)
	{
		return (1 - t) * a + t * b;
	}

	void AddComponent(Component* component)
	{
		folls.push_back((CameraFollowComponent*)component);
	}
};

class AnimationControllerSystem : public System
{
public:
	vector<AnimationControllerComponent*> controllers;

	void Update(float deltaTime)
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

				DragonriderAnimationControllerComponent* d = (DragonriderAnimationControllerComponent*)c;
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
					if (p->velocityX < 0)
					{
						c->animator->flipped = true;
					}
					else if (p->velocityX > 0)
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
						if (c->animator->activeAnimation != s + "stab")
						{
							c->animator->SetAnimation(s + "stab");
						}
					}
					else if (abs(p->velocityY) > 10.0f && !col->onPlatform)
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
					else if (move->preparingToJump && c->animator->activeAnimation != s + "jumpPrep")
					{
						c->animator->SetAnimation(s + "jumpPrep");
					}
					else if (abs(p->velocityX) > 0.5f && col->onPlatform && move->canMove && c->animator->activeAnimation != s + "walk")
					{
						c->animator->SetAnimation(s + "walk");
					}
					else if (abs(p->velocityX) < 0.5f && move->canMove && c->animator->activeAnimation != s + "idle")
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

	void AddComponent(Component* component)
	{
		controllers.push_back((AnimationControllerComponent*)component);
	}
};

class AnimationSystem : public System
{
public:
	vector<AnimationComponent*> anims;

	void Update(float deltaTime)
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

	void AddComponent(Component* component)
	{
		anims.push_back((AnimationComponent*)component);
	}
};

class HealthSystem : public System
{
public:
	vector<HealthComponent*> healths;

	void Update(float deltaTime)
	{
		for (int i = 0; i < healths.size(); i++)
		{
			HealthComponent* h = healths[i];

			h->bleeding -= h->coalgulationRate * deltaTime;

			if (h->bleeding > 0)
			{
				h->blood -= h->bleeding * deltaTime;
			}
			else
			{
				h->bleeding = 0;
			}

			if (h->blood <= 0)
			{
				// You're dying.
				h->fatigue -= 10 * deltaTime;
				h->health -= 10 * deltaTime;
			}

			if (h->fatigue <= 0)
			{
				// You're unconscious.
				// We'll have to sort out how this should work later.
			}

			if (h->health <= 0)
			{
				// You're dead.
				h->dead = true;
			}
		}
	}

	void AddComponent(Component* component)
	{
		healths.push_back((HealthComponent*)component);
	}
};

class DuellingSystem : public System
{
public:
	vector<DuelistComponent*> duels;

	void Update(float deltaTime)
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
						if (!m->jumping && !m->preparingToJump && abs(p->velocityX) < 0.5f && abs(p->velocityY) < 0.5f)
						{
							d->isAttacking = false;
							m->canMove = true;
						}
					}

					if (d->lastTick > 0.5f)
					{
						if (!m->jumping && !m->preparingToJump && abs(p->velocityX) < 0.5f && abs(p->velocityY) < 0.5f)
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

	void AddComponent(Component* component)
	{
		duels.push_back((DuelistComponent*)component);
	}
};

#endif
