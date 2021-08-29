#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include "entity.h"

using namespace std;

// The purpose of this class is to hold all the entities and generally manage world states and all that
// so that the main file doesn't become cluttered.

class Engine
{
public:
	static Engine main;
	vector<Entity> entities;
};

#endif