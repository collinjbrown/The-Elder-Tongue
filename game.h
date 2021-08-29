#ifndef GAME_H
#define GAME_H

class Game
{
public:
	static Game main;

	float camX;
	float camY;
	float camZ;

	int windowWidth = 1280;
	int windowHeight = 720;

	// QuadRenderer* quadRenderer;
};

#endif