#pragma once
#include "../../external/imgui/imgui.h"
#include "../../external/imgui/imgui_impl_win32.h"
#include "../../external/imgui/imgui_impl_dx11.h"
#include "../../external/minhook/include/MinHook.h"
#include "../sdk/memory/memory.h"
#include "../sdk/Interfaces/Interfaces.h"
#include "../features/visuals/WorldModulation.h"
#include "../features/viewport/OverrideView.h"
#include "../features/viewport/Viewmodel.h"
#include "../features/visuals/Removals.h"
#include "cs2/classes/C_CSPlayerPawn/C_CSPlayerPawn.h"
#include "../features/misc/radarhack/radarhack.h"
#include "../features/chams/chams.h"
#include "../../globals.h"
#include "../features/esp/esp.h"
#include "dx11hook.h"
#include <string>
#include <unordered_map>
#include <Windows.h>
#include <iostream>
#include <d3d11.h>
#include <dxgi.h>
#include <cstdint>
#include <optional>
#include <array>
#include <d3dcompiler.h>
#pragma comment(lib, "d3d11.lib")

using  levelInitFn = __int64(__fastcall*)(__int64 a1, __int64 a2);
inline levelInitFn olevelInit = nullptr;
using FrameStageNotifyFn = void(__fastcall*)(void* a1, int stage);
inline FrameStageNotifyFn oFrameStageNotify = nullptr;

#define SetModel "40 53 48 83 EC ? 48 8B D9 4C 8B C2 48 8B 0D ? ? ? ? 48 8D 54 24"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
namespace Hook
{
    bool Hook(const char* pattern, LPVOID detour, LPVOID* original, std::string module);
    bool HookAddress(uintptr_t address, LPVOID detour, LPVOID* original);
    bool HookVtable(void* class_instance, int index, void* detour, void** original, std::string module);

    bool initialize();
    void shutdown();

    inline C_CSPlayerPawn* (__fastcall* oGetLocalPlayer)(int);
    inline void* (__fastcall* ogGetBaseEntity)(void*, int);
}