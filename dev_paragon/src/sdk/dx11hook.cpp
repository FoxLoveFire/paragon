#include "dx11hook.h"

ID3D11Device* D::g_pDevice = nullptr;
ID3D11DeviceContext* D::g_pContext = nullptr;
ID3D11RenderTargetView* D::g_pRenderTargetView = nullptr;
HWND D::g_hWindow = nullptr;
bool D::g_bInitialized = false;

Present_t D::g_pOriginalPresent = nullptr;
ResizeBuffers_t D::g_pResizeBuffers = nullptr;
WNDPROC_t D::g_pOriginalWndProc = nullptr;

IsRelativeMouseMode_t D::oIsRelativeMouseMode = nullptr;
MouseInputEnabled_t D::oMouseInputEnabled = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace D
{
    LRESULT CALLBACK WndProcHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (uMsg == WM_KEYDOWN && wParam == VK_INSERT)
        {
            G::menu.show = !G::menu.show;

            if (G::menu.show) {
                ShowCursor(TRUE);
                oIsRelativeMouseMode(I::input_system, false);
            }
            else {
                if (I::engine_client->is_in_game())
                {
                    ShowCursor(FALSE);
                    oIsRelativeMouseMode(I::input_system, true);
                    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
                    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
                    SetCursorPos(screenWidth / 2, screenHeight / 2);
                }
            }
        }

        if (G::menu.show)
        {
            ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

            switch (uMsg) {
            case WM_MOUSEMOVE:
            case WM_NCMOUSEMOVE:
            case WM_LBUTTONDOWN: case WM_LBUTTONUP:
            case WM_RBUTTONDOWN: case WM_RBUTTONUP:
            case WM_MBUTTONDOWN: case WM_MBUTTONUP:
            case WM_XBUTTONDOWN: case WM_XBUTTONUP:
            case WM_MOUSEWHEEL:
                return 1L;
            }

            const ImGuiIO& io = ImGui::GetIO();
            if (io.WantCaptureKeyboard) {
                switch (uMsg) {
                case WM_KEYDOWN:    case WM_KEYUP:
                case WM_SYSKEYDOWN: case WM_SYSKEYUP:
                case WM_CHAR:
                    return 1L;
                }
            }
        }

        return CallWindowProc(g_pOriginalWndProc, hWnd, uMsg, wParam, lParam);
    }

    HRESULT WINAPI hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
    {
        if (!g_bInitialized) {
            if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&g_pDevice)))) {
                g_pDevice->GetImmediateContext(&g_pContext);

                DXGI_SWAP_CHAIN_DESC desc{};
                pSwapChain->GetDesc(&desc);
                g_hWindow = desc.OutputWindow;

                ComPtr<ID3D11Texture2D> pBackBuffer;
                if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), pBackBuffer.GetVoidAddressOf()))) {
                    g_pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &g_pRenderTargetView);
                }

                IMGUI_CHECKVERSION();
                ImGui::CreateContext();
                ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
                ImGui::StyleColorsDark();

                ImGui_ImplWin32_Init(g_hWindow);
                ImGui_ImplDX11_Init(g_pDevice, g_pContext);

                Menu::Initialize();

                g_pOriginalWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(g_hWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProcHandler)));
                g_bInitialized = true;
            }
        }

        if (!g_pRenderTargetView) {
            ComPtr<ID3D11Texture2D> pBackBuffer;
            if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), pBackBuffer.GetVoidAddressOf()))) {
                g_pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &g_pRenderTargetView);
            }
        }

        if (g_pRenderTargetView && g_pContext) {
            ComPtr<ID3D11RenderTargetView> pOldRTV;
            ComPtr<ID3D11DepthStencilView> pOldDSV;
            g_pContext->OMGetRenderTargets(1, pOldRTV.GetAddressOf(), pOldDSV.GetAddressOf());

            std::array<D3D11_VIEWPORT, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE> oldVP{};
            UINT numViewports = oldVP.size();
            g_pContext->RSGetViewports(&numViewports, oldVP.data());

            g_pContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);

            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            if (G::menu.show) {
                Menu::Render();
            }

            ImGui::Render();
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            g_pContext->RSSetViewports(numViewports, oldVP.data());
            g_pContext->OMSetRenderTargets(1, pOldRTV.GetAddressOf(), pOldDSV.Get());
        }

        return g_pOriginalPresent(pSwapChain, SyncInterval, Flags);
    }

    __int64 __fastcall hkResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
    {
        if (g_pRenderTargetView) {
            g_pRenderTargetView->Release();
            g_pRenderTargetView = nullptr;
        }
        ImGui_ImplDX11_InvalidateDeviceObjects();
        auto result = g_pResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);

        if (SUCCEEDED(result))
        {
            ID3D11Texture2D* backBuffer = nullptr;
            if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer)))
            {
                g_pDevice->CreateRenderTargetView(backBuffer, nullptr, &g_pRenderTargetView);
                backBuffer->Release();
            }
            ImGui_ImplDX11_CreateDeviceObjects();
        }

        return result;
    }

    uintptr_t GetPresentAddress()
    {
        HWND hWnd = GetForegroundWindow();
        if (!hWnd) hWnd = GetDesktopWindow();

        DXGI_SWAP_CHAIN_DESC scDesc{};
        scDesc.BufferCount = 1;
        scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scDesc.OutputWindow = hWnd;
        scDesc.SampleDesc.Count = 1;
        scDesc.Windowed = TRUE;

        constexpr D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

        ComPtr<IDXGISwapChain> pDummySwapChain;
        ComPtr<ID3D11Device> pDummyDevice;
        ComPtr<ID3D11DeviceContext> pDummyContext;

        if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, featureLevels, 1, D3D11_SDK_VERSION, &scDesc, pDummySwapChain.GetAddressOf(), pDummyDevice.GetAddressOf(), nullptr, pDummyContext.GetAddressOf()))) {
            return 0;
        }

        void** pVTable = *reinterpret_cast<void***>(pDummySwapChain.Get());
        return reinterpret_cast<uintptr_t>(pVTable[8]);
    }

    uintptr_t GetResizeBuffersAddress()
    {
        HWND hWnd = GetForegroundWindow();
        if (!hWnd) hWnd = GetDesktopWindow();

        DXGI_SWAP_CHAIN_DESC scDesc{};
        scDesc.BufferCount = 1;
        scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scDesc.OutputWindow = hWnd;
        scDesc.SampleDesc.Count = 1;
        scDesc.Windowed = TRUE;

        constexpr D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

        ComPtr<IDXGISwapChain> pDummySwapChain;
        ComPtr<ID3D11Device> pDummyDevice;
        ComPtr<ID3D11DeviceContext> pDummyContext;

        if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, featureLevels, 1, D3D11_SDK_VERSION, &scDesc, pDummySwapChain.GetAddressOf(), pDummyDevice.GetAddressOf(), nullptr, pDummyContext.GetAddressOf()))) {
            return 0;
        }

        void** pVTable = *reinterpret_cast<void***>(pDummySwapChain.Get());

        if (!pVTable[13]) {
            for (int i = 0; i < 20; i++) {
                if (pVTable[i]) return 0;
            }
            return 0;
        }

        return reinterpret_cast<uintptr_t>(pVTable[13]);
    }

    void CleanupImGui()
    {
        if (!g_bInitialized) return;

        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        if (g_hWindow && g_pOriginalWndProc) {
            SetWindowLongPtr(g_hWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(g_pOriginalWndProc));
            g_pOriginalWndProc = nullptr;
            g_hWindow = nullptr;
        }

        if (g_pRenderTargetView) {
            g_pRenderTargetView->Release();
            g_pRenderTargetView = nullptr;
        }

        if (g_pContext) {
            g_pContext->Release();
            g_pContext = nullptr;
        }

        if (g_pDevice) {
            g_pDevice->Release();
            g_pDevice = nullptr;
        }

        g_bInitialized = false;
    }

    bool __fastcall hkIsRelativeMouseMode(void* ecx, bool enable)
    {
        if (G::menu.show) return oIsRelativeMouseMode(ecx, false);
        return oIsRelativeMouseMode(ecx, enable);
    }

    bool __fastcall hkMouseInputEnabled(void* ecx)
    {
        if (G::menu.show) return false;
        return oMouseInputEnabled(ecx);
    }
}