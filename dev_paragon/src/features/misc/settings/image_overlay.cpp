#include "image_overlay.h"
#include "../../../sdk/menu/menu.h"
#include "../../../../external/imgui/imgui.h"
#include <wincodec.h>
#include <urlmon.h>
#include <vector>
#include <d3d11.h>

#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "urlmon.lib")

namespace D { extern ID3D11Device* g_pDevice; }

namespace Features::Settings::ImageOverlay {
    static ID3D11ShaderResourceView* g_pOverlayTexture = nullptr;
    static int g_OverlayWidth = 0;
    static int g_OverlayHeight = 0;

    static bool LoadTextureFromFileOrURL(const std::string& path, ID3D11Device* d3dDevice, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height) {
        if (!d3dDevice || path.empty()) return false;

        CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        IWICImagingFactory* pFactory = nullptr;
        
        if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFactory)))) 
            return false;

        std::wstring wpath(path.begin(), path.end());
        IWICBitmapDecoder* pDecoder = nullptr;
        
        if (FAILED(pFactory->CreateDecoderFromFilename(wpath.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder))) {
            pFactory->Release();
            return false;
        }

        IWICBitmapFrameDecode* pFrame = nullptr;
        if (FAILED(pDecoder->GetFrame(0, &pFrame)) || !pFrame) {
            pDecoder->Release();
            pFactory->Release();
            return false;
        }

        IWICFormatConverter* pConverter = nullptr;
        if (FAILED(pFactory->CreateFormatConverter(&pConverter)) || !pConverter) {
            pFrame->Release();
            pDecoder->Release();
            pFactory->Release();
            return false;
        }

        if (FAILED(pConverter->Initialize(pFrame, GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, NULL, 0.0, WICBitmapPaletteTypeCustom))) {
            pConverter->Release();
            pFrame->Release();
            pDecoder->Release();
            pFactory->Release();
            return false;
        }

        UINT width, height;
        pFrame->GetSize(&width, &height);

        std::vector<BYTE> pixels(width * height * 4);
        if (FAILED(pConverter->CopyPixels(NULL, width * 4, static_cast<UINT>(pixels.size()), pixels.data()))) {
            pConverter->Release();
            pFrame->Release();
            pDecoder->Release();
            pFactory->Release();
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

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = pixels.data();
        initData.SysMemPitch = width * 4;

        ID3D11Texture2D* pTexture = nullptr;
        bool success = false;
        if (SUCCEEDED(d3dDevice->CreateTexture2D(&desc, &initData, &pTexture)) && pTexture) {
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = 1;
            
            if (SUCCEEDED(d3dDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv))) {
                success = true;
            }
            pTexture->Release();
        }

        pConverter->Release();
        pFrame->Release();
        pDecoder->Release();
        pFactory->Release();

        if (success) {
            *out_width = width;
            *out_height = height;
            return true;
        }
        return false;
    }

    void ClearTexture() {
        if (g_pOverlayTexture) {
            g_pOverlayTexture->Release();
            g_pOverlayTexture = nullptr;
        }
    }

    void HandleApply() {
        if (Menu::cfg["custom_overlay_apply"].b) {
            ClearTexture();
            std::string path;
            
            if (Menu::cfg["custom_overlay_type"].i == 0) {
                char tempPath[MAX_PATH];
                if (SUCCEEDED(URLDownloadToCacheFileA(NULL, Menu::cfg["custom_overlay_url"].s.c_str(), tempPath, MAX_PATH, 0, NULL))) {
                    path = tempPath;
                }
            } 
            else {
                const char* k_images_dir = "C:\\paragon\\settings\\images\\";
                path = std::string(k_images_dir) + Menu::cfg["custom_overlay_file"].s;
            }
            
            if (!path.empty()) {
                LoadTextureFromFileOrURL(path, D::g_pDevice, &g_pOverlayTexture, &g_OverlayWidth, &g_OverlayHeight);
            }
            Menu::cfg["custom_overlay_apply"].b = false;
        }
    }

    void Render() {
        if (Menu::cfg["custom_overlay"].b && g_pOverlayTexture) {
            float scale = Menu::cfg["custom_overlay_scale"].f;
            if (scale <= 0.0f) scale = 1.0f;
            
            float alpha = Menu::cfg["custom_overlay_alpha"].f;
            if (alpha <= 0.0f && !Menu::cfg["custom_overlay_alpha_init"].b) alpha = 1.0f;

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::Begin("OverlayImage", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);
            
            ImGui::Image((void*)g_pOverlayTexture, ImVec2(g_OverlayWidth * scale, g_OverlayHeight * scale), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, alpha), ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
            
            ImGui::End();
            ImGui::PopStyleVar(2);
        }
    }
}