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

	// Keyboard and Mouse Mappings
	int bladeManualTargetKey = GLFW_MOUSE_BUTTON_3;
	int bladeThrowKey = GLFW_MOUSE_BUTTON_2;
	int dashKey = GLFW_MOUSE_BUTTON_1;

	int climbKey = GLFW_KEY_LEFT_SHIFT;
	int dropWeaponKey = GLFW_KEY_C;
	int jumpKey = GLFW_KEY_SPACE;
	int crouchKey = GLFW_KEY_LEFT_CONTROL;

	int climbUpKey = GLFW_KEY_W;
	int climbDownKey = GLFW_KEY_S;

	int moveRightKey = GLFW_KEY_D;
	int moveLeftKey = GLFW_KEY_A;

	int dashUpKey = GLFW_KEY_W;
	int dashDownKey = GLFW_KEY_S;
	int dashRightKey = GLFW_KEY_D;
	int dashLeftKey = GLFW_KEY_A;

	// Gamepad Mappings
	int bladeManualTargetPad = GLFW_GAMEPAD_BUTTON_LEFT_THUMB;
	int bladeThrowPad = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER;
	int dashPad = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER;

	int climbPad = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER;
	int dropWeaponPad = GLFW_GAMEPAD_BUTTON_Y;
	int jumpPad = GLFW_GAMEPAD_BUTTON_A;
	int crouchPad = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER;

	int climbUpPad = GLFW_GAMEPAD_AXIS_LEFT_Y;
	int climbDownPad = GLFW_GAMEPAD_AXIS_LEFT_Y;

	int moveRightPad = GLFW_GAMEPAD_AXIS_LEFT_X;
	int moveLeftPad = GLFW_GAMEPAD_AXIS_LEFT_X;

	int dashUpPad = GLFW_GAMEPAD_AXIS_LEFT_Y;
	int dashDownPad = GLFW_GAMEPAD_AXIS_LEFT_Y;
	int dashRightPad = GLFW_GAMEPAD_AXIS_LEFT_X;
	int dashLeftPad = GLFW_GAMEPAD_AXIS_LEFT_X;

	int swordRotXPad = GLFW_GAMEPAD_AXIS_RIGHT_X;
	int swordRotYPad = GLFW_GAMEPAD_AXIS_RIGHT_Y;
};

#endif