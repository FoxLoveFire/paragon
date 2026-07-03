#pragma once

#include "../../external/imgui/imgui.h"
#include "../../external/imgui/imgui_impl_win32.h"
#include "../../external/imgui/imgui_impl_dx11.h"
#include <d3d11.h>
#include <dxgi.h>
#include <Windows.h>
#include "../../globals.h"
#include "Interfaces/Interfaces.h"
#include <cstdint>
#include <optional>
#include <array>

#pragma comment(lib, "d3d11.lib")

using Present_t = HRESULT(WINAPI*)(IDXGISwapChain*, UINT, UINT);
using ResizeBuffers_t = __int64(__fastcall*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
using CreateSwapChain_t = HRESULT(WINAPI*)(IDXGIFactory*, IUnknown*, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**);
using WNDPROC_t = LRESULT(CALLBACK*)(HWND, UINT, WPARAM, LPARAM);
using IsRelativeMouseMode_t = bool(__fastcall*)(void* ecx, bool enable);
using MouseInputEnabled_t = bool(__fastcall*)(void* ecx);

template<typename T>
class ComPtr {
private:
    T* m_pPtr = nullptr;

public:
    ComPtr() = default;
    explicit ComPtr(T* ptr) noexcept : m_pPtr(ptr) {}
    ~ComPtr() { Reset(); }

    ComPtr(const ComPtr&) = delete;
    ComPtr& operator=(const ComPtr&) = delete;

    ComPtr(ComPtr&& other) noexcept : m_pPtr(other.m_pPtr) {
        other.m_pPtr = nullptr;
    }

    ComPtr& operator=(ComPtr&& other) noexcept {
        if (this != &other) {
            Reset();
            m_pPtr = other.m_pPtr;
            other.m_pPtr = nullptr;
        }
        return *this;
    }

    void Reset(T* ptr = nullptr) {
        if (m_pPtr) {
            m_pPtr->Release();
            m_pPtr = nullptr;
        }
        m_pPtr = ptr;
    }

    [[nodiscard]] T* Get() const { return m_pPtr; }
    [[nodiscard]] T** GetAddressOf() { return &m_pPtr; }
    [[nodiscard]] void** GetVoidAddressOf() { return reinterpret_cast<void**>(&m_pPtr); }
    [[nodiscard]] T* operator->() const { return m_pPtr; }
    [[nodiscard]] explicit operator bool() const { return m_pPtr != nullptr; }
};

namespace D
{
    LRESULT CALLBACK WndProcHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    HRESULT WINAPI hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
    __int64 __fastcall hkResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
    void CleanupImGui();
    uintptr_t GetPresentAddress();
    uintptr_t GetResizeBuffersAddress();
    bool __fastcall hkIsRelativeMouseMode(void* ecx, bool enable);
    bool __fastcall hkMouseInputEnabled(void* ecx);

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
}