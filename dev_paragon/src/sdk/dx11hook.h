#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <iostream>
#include <wrl.h>
#include <array>

#include "../../external/imgui/imgui.h"
#include "../../external/imgui/imgui_impl_win32.h"
#include "../../external/imgui/imgui_impl_dx11.h"
#include "../sdk/Interfaces/Interfaces.h"
#include "../../globals.h"

using Microsoft::WRL::ComPtr;

typedef HRESULT(WINAPI* Present_t)(IDXGISwapChain*, UINT, UINT);
typedef __int64(__fastcall* ResizeBuffers_t)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
typedef LRESULT(CALLBACK* WNDPROC_t)(HWND, UINT, WPARAM, LPARAM);
typedef bool(__fastcall* IsRelativeMouseMode_t)(void*, bool);
typedef bool(__fastcall* MouseInputEnabled_t)(void*);

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace D
{
    extern ID3D11Device* g_pDevice;
    extern ID3D11DeviceContext* g_pContext;
    extern ID3D11RenderTargetView* g_pRenderTargetView;
    extern HWND g_hWindow;
    extern bool g_bInitialized;

    extern Present_t g_pOriginalPresent;
    extern ResizeBuffers_t g_pResizeBuffers;
    extern WNDPROC_t g_pOriginalWndProc;

    extern IsRelativeMouseMode_t oIsRelativeMouseMode;
    extern MouseInputEnabled_t oMouseInputEnabled;

    LRESULT CALLBACK WndProcHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    HRESULT WINAPI hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
    __int64 __fastcall hkResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
    
    void CleanupImGui();
    bool __fastcall hkIsRelativeMouseMode(void* ecx, bool enable);
    bool __fastcall hkMouseInputEnabled(void* ecx);
}