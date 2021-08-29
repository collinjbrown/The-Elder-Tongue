#ifndef GAME_H
#define GAME_H

#include "renderer.h"
#include <glm/glm.hpp>
#include <vector>

class Game
{
public:
	static Game main;

	float camX = 0.0f;
	float camY = 0.0f;
	float camZ = 100.0f;
	float zoom = 1.0f;

	int windowWidth = 1280;
	int windowHeight = 720;

	float topY;
	float bottomY;
	float leftX;
	float rightX;

	glm::mat4 view;
	glm::mat4 projection;

	Renderer* renderer;
	void updateOrtho();
};

#endif