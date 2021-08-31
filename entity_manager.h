#ifndef  ENTITYMANAGER_H
#define  ENTITYMANAGER_H

#include "entity.h"
#include <vector>

// I've gone back and forth on whether or not I care about creating
// a robust ECS system for this game, and I've decided it doesn't really
// make sense for what we're doing.

// The game won't be so component / entity heavy that it would meaningfully
// improve performance; the only place where it might help is in procedural
// enemy and item generation, but I think we can handle those on their own.

class EntityManager
{
public:
	std::vector<Entity> entities;
	static EntityManager main;

	void Update()
	{
        #pragma region Movement

        #pragma endregion


        #pragma region Rendering
        for (int i = 0; i < entities.size(); i++)
        {
            if (entities[i].isShown)
            {
                Entity e = entities[i];

                Game::main.renderer->prepareQuad(glm::vec2(e.posX, e.posY), e.width, e.height, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), e.texture->ID);
            }
        }
        #pragma endregion
	}
};

#endif