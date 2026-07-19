#include <includes.h>

namespace FakeLag 
{
    void __fastcall hkSendMovePacket(void* rcx) 
    {
        static int choked_ticks = 0;

        if (!G::fakelag.enabled) {
            choked_ticks = 0;
            return oSendMovePacket(rcx);
        }

        C_CSPlayerPawn* local_player = Hook::oGetLocalPlayer(0); 
        if (!local_player) {
            choked_ticks = 0;
            return oSendMovePacket(rcx);
        }

        if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) ||
            (GetAsyncKeyState('1') & 0x8000) || (GetAsyncKeyState('2') & 0x8000) || 
            (GetAsyncKeyState('3') & 0x8000) || (GetAsyncKeyState('4') & 0x8000) || 
            (GetAsyncKeyState('5') & 0x8000) || (GetAsyncKeyState('Q') & 0x8000) ||
            (GetAsyncKeyState('R') & 0x8000) || (GetAsyncKeyState(VK_SPACE) & 0x8000)) {
            choked_ticks = 0;
            return oSendMovePacket(rcx);
        }

        bool is_moving = (GetAsyncKeyState('W') & 0x8000) || (GetAsyncKeyState('A') & 0x8000) ||
                         (GetAsyncKeyState('S') & 0x8000) || (GetAsyncKeyState('D') & 0x8000);

        int current_limit = static_cast<int>(G::fakelag.limit);

        if (is_moving && current_limit > 8) {
            current_limit = 8; 
        }

        if (choked_ticks < current_limit) {
            choked_ticks++;
            return;
        } else {
            choked_ticks = 0;
            return oSendMovePacket(rcx);
        }
    }
}