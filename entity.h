#ifndef ENTITY_H
#define ENTITY_H

#include <vector>

// Entities are the basic objects in the game.
// Due to the small pool of entities in the game
// (or what I assume to be a small pool of entities now)
// we'll just use inheritance.

class Entity
{
private:
    int ID;
    std::string name;

public:
    
    int         Get_ID() { return ID; }
    std::string Get_Name() { return name; }

    void        Set_ID(int newID) { ID = newID; }
    void        Set_Name(std::string newName) { name = newName; }

    Entity(int ID, std::string name)
    {
        this->ID = ID;
        this->name = name;
    };

    Entity() {};
};

#endif
