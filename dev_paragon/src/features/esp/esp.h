#pragma once
#include "../../sdk/Interfaces/Interfaces.h"
#include "../../sdk/cs2/classes/CCSPlayerController/CCSPlayerController.h"
#include "../../sdk/math/vector3d.h"
#include "../../../external/imgui/imgui.h"
#include "../../sdk/sdk.h"
#include "../../sdk/math/mat4x4.h"

namespace ESP
{
    struct PlayerData {
        int health;
        vector3d position;
    };

	void store_players();
    void render_esp();
    bool world_to_screen(const vector3d& world, vector3d& screen);
}