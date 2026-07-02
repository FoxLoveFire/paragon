#pragma once
#include <stdint.h>
#include "../math/vector3d.h"
#include "../memory/memory.h"

enum render_frame_stage : int
{
	FRAME_NET_UPDATE_POSTDATAUPDATE_START = 2,
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	FRAME_NET_FULL_FRAME_UPDATE_ON_REMOVE,
	FRAME_RENDER_START,
	FRAME_RENDER_END,
	FRAME_NET_UPDATE_END,
	FRAME_NET_CREATION,
	FRAME_RESTORE_SERVER_STATE,
	FRAME_SIMULATE_END
};

enum engine_vfuncs
{
	IS_IN_GAME = 39,
	IS_CONNECTED = 40,

	GET_LOCAL_PLAYER_INDEX = 54,
	CLIENT_CMD_UNRESTRICTED = 50,
	GET_NETWORKED_CLIENT_INFO = 179,
};

class c_local_data
{
public:
	char pad_0000[12]; //0x0000
	vector3d eye_pos; //0x000C
}; //Size: 0x0018

class c_networked_client_info
{
public:
	int32_t unk; //0x0000
	int32_t render_tick; //0x0004
	float render_tick_fraction; //0x0008
	int32_t player_tick_count; //0x000C
	float player_tick_fraction; //0x0010
	char pad_0014[4]; //0x0014
	c_local_data* local_data; //0x0018
	char pad_0020[32]; //0x0020
}; //Size: 0x0040

class c_engine_client
{
public:

	bool is_in_game();

	bool is_connected();

	int get_local_player_index();

	bool valid();
};

class c_engine_pvs {
public:
	void set(bool value)
	{
		Memory::call_vfunc<void*>(this, 6, value);
	}
};