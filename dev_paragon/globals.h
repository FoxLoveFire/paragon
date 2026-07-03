#pragma once
#include "src/sdk/math/color.h"

namespace G
{
	struct Menu_t
	{
		bool show = true;
	};

	inline Menu_t menu = Menu_t();

	struct WorldModulation_t
	{
		c_color light_color = c_color(255, 0, 255);
		c_color wall_color = c_color(0, 255, 0);
	};

	inline WorldModulation_t world = WorldModulation_t();
}