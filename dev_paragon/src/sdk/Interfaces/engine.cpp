#include "engine.h"
#include "../memory/memory.h"

bool c_engine_client::is_in_game()
{
	return Memory::call_vfunc<bool>(this, IS_IN_GAME);
}

bool c_engine_client::is_connected()
{
	return Memory::call_vfunc<bool>(this, IS_CONNECTED);
}

int c_engine_client::get_local_player_index()
{
	int index{ -1 };

	Memory::call_vfunc<void>(this, GET_LOCAL_PLAYER_INDEX, &index, 0);
	return index + 1;
}

bool c_engine_client::valid()
{
	return is_in_game() && is_connected();
}