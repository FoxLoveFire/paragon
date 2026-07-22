#pragma once
#include "../../sdk/cs2/datatypes/usercmd/usercmd.h"

namespace Defensive
{
void OnCreateMove(CUserCmd* cmd);
bool ShouldChokePacket();
}