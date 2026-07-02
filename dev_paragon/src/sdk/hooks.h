#pragma once
#include "../../external/minhook/include/MinHook.h"
#include "../sdk/memory/memory.h"
#include <string>
#include <unordered_map>
#include <Windows.h>
#include <iostream>
#include "math/vector4d.h"
#include "Interfaces/c_scene_object.h"
#include <d3d11.h>
#include <dxgi.h>
#include "../../external/imgui/imgui.h"
#include "../../external/imgui/imgui_impl_win32.h"
#include "../../external/imgui/imgui_impl_dx11.h"
#include "../../globals.h"
#include <cstdint>

typedef HRESULT(WINAPI* Present_t)(IDXGISwapChain*, UINT, UINT);
typedef __int64(__fastcall* ResizeBuffers_t)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
typedef HRESULT(WINAPI* CreateSwapChain_t)(IDXGIFactory* factory, IUnknown* device, DXGI_SWAP_CHAIN_DESC* desc, IDXGISwapChain** swap_chain);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Hook
{
	bool Hook(const char* pattern, LPVOID detour, LPVOID* origina, std::string module);
	bool HookAddress(uintptr_t address, LPVOID detour, LPVOID* original);

	bool initialize();

	void Shutdown();

	bool HookVtable(void* class_instance, int index, void* detour, void** original, std::string module);

}