#ifndef SYSTEM_H
#define SYSTEM_H

#include "component.h"
#include <vector>

using namespace std;


class RenderingSystem
{
public:
	vector<SpriteComponent> sprites;

	void Update()
	{
		for (int i = 0; i < sprites.size(); i++)
		{
			SpriteComponent s = sprites[i];
			Game::main.renderer->prepareQuad(glm::vec2(s.position->x, s.position->y), s.width, s.height, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), s.texture->ID);
		}
	}
};

class PhysicsSystem
{
public:
	vector<PhysicsComponent> vels;

	void Update()
	{
		for (int i = 0; i < vels.size(); i++)
		{
			vels[i].velocityY -= vels[i].gravityMod;

			vels[i].position->x += vels[i].velocityX;
			vels[i].position->y += vels[i].velocityY;
		}
	}
};

#endif
