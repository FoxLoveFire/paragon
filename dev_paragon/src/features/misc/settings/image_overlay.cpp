#include "image_overlay.h"
#include "../../../sdk/menu/menu.h"
#include "../../../../external/imgui/imgui.h"
#include "../../../../external/imgui/imgui_internal.h"
#include <wincodec.h>
#include <urlmon.h>
#include <vector>
#include <d3d11.h>
#pragma comment(lib, "urlmon.lib")

namespace D { extern ID3D11Device* g_pDevice; }

namespace Features::Settings::ImageOverlay {
    static ID3D11ShaderResourceView* g_pOverlayTexture = nullptr;
    static int g_OverlayWidth = 0;
    static int g_OverlayHeight = 0;

    static std::wstring StringToWString(const std::string& str) {
        if (str.empty()) return L"";
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
        if (size_needed <= 0) {
            size_needed = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), NULL, 0);
            std::wstring wstrTo(size_needed, 0);
            MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
            return wstrTo;
        }
        std::wstring wstrTo(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
        return wstrTo;
    }

    static bool LoadTextureFromFileOrURL(const std::string& path, ID3D11Device* d3dDevice, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height) {
        if (!d3dDevice || path.empty()) return false;

        std::wstring wpath;
        if (path.rfind("http", 0) == 0) {
            wchar_t tempPath[MAX_PATH];
            GetTempPathW(MAX_PATH, tempPath);
            wchar_t tempFile[MAX_PATH];
            GetTempFileNameW(tempPath, L"img", 0, tempFile);
            
            std::wstring wurl = StringToWString(path);
            if (FAILED(URLDownloadToFileW(NULL, wurl.c_str(), tempFile, 0, NULL))) {
                return false;
            }
            wpath = tempFile;
        } else {
            wpath = StringToWString(path);
        }

        HRESULT hr_co = CoInitialize(NULL);

        IWICImagingFactory* pFactory = nullptr;
        if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFactory)))) {
            if (SUCCEEDED(hr_co)) CoUninitialize();
            return false;
        }

        IWICBitmapDecoder* pDecoder = nullptr;
        if (FAILED(pFactory->CreateDecoderFromFilename(wpath.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pDecoder))) {
            pFactory->Release();
            if (SUCCEEDED(hr_co)) CoUninitialize();
            return false;
        }

        IWICBitmapFrameDecode* pFrame = nullptr;
        if (FAILED(pDecoder->GetFrame(0, &pFrame))) {
            pDecoder->Release();
            pFactory->Release();
            if (SUCCEEDED(hr_co)) CoUninitialize();
            return false;
        }

        UINT width = 0, height = 0;
        pFrame->GetSize(&width, &height);

        IWICFormatConverter* pConverter = nullptr;
        if (FAILED(pFactory->CreateFormatConverter(&pConverter))) {
            pFrame->Release();
            pDecoder->Release();
            pFactory->Release();
            if (SUCCEEDED(hr_co)) CoUninitialize();
            return false;
        }

        if (FAILED(pConverter->Initialize(pFrame, GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, NULL, 0.0, WICBitmapPaletteTypeCustom))) {
            pConverter->Release();
            pFrame->Release();
            pDecoder->Release();
            pFactory->Release();
            if (SUCCEEDED(hr_co)) CoUninitialize();
            return false;
        }

        UINT rowPitch = width * 4;
        UINT imageSize = rowPitch * height;
        std::vector<unsigned char> pixels(imageSize);
        if (FAILED(pConverter->CopyPixels(NULL, rowPitch, imageSize, pixels.data()))) {
            pConverter->Release();
            pFrame->Release();
            pDecoder->Release();
            pFactory->Release();
            if (SUCCEEDED(hr_co)) CoUninitialize();
            return false;
        }

        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA subResource = {};
        subResource.pSysMem = pixels.data();
        subResource.SysMemPitch = rowPitch;

        ID3D11Texture2D* pTexture = nullptr;
        HRESULT hr = d3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);
        if (SUCCEEDED(hr)) {
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Format = desc.Format;
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = 1;

            hr = d3dDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
            pTexture->Release();
        }

        pConverter->Release();
        pFrame->Release();
        pDecoder->Release();
        pFactory->Release();
        
        if (SUCCEEDED(hr_co)) CoUninitialize();

        if (SUCCEEDED(hr)) {
            *out_width = (int)width;
            *out_height = (int)height;
            return true;
        }
        return false;
    }

    void ClearTexture() {
        if (g_pOverlayTexture) {
            g_pOverlayTexture->Release();
            g_pOverlayTexture = nullptr;
        }
        g_OverlayWidth = 0;
        g_OverlayHeight = 0;
    }

    void HandleApply() {
        if (Menu::cfg["custom_overlay_apply"].b) {
            ClearTexture();
            std::string path = "";
            if (Menu::cfg["custom_overlay_type"].i == 0) {
                path = Menu::cfg["custom_overlay_url"].s;
            } else {
                const char* k_images_dir = "C:\\paragon\\images\\";
                path = std::string(k_images_dir) + Menu::cfg["custom_overlay_file"].s;
            }
            if (!path.empty()) {
                LoadTextureFromFileOrURL(path, D::g_pDevice, &g_pOverlayTexture, &g_OverlayWidth, &g_OverlayHeight);
            }
            Menu::cfg["custom_overlay_apply"].b = false;
        }
    }

    void Render() {
        if (!G::menu.show) return;

        if (Menu::cfg["custom_overlay"].b && g_pOverlayTexture) {
            float scale = Menu::cfg["custom_overlay_scale"].f;
            if (scale <= 0.0f) scale = 1.0f;
            
            float alpha = Menu::cfg["custom_overlay_alpha"].f;
            if (alpha <= 0.0f) alpha = 1.0f;

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            
            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
            
            ImGui::Begin("OverlayImage", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);
            
            ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
            
            ImGui::Image((void*)g_pOverlayTexture, ImVec2((float)g_OverlayWidth * scale, (float)g_OverlayHeight * scale), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1.0f, 1.0f, 1.0f, alpha), ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
            
            ImGui::End();
            ImGui::PopStyleVar(2);
        }
    }
}