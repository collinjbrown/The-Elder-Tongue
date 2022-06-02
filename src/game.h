#ifndef GAME_H
#define GAME_H

#include "renderer.h"
#include <glm/glm.hpp>
#include <vector>
#include <map>

using namespace std;

// The game class holds data about the camera, window, and rendering.
// We may also add info about key mapping to this.

enum class InputType { button, trigger, stickPos, stickNeg };

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

	float topY = 0;
	float bottomY = 0;
	float leftX = 0;
	float rightX = 0;

	glm::mat4 view;
	glm::mat4 projection;

	Renderer* renderer;
	void updateOrtho();

	// Keyboard and Mouse Mappings
	int bladeShootKey = GLFW_MOUSE_BUTTON_1;
	int bladeManualTargetKey = GLFW_MOUSE_BUTTON_3;
	int bladeThrowKey = GLFW_MOUSE_BUTTON_2;

	int climbKey = GLFW_KEY_LEFT_SHIFT;
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

	InputType bladeShootPadType = InputType::trigger;
	int bladeShootPad = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER;

	InputType bladeManualTargetPadType = InputType::button;
	int bladeManualTargetPad = GLFW_GAMEPAD_BUTTON_LEFT_THUMB;

	InputType bladeThrowPadType = InputType::button;
	int bladeThrowPad = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER;

	InputType climbPadType = InputType::trigger;
	int climbPad = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER;

	InputType jumpPadType = InputType::button;
	int jumpPad = GLFW_GAMEPAD_BUTTON_A;

	InputType crouchPadType = InputType::button;
	int crouchPad = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER;


	InputType climbUpPadType = InputType::stickNeg;
	int climbUpPad = GLFW_GAMEPAD_AXIS_LEFT_Y;

	InputType climbDownPadType = InputType::stickPos;
	int climbDownPad = GLFW_GAMEPAD_AXIS_LEFT_Y;

	InputType moveRightPadType = InputType::stickPos;
	int moveRightPad = GLFW_GAMEPAD_AXIS_LEFT_X;

	InputType moveLeftPadType = InputType::stickNeg;
	int moveLeftPad = GLFW_GAMEPAD_AXIS_LEFT_X;

	InputType dashUpPadType = InputType::stickNeg;
	int dashUpPad = GLFW_GAMEPAD_AXIS_LEFT_Y;

	InputType dashDownPadType = InputType::stickPos;
	int dashDownPad = GLFW_GAMEPAD_AXIS_LEFT_Y;

	InputType dashRightPadType = InputType::stickPos;
	int dashRightPad = GLFW_GAMEPAD_AXIS_LEFT_X;

	InputType dashLeftPadType = InputType::stickNeg;
	int dashLeftPad = GLFW_GAMEPAD_AXIS_LEFT_X;


	InputType swordRotRightPadType = InputType::stickPos;
	int swordRotRightPad = GLFW_GAMEPAD_AXIS_RIGHT_X;

	InputType swordRotLeftPadType = InputType::stickNeg;
	int swordRotLeftPad = GLFW_GAMEPAD_AXIS_RIGHT_X;

	InputType swordRotUpPadType = InputType::stickNeg;
	int swordRotUpPad = GLFW_GAMEPAD_AXIS_RIGHT_Y;

	InputType swordRotDownPadType = InputType::stickPos;
	int swordRotDownPad = GLFW_GAMEPAD_AXIS_RIGHT_Y;
};

#endif