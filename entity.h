#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include "texture_2D.h"
#include "renderer.h"

// Entities are the basic objects in the game.
// They take components and whatnot.

class Entity
{
private:
    int ID;
    Quad* quad;
    std::string name;

public:
    bool       isShown;
    Texture2D* texture;
    float         posX;
    float         posY;
    float         posZ;

    float        width;
    float       height;
    
    int         Get_ID() { return ID; }
    std::string Get_Name() { return name; }

    void        Set_ID(int newID) { ID = newID; }
    void        Set_Name(std::string newName) { name = newName; }

    Entity(int ID, std::string name, bool isShown, Texture2D* texture, float x, float y, float z, float width, float height)
    {
        this->ID = ID;
        this->name = name;
        this->isShown = isShown;
        this->texture = texture;
        this->posX = x;
        this->posY = y;
        this->posZ = z;
        this->width = width;
        this->height = height;
    };

    Entity() {};
};

#endif