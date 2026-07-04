#pragma once
#include <vector>
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
		bool custom_light = false;
		c_color light_color = c_color(255, 0, 255);
		
		bool custom_wall_color = false;
		c_color wall_color = c_color(0, 255, 0);

		int m_exposure = 0;
	};

	inline WorldModulation_t world = WorldModulation_t();

	struct Viewport_t
	{
		bool custom_fov = false;
		float fov = 90;
		float aspect_ratio = 1.77778f;
	};

	inline Viewport_t viewport = Viewport_t();

	struct Viewmodel_t
	{
		bool is_enabled = false;
		std::vector<float> xyz = {0.0f, 0.0f, 0.0f};
		float v_fov = 0.0f;
	};

	inline Viewmodel_t viewmodel = Viewmodel_t();

	struct Removals_t
	{
		bool disable_smoke = true;
		bool disable_scopeoverlay = true;
		bool remove_legs = true;
		bool disable_flashbang = true;
	};

	inline Removals_t removals = Removals_t();

	struct Misc_t
	{
		bool rh_enabled = false;
	};

	inline Misc_t misc = Misc_t();
}