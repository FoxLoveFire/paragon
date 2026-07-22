#pragma once
#include "../../sdk/math/vector3d.h"
#include "../../sdk/utils/viewmatrix/viewmatrix.h"
#include "../../../external/imgui/imgui.h"
#include "../../sdk/memory/memory.h"
#include "../../sdk/Interfaces/Interfaces.h"
#include "../../sdk/cs2/classes/CCSPlayerController/CCSPlayerController.h"
#include "../../sdk/hooks.h"
#include <vector>

class LocalPlayerCached {
public:
	int handle;
	int health;
	int team;
	bool alive;
	vector3d poisition;
	void reset() {
		poisition = vector3d();
		team = 0;
		health = 0;
		handle = 0;
		alive = false;
	}
};

enum PlayerType_t : int
{
	none = -1,
	enemy = 0,
	team = 1,
};

struct PlayerCache
{
	PlayerCache(C_BaseEntity* a1, C_CSPlayerPawn* plyr, CBaseHandle hdl, PlayerType_t typ, int hp, const char* nm,
		const char* wep_name, vector3d pos, vector3d viewOff, int teamnm) :
		entity(a1), player(plyr), handle(hdl), type(typ), health(hp), name(nm),
		weapon_name(wep_name), position(pos), viewOffset(viewOff), team_num(teamnm) {}

	C_BaseEntity* entity;
	C_CSPlayerPawn* player;
	CBaseHandle handle;
	PlayerType_t type;
	int health;
	const char* name;
	const char* weapon_name;
	vector3d position;
	vector3d viewOffset;
	int team_num;
};

namespace Esp {
	class Visuals {
	public:
		void init();
		void esp();
	private:
		ViewMatrix viewMatrix;
	};
	void cache();
}

extern LocalPlayerCached cached_local;
extern std::vector<PlayerCache> cached_players;