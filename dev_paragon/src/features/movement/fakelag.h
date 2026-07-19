#pragma once
#include <Windows.h>

namespace FakeLag
{
    using SendMovePacketFn = void(__fastcall*)(void* rcx);
    inline SendMovePacketFn oSendMovePacket = nullptr;

    void __fastcall hkSendMovePacket(void* rcx);
}