#pragma once
#include "../../sdk/cs2/datatypes/usercmd/usercmd.h"

namespace FakeLag
{
    void OnCreateMove(CUserCmd* cmd);
    bool ShouldChokePacket();
}