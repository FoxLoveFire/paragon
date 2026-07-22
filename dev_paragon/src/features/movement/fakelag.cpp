#include "fakelag.h"
#include "../../sdk/hooks.h"
#include "../../../globals.h"
#include <algorithm>
#include <random>
#include <Windows.h>

#define FL_ONGROUND (1<<0)

namespace FakeLag
{
    static int choked_ticks = 0;
    static int max_choke = 0;
    static bool is_peeking = false;
    static std::mt19937 rng(std::random_device{}());

    void OnCreateMove(CUserCmd* cmd)
    {
        if (!G::fakelag.enabled || !cmd)
            return;

        if (choked_ticks > 0 && cmd->csgoUserCmd.pBaseCmd)
        {
        }
    }

    bool ShouldChokePacket()
    {
        if (!G::fakelag.enabled)
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
        {
            choked_ticks = 0;
            return false;
        }

        int flags = local->getFlags();
        bool on_ground = (flags & FL_ONGROUND) != 0;
        bool is_moving = (GetAsyncKeyState('W') & 0x8000) || (GetAsyncKeyState('A') & 0x8000) ||
                         (GetAsyncKeyState('S') & 0x8000) || (GetAsyncKeyState('D') & 0x8000);

        int target_limit = G::fakelag.limit;

        switch (G::fakelag.mode)
        {
        case 1:
            target_limit = std::uniform_int_distribution<int>(1, G::fakelag.limit)(rng);
            break;
        case 2:
            if (is_moving)
                target_limit = (std::max)(1, G::fakelag.limit / 2);
            else
                target_limit = G::fakelag.limit;
            break;
        case 3:
            is_peeking = is_moving;
            if (is_peeking)
                target_limit = G::fakelag.peek_limit;
            break;
        default:
            break;
        }

        if (G::fakelag.valve_mode)
            target_limit = (std::min)(target_limit, 6);

        max_choke = target_limit;

        if (choked_ticks < max_choke)
        {
            ++choked_ticks;
            return true;
        }

        choked_ticks = 0;
        return false;
    }
}