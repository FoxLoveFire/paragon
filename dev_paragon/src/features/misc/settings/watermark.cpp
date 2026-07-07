#include "watermark.h"
#include "../../../sdk/menu/menu.h"
#include "../../../../external/imgui/imgui.h"
#include "steam_api.h" 
#include <string>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include "../../../sdk/Interfaces/Interfaces.h"

namespace Settings {
    namespace Watermark {

        ISteamFriends* g_pSteamFriends = nullptr;

        std::string GetSteamName() {
            if (!g_pSteamFriends) {
                g_pSteamFriends = GetSteamFriendsInterface();
            }
            
            if (g_pSteamFriends) {
                const char* name = g_pSteamFriends->GetPersonaName();
                if (name && name[0] != '\0') {
                    return std::string(name);
                }
            }
            return "UnknownUser";
        }

        void Render() {
            if (!Menu::cfg["watermark"].b) return;

            int options = Menu::cfg["watermark_options"].i;
            int name_source = Menu::cfg["watermark_name_source"].i;
            
            std::string name = (name_source == 0) ? "LoaderUser" : GetSteamName();
            std::string text = "paragon | " + name;

            int current_ping = 0;
            
            // зафикси если не пидорас

            if (options & (1 << 1)) text += " | " + std::to_string(current_ping) + "ms";
            
            if (options & (1 << 2)) {
                auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                std::tm tm; localtime_s(&tm, &now);
                std::ostringstream oss; oss << std::put_time(&tm, "%H:%M:%S");
                text += " | " + oss.str();
            }

            ImVec2 sz = ImGui::CalcTextSize(text.c_str());
            ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
            
            ImGui::Begin("##watermark", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
            
            ImDrawList* dl = ImGui::GetWindowDrawList();
            ImVec2 p = ImGui::GetWindowPos();
            
            dl->AddRectFilled(p, ImVec2(p.x + sz.x + 16, p.y + sz.y + 12), IM_COL32(30, 30, 30, 200));
            
            ImVec4 clr = (options & (1 << 3)) ? Menu::cfg["menu_color"].c : Menu::cfg["watermark_color"].c;
            dl->AddRectFilled(p, ImVec2(p.x + sz.x + 16, p.y + 2), ImGui::ColorConvertFloat4ToU32(clr));
            
            ImGui::SetCursorPos(ImVec2(8, 6)); 
            ImGui::TextUnformatted(text.c_str());
            
            ImGui::End();
        }
    }
}