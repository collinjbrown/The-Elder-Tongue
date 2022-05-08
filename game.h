#ifndef GAME_H
#define GAME_H

#include "renderer.h"
#include <glm/glm.hpp>
#include <vector>
#include <map>

using namespace std;

// The game class holds data about the camera, window, and rendering.
// We may also add info about key mapping to this.

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

	int bladeManualTargetButton = GLFW_MOUSE_BUTTON_3;
	int bladeThrowButton = GLFW_MOUSE_BUTTON_2;
	int attackButton = GLFW_MOUSE_BUTTON_1;

	int climbButton = GLFW_KEY_LEFT_SHIFT;
	int dropWeaponButton = GLFW_KEY_C;
	int jumpButton = GLFW_KEY_SPACE;
	int crouchButton = GLFW_KEY_LEFT_CONTROL;

	int climbUpButton = GLFW_KEY_W;
	int climbDownButton = GLFW_KEY_S;

	int moveRightButton = GLFW_KEY_D;
	int moveLeftButton = GLFW_KEY_A;
};

#endif