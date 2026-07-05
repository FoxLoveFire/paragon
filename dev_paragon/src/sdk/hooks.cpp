#include "hooks.h"
#include "../sdk/Interfaces/c_scene_object.h"

#define PRESENT_PATTERN "48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 54 41 56 41 57 48 83 EC ? 41 8B E8"
#define RESIZE_BUFFER_PATTERN "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 54 41 56 41 57 48 83 EC ? 44 8B E2"

bool Hook::Hook(const char* pattern, LPVOID detour, LPVOID* original, std::string module) {
    uintptr_t addr = Memory::pattern_scan(pattern, module);
    if (!addr) {
        return false;
    }
    return HookAddress(addr, detour, original);
}

bool Hook::HookAddress(uintptr_t address, LPVOID detour, LPVOID* original) {
    if (!address) return false;
    if (MH_CreateHook(reinterpret_cast<LPVOID>(address), detour, original) != MH_OK)
        return false;
    return MH_EnableHook(reinterpret_cast<LPVOID>(address)) == MH_OK;
}

bool Hook::HookVtable(void* class_instance, int index, void* detour, void** original, std::string module) {
    if (!class_instance) return false;
    void** vtable = *reinterpret_cast<void***>(class_instance);
    if (!vtable || !vtable[index]) return false;
    return MH_CreateHook(vtable[index], detour, original) == MH_OK;
}

bool Hook::initialize()
{
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED) {
        return false;
    }

    oGetLocalPlayer = reinterpret_cast<decltype(oGetLocalPlayer)>(Memory::getAbsoluteAddress(
        Memory::pattern_scan("E8 ? ? ? ? 4C 8B E0 48 85 C0 0F 84 ? ? ? ? ? ? ? 48 8B C8 FF 92 ? ? ? ? 84 C0 0F 84 ? ? ? ? ? ? ? ? 49 8B CC", "client.dll"), 1));

    Hook("48 89 54 24 ?? 55 57 41 56 48 83 EC", WorldModulation::hkDrawArrayLight, (void**)&WorldModulation::oDrawArrayLight, "scenesystem.dll");
    Hook("48 8B C4 48 89 50 ? 48 89 48 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70", WorldModulation::hkWallColor, (void**)&WorldModulation::oWallColor, "scenesystem.dll");
    Hook("40 53 48 83 EC ? 48 8B D9 0F 29 74 24", WorldModulation::hk_tonemap_render_debug, (void**)&WorldModulation::oToneMap, "scenesystem.dll");
    
    Hook("40 57 48 83 EC ? 48 8B FA E8 ? ? ? ? BA", OverrideView::hkOverrideView, (void**)&OverrideView::oOverrideView, "client.dll");
    Hook("40 55 53 56 41 56 41 57 48 8B EC", Viewmodel::hkViewModel, (void**)&Viewmodel::oViewModel, "client.dll");
    
    Hook("40 55 41 54 41 55 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 4C 8B E2", Removals::hkDrawSmoke, (void**)&Removals::oDrawSmoke, "client.dll");
    Hook("48 8B C4 53 57 48 83 EC ? 48 8B FA", Removals::hkDrawScopeOverlay, (void**)&Removals::oDrawScopeOverlay, "client.dll");
    Hook("40 55 53 56 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? F2 0F 10 42", Removals::oDrawLegs, (void**)&Removals::oDrawLegs, "client.dll");
    Hook("40 55 53 56 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? F2 0F 10 42", Removals::hkFlashbangOverlay, (void**)&Removals::oFlashbangOverlay, "client.dll");
    
    Hook("40 53 48 83 EC 20 80 B9 ? ? ? ? ? 48 8B D9 75 78", D::hkMouseInputEnabled, (void**)&D::oMouseInputEnabled, "client.dll");
    Hook("48 89 4C 24 ? 55 41 54 48 8D 6C 24", radarhack::hkRadar, (void**)&radarhack::oRadar, "client.dll");
    HookVtable(I::input_system, 76, D::hkIsRelativeMouseMode, (void**)&D::oIsRelativeMouseMode, "inputsystem.dll");

    if (!Hook(PRESENT_PATTERN, D::hkPresent, reinterpret_cast<void**>(&D::g_pOriginalPresent), "GameOverlayRenderer64.dll")) {
        return false;
    }

    if (!Hook(RESIZE_BUFFER_PATTERN, D::hkResizeBuffers, reinterpret_cast<void**>(&D::g_pResizeBuffers), "GameOverlayRenderer64.dll")) {
        return false;
    }

    return true;
}

void Hook::shutdown()
{
    D::CleanupImGui();
    MH_DisableHook(MH_ALL_HOOKS);
    MH_RemoveHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}