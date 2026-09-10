#include "hooks.h"

__int64 __fastcall hkLevelInit(__int64 a1, __int64 a2)
{
    __int64 original = olevelInit(a1, a2);
    I::pvs->set(false);

    return original;
}

bool hkCreateMove(CCSGOInput* pInput, int nSlot, CUserCmd* pCmd)
{
    silentaim::runSilentAim(pInput, pCmd);

    return oCreateMove(pInput, nSlot, pCmd);
}

void __fastcall hkFrameStageNotify(void* a1, int stage)
{

    switch (stage)
    {
    case FRAME_RENDER_START:
        break;
    case FRAME_RENDER_END:
        ModelChanger::ChangeModelNow();
        break;
    case FRAME_NET_UPDATE_END:
        break;
    case FRAME_SIMULATE_END:
        break;
    default:
        break;
    }
    oFrameStageNotify(a1, stage);
}

bool Hook::HookAddress(void* address, LPVOID detour, LPVOID* original) {
    if (!address) return false;

    if (MH_CreateHook(reinterpret_cast<LPVOID>(address), detour, original) != MH_OK)
        return false;

    return MH_EnableHook(reinterpret_cast<LPVOID>(address)) == MH_OK;
}

bool Hook::Hook(const char* pattern, LPVOID detour, LPVOID* original, std::string module) {
    uintptr_t addr = Memory::pattern_scan(pattern, module);
    if (!addr) {
        std::cout << "[ERROR] Pattern failed: " << module << " -> " << pattern << std::endl;
        return false;
    }
    return HookAddress((void*)addr, detour, original);
}

template< typename T >
__forceinline T get_method(void* thisptr, uintptr_t idx)
{
    return reinterpret_cast<T>((*static_cast<uintptr_t**>(thisptr))[idx]);
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
        std::cout << "[-] MinHook init failed: " << MH_StatusToString(status) << std::endl;
        return false;
    }

    fnSetModel = (void* (*)(void*, const char*))Memory::pattern_scan("40 53 48 83 EC ? 48 8B D9 4C 8B C2 48 8B 0D ? ? ? ? 48 8D 54 24", "client.dll");
    fnPrecache = (void* (*)(void*, void*, const char*))Memory::pattern_scan("40 53 55 57 48 81 EC 80 00 00 00 48 8B 01 49 8B E8 48 8B FA", "resourcesystem.dll");

    oGetLocalPlayer = reinterpret_cast<decltype(oGetLocalPlayer)>(Memory::getAbsoluteAddress(
        Memory::pattern_scan("E8 ? ? ? ? 4C 8B E0 48 85 C0 0F 84 ? ? ? ? ? ? ? 48 8B C8 FF 92 ? ? ? ? 84 C0 0F 84 ? ? ? ? ? ? ? ? 49 8B CC", "client.dll"), 1));
    ogGetBaseEntity = reinterpret_cast<decltype(ogGetBaseEntity)>(Memory::pattern_scan("4C 8D 49 ? 81 FA", "client.dll"));
   
    Hook("48 89 54 24 ?? 55 57 41 56 48 83 EC", WorldModulation::hkDrawArrayLight,
        (void**)&WorldModulation::oDrawArrayLight, "scenesystem.dll");
    Hook("48 8B C4 48 89 50 ? 48 89 48 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70", WorldModulation::hkWallColor,
        (void**)&WorldModulation::oWallColor, "scenesystem.dll");
    Hook("40 53 48 83 EC ? 48 8B D9 0F 29 74 24", WorldModulation::hk_tonemap_render_debug,
        (void**)&WorldModulation::oToneMap, "scenesystem.dll");
    Hook("45 85 C9 0F 8E ? ? ? ? 4C 8B DC 55", WorldModulation::hkSkyboxArray,
        (void**)&WorldModulation::oSkyboxArray, "scenesystem.dll");
    Hook("40 57 48 83 EC ? 48 8B FA E8 ? ? ? ? BA", OverrideView::hkOverrideView,
        (void**)&OverrideView::oOverrideView, "client.dll");
    Hook("48 89 5C 24 08 57 48 83 EC 20 48 8B D9 E8 ? ? ? ? 48 85", OverrideView::hkDrawCrosshair,
        (void**)&OverrideView::oDrawCrosshair, "client.dll");
    Hook("40 55 53 56 41 56 41 57 48 8B EC", Viewmodel::hkViewModel,
        (void**)&Viewmodel::oViewModel, "client.dll");
    Hook("48 89 54 24 ? 55 41 54 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 4C 8B E2", Removals::hkDrawSmoke,
        (void**)&Removals::oDrawSmoke, "client.dll");
    Hook("48 8B C4 53 57 48 83 EC ? 48 8B FA", Removals::hkDrawScopeOverlay,
        (void**)&Removals::oDrawScopeOverlay, "client.dll");
    Hook("40 55 53 56 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? F2 0F 10 42", Removals::hkDrawLegs,
        (void**)&Removals::oDrawLegs, "client.dll");
    Hook("85 D2 0F 88 ? ? ? ? 48 89 4C 24 ? 55 56", Removals::hkFlashbangOverlay,
        (void**)&Removals::oFlashbangOverlay, "client.dll");
    Hook("40 53 48 83 EC 20 80 B9 ? ? ? ? ? 48 8B D9 75 78", D::hkMouseInputEnabled, 
        (void**)&D::oMouseInputEnabled, "client.dll");
    Hook("48 89 4C 24 ? 55 41 54 48 8D 6C 24", radarhack::hkRadar,
        (void**)&radarhack::oRadar, "client.dll");
    Hook("48 89 74 24 ? 57 48 83 EC ? 48 8B 0D ? ? ? ? 48 8B FA", hkLevelInit,
        (void**)&olevelInit, "client.dll");
    Hook("48 89 5C 24 ?? 48 89 6C 24 ?? 57 48 83 EC ?? 48 8B F9 33 ED", hkFrameStageNotify,
        (void**)&oFrameStageNotify, "client.dll");
    //HookAddress(get_method<void*>(I::scene_system->get_scene_object_desc("AnimatableSceneObjectDesc"), 4), 
    //    chams::hkOnGeneratePrimitives, (void**)&chams::oOnGeneratePrimitives);
    HookVtable(I::input_system, 76, D::hkIsRelativeMouseMode, 
        (void**)&D::oIsRelativeMouseMode, "inputsystem.dll");
    Hook("48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 54 41 56 41 57 48 83 EC ? 41 8B F0", D::hkPresent,
        (void**)&D::g_pOriginalPresent, "GameOverlayRenderer64.dll");
    //Hook("48 89 5C 24 08 48 89 6C 24 10 56 57 41 54 41 56 41 57 48 83 EC 20 41 8B", D::hkResizeBuffers,
    //    (void**)&D::g_pResizeBuffers, "GameOverlayRenderer64.dll");
    Hook("48 8B C4 4C 89 40 18 48 89 48 08 55 53 41 54 41 55", hkCreateMove, 
        (void**)&oCreateMove, "client.dll");

    Material::material_init();
    ModelChanger::ScanModels();

    std::cout << "[+] All hooks installed successfully!" << std::endl;
    return true;
}

void Hook::shutdown()
{
    D::CleanupImGui();
    MH_DisableHook(MH_ALL_HOOKS);
    MH_RemoveHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}