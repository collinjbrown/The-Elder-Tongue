#ifndef GAME_H
#define GAME_H

#include "renderer.h"
#include <glm/glm.hpp>
#include <vector>
#include <map>

using namespace std;

// The game class holds data about the broader game state;
// if you have data that pertains to the actual gameplay
// and world state, put it in the engine.

class Game
{
public:
	static Game main;

	map<string, Texture2D*> textureMap;
	map<string, Animation2D*> animationMap;
	GLFWwindow* window;

	int windowWidth = 1280;
	int windowHeight = 960;

	float camX = 0.0f;
	float camY = 0.0f;
	float camZ = 120.0f;
	float zoom = 0.5f;

	float mouseX = 0.0f;
	float mouseY = 0.0f;

	float deltaMouseX = 0.0f;
	float deltaMouseY = 0.0f;

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