#include "hooks.h"

Present_t oPresent = nullptr;
ResizeBuffers_t oResizeBuffers = nullptr;
CreateSwapChain_t oCreateSwapChain;
WNDPROC oWndProc = nullptr;

ID3D11Device* pDevice = nullptr;
ID3D11DeviceContext* pContext = nullptr;
ID3D11RenderTargetView* pRenderTargetView = nullptr;
IDXGISwapChain* swap_chain = nullptr;

DWORD_PTR* pSwapChainVTable = nullptr;

bool initialized = false;
HWND window = nullptr;

LRESULT CALLBACK hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

void ImGuiInitialize(IDXGISwapChain* pSwapChain)
{
	HRESULT hr = pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice);
	if (FAILED(hr)) {
		return;
	}

	pDevice->GetImmediateContext(&pContext);

	DXGI_SWAP_CHAIN_DESC desc;
	pSwapChain->GetDesc(&desc);
	window = desc.OutputWindow;

	ID3D11Texture2D* pBackBuffer = nullptr;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	if (pBackBuffer) {
		pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView);
		pBackBuffer->Release();
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);

	oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)hkWndProc);

	initialized = true;
}

void CleanupRenderTarget()
{
	if (pRenderTargetView) {
		pRenderTargetView->Release();
		pRenderTargetView = nullptr;
	}
}

void CreateRenderTarget(IDXGISwapChain* pSwapChain)
{
	ID3D11Texture2D* pBackBuffer = nullptr;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	if (pBackBuffer) {
		pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView);
		pBackBuffer->Release();
	}
}

void CleanupImGui()
{
	if (!initialized) {
		return;
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if (window && oWndProc) {
		SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)oWndProc);
	}

	CleanupRenderTarget();

	if (pContext) {
		pContext->Release();
		pContext = nullptr;
	}

	if (pDevice) {
		pDevice->Release();
		pDevice = nullptr;
	}

	initialized = false;
}

HRESULT WINAPI hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
	if (!initialized) {
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice))) {
			pDevice->GetImmediateContext(&pContext);

			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			ImGuiInitialize(pSwapChain);
			initialized = true;
		}
		else {
			return oPresent(pSwapChain, SyncInterval, Flags);
		}
	}

	if (pRenderTargetView == nullptr) {
		ID3D11Texture2D* pBackBuffer = nullptr;
		if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer))) {
			pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView);
			pBackBuffer->Release();
		}
	}

	if (G::menu.show && pRenderTargetView && pContext) {
		ID3D11RenderTargetView* old_rtv = nullptr;
		ID3D11DepthStencilView* old_dsv = nullptr;
		pContext->OMGetRenderTargets(1, &old_rtv, &old_dsv);

		D3D11_VIEWPORT old_vp[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
		UINT nv = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
		pContext->RSGetViewports(&nv, old_vp);

		pContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::ShowDemoWindow();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		pContext->RSSetViewports(nv, old_vp);
		pContext->OMSetRenderTargets(1, &old_rtv, old_dsv);

		if (old_rtv) old_rtv->Release();
		if (old_dsv) old_dsv->Release();
	}

	return oPresent(pSwapChain, SyncInterval, Flags);
}

typedef void(__fastcall* DrawArrayLightFn)(__int64 a1, CSceneLightObject* a2, __int64 a3);
inline  DrawArrayLightFn oDrawArrayLight = nullptr;

void __fastcall DrawArrayLight(__int64 a1, CSceneLightObject* a2, __int64 a3)
{
	if (!a2)
		return;

	a2->desc.colorR = 255;
	a2->desc.colorG = 0;
	a2->desc.colorB = 0;

	if (oDrawArrayLight)
	{
		oDrawArrayLight(a1, a2, a3);
	}
}

bool Hook::Hook(const char* pattern, LPVOID detour, LPVOID* original, std::string module)
{
	uintptr_t addr = Memory::pattern_scan(pattern, module);

	if (!addr) {
		std::cout << "Failed to get pattern! Moudle: " + module + " -> " + "Pattern: " + pattern << std::endl;
	}

	MH_STATUS status = MH_CreateHook(reinterpret_cast<LPVOID>(addr), detour, original);

	if (MH_OK != status) {
		return false;
	}

	return MH_EnableHook(reinterpret_cast<LPVOID>(addr)) == MH_OK;
}

bool Hook::HookAddress(uintptr_t address, LPVOID detour, LPVOID* original)
{
	if (!address) {
		std::cout <<"Failed to hook: Address is null!"<< std::endl;
		return false;
	}

	MH_STATUS status = MH_CreateHook(reinterpret_cast<LPVOID>(address), detour, original);

	if (MH_OK != status) {
		return false;
	}

	return MH_EnableHook(reinterpret_cast<LPVOID>(address)) == MH_OK;
}

bool Hook::HookVtable(void* class_instance, int index, void* detour, void** original, std::string module)
{
	if (!class_instance) {
		std::cout << "[FAILED] " + module + " | class instance is null\n" << std::endl;
		return false;
	}

	void** vtable = *reinterpret_cast<void***>(class_instance);
	if (!vtable) {
		std::cout << "[FAILED] " + module + " | vtable is null\n" << std::endl;
		return false;
	}

	void* addr = vtable[index];
	if (!addr) {
		std::cout << "[FAILED] " + module + " | vtable is null\n" << std::endl;
		return false;
	}

	return MH_CreateHook(addr, detour, original) == MH_OK;
}

bool Hook::initialize()
{
	MH_STATUS status = MH_Initialize();
	if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED) {

		std::cout << "[-] MinHook initialize failed: " << MH_StatusToString(status) << std::endl;
		return false;
	}

	Hook("48 89 54 24 ?? 55 57 41 56 48 83 EC", DrawArrayLight, (void**)&oDrawArrayLight, "scenesystem.dll");

	HookVtable(swap_chain, 8, hkPresent, (void**)&oPresent, "GameOverlayRenderer64.dll");
	return true;
}

void Hook::Shutdown()
{
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}