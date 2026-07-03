#include "hooks.h"
#include "../sdk/Interfaces/c_scene_object.h"

bool Hook::Hook(const char* pattern, LPVOID detour, LPVOID* original, std::string module) {
    uintptr_t addr = Memory::pattern_scan(pattern, module);
    if (!addr) {
        std::cout << "[ERROR] Pattern failed: " << module << " -> " << pattern << std::endl;
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
        std::cout << "[-] MinHook init failed: " << MH_StatusToString(status) << std::endl;
        return false;
    }

    Hook("48 89 54 24 ?? 55 57 41 56 48 83 EC", WorldModulation::hkDrawArrayLight,
        (void**)&WorldModulation::oDrawArrayLight, "scenesystem.dll");
    Hook("48 8B C4 48 89 50 ? 48 89 48 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70", WorldModulation::hkWallColor,
        (void**)&WorldModulation::oWallColor, "scenesystem.dll");


    Hook("40 53 48 83 EC 20 80 B9 ? ? ? ? ? 48 8B D9 75 78", D::hkMouseInputEnabled, (void**)&D::oMouseInputEnabled, "client.dll");
    HookVtable(I::input_system, 76, D::hkIsRelativeMouseMode, (void**)&D::oIsRelativeMouseMode, "inputsystem.dll");
    
    uintptr_t presentAddr = D::GetPresentAddress();
    if (!presentAddr) {
        std::cout << "[-] Failed to get Present address" << std::endl;
        return false;
    }

    if (!HookAddress(presentAddr, D::hkPresent, reinterpret_cast<LPVOID*>(&D::g_pOriginalPresent))) {
        std::cout << "[-] Failed to hook Present" << std::endl;
        return false;
    }

    uintptr_t resisebuff = D::GetResizeBuffersAddress();
    if (!resisebuff)
    {
        std::cout << "[-] Failed to get resize buffer address" << std::endl;
        return false;
    }

    if (!HookAddress(resisebuff, D::hkResizeBuffers, reinterpret_cast<LPVOID*>(&D::g_pResizeBuffers)))
    {
        std::cout << "[-] Failed to hook resize buffer" << std::endl;
        return false;
    }

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