#include "defensiveaa.h"
#include "../../sdk/hooks.h"
#include "../../../globals.h"
#include "../../sdk/math/QAngle_t.h"

namespace Defensive
{
static int choked_ticks = 0;

void OnCreateMove(CUserCmd* cmd)
{
    if (!G::defensive.enabled || !cmd)
        return;

    if (G::viewport.thirdperson)
        return;

    if (!cmd->csgoUserCmd.pBaseCmd || !cmd->csgoUserCmd.pBaseCmd->pViewAngles)
        return;

    QAngle_t& angles = cmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue;
    angles.y += G::defensive.fake_yaw;
    angles.clamp();
}

bool ShouldChokePacket()
{
    if (!G::defensive.enabled)
    {
        choked_ticks = 0;
        return false;
    }

    auto local = Hook::oGetLocalPlayer(0);
    if (!local || local->getHealth() <= 0)
    {
        choked_ticks = 0;
        return false;
    }

    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
        choked_ticks = G::defensive.max_choke;

    if (choked_ticks < G::defensive.max_choke)
    {
        ++choked_ticks;
        return true;
    }

    choked_ticks = 0;
    return false;
}
}