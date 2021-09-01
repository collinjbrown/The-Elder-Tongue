#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include <unordered_map>
#include "component.h"

// Entities are the basic objects in the game.
// While it would be perhaps faster to implement a
// more robust ECS architecture, we aren't exactly
// playing a high-speed game that necessarily
// requires it, yet (though that may change).
// We are going to bundle components into objects.
// They won't contain any of the logic, just data---
// that much will still be contained in systems---
// but we won't worry overly much about having that
// information accessed in a contiguous fashion.

// As everything in this roguelike should have some presence
// in the game world, we're going to hard code position,
// velocity, and sprites into each entity.
// This also helps because lots of components will want to access
// these without having to dig through other components.

class Entity
{
private:
    // Basic Info
    int ID;
    std::string name;

public:
    // Components
    std::unordered_map<int, Component*> componentIDMap;
    std::vector<Component*> components;

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
