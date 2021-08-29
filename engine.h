#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include "entity.h"

using namespace std;

class Engine
{
public:
	static Engine main;
	vector<Entity> entities;
};

#endif