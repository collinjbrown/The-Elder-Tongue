#pragma once

#include "libs/entt/single_include/entt/entt.hpp"

class World
{
public:
	static World main;

	void Init()
	{
		entt::registry registry;

		auto entity = registry.create();
	}

	void Update()
	{
		
	}
};
