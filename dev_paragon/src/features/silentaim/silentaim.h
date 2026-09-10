#pragma once
#include "../esp/esp.h"
#include "../../sdk/math/vector3d.h"
#include "../../sdk/cs2/datatypes/usercmd/usercmd.h"
#include "../../sdk/Interfaces/Interfaces.h"

namespace silentaim
{

	void runSilentAim(CCSGOInput* pInput, CUserCmd* pCmd);
}