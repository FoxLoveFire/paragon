#include "menu.h"
#include "menu_layout.h"
#include "../../../external/imgui/imgui.h"
#include "../../../external/imgui/imgui_internal.h"
#include "../../../globals.h"
#include "../../../external/nlohmann/single_include/nlohmann/json.hpp"
#include <fstream>
#include <windows.h>
#include <shellapi.h>

using json = nlohmann::json;

namespace UI_MDD {
    namespace {
        ImGuiID open_id = 0;
        int open_frame = -1;
        ImVec2 open_pos = {};
        float open_w = 0.f;
        std::vector<std::string> items;
        std::string open_config_id = "";
    }

    inline bool MultiDropdown(const char* label, const std::string& config_id, const std::vector<std::string>& items_list, float width, ImVec2 pos, ImDrawList* dl) {
        const float h = 17.0f;
        ImGui::SetCursorScreenPos(pos);
        ImGui::InvisibleButton(label, ImVec2(width, h));
        bool clicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);
        ImGuiID my_id = ImGui::GetID(label);

        if (clicked) {
            if (open_id == my_id) {
                open_id = 0;
                open_config_id = "";
            } else {
                open_id = my_id;
                open_frame = ImGui::GetFrameCount();
                open_pos = ImVec2(pos.x, pos.y + h);
                open_w = width;
                items = items_list;
                open_config_id = config_id;
            }
        }

        bool is_open = (open_id == my_id);
        dl->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + h), Colors::Bg, 2.0f);
        
        ImVec2 grad_top = pos;
        ImVec2 grad_bot = ImVec2(pos.x + width, pos.y + h);
        dl->AddRectFilledMultiColor(
            grad_top, grad_bot,
            IM_COL32(20, 20, 20, 255), IM_COL32(20, 20, 20, 255),
            IM_COL32(9, 9, 9, 255), IM_COL32(9, 9, 9, 255));

        std::string preview = "";
        int active_count = 0;
        int current_val = Menu::cfg[config_id].i;

        for (size_t i = 0; i < items_list.size(); i++) {
            if (current_val & (1 << i)) {
                if (active_count > 0) preview += ", ";
                preview += items_list[i];
                active_count++;
            }
        }
        
        if (active_count == 0) preview = "";
        else if (active_count == static_cast<int>(items_list.size())) preview = "all";

        ImGui::PushClipRect(pos, ImVec2(pos.x + width - 14.0f, pos.y + h), true);
        float ty = pos.y + (h - ImGui::GetTextLineHeight()) * 0.5f;
        dl->AddText(ImVec2(pos.x + 7.0f, ty), IM_COL32(255, 255, 255, 255), preview.c_str());
        ImGui::PopClipRect();

        float ax = pos.x + width - 14.0f;
        float ay = pos.y + h * 0.5f;
        dl->AddTriangleFilled(
            ImVec2(ax, ay - 2.0f),
            ImVec2(ax + 5.0f, ay - 2.0f),
            ImVec2(ax + 2.5f, ay + 2.0f),
            is_open ? Colors::Accent : Colors::TextBind);

        return clicked;
    }

    inline void RenderOpen() {
        if (open_id == 0 || open_config_id.empty()) return;

        const float item_h = 16.0f;
        float total = static_cast<float>(items.size()) * item_h + 4.0f;
        ImVec2 p0 = open_pos;
        ImVec2 p1 = ImVec2(p0.x + open_w, p0.y + total);
        ImVec2 mouse = ImGui::GetIO().MousePos;

        ImGui::SetNextWindowPos(p0);
        ImGui::SetNextWindowSize(ImVec2(open_w, total));
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        
        ImGui::Begin("##mdd_popup", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoFocusOnAppearing);
        
        ImDrawList* dl = ImGui::GetWindowDrawList();
        dl->AddRectFilled(ImVec2(p0.x + 2, p0.y + 2), ImVec2(p1.x + 2, p1.y + 2), IM_COL32(0, 0, 0, 60), 3.0f);
        dl->AddRectFilled(p0, p1, Colors::Bg, 3.0f);

        int click_sel = -1;
        bool click_any = ImGui::IsMouseClicked(ImGuiMouseButton_Left) && (ImGui::GetFrameCount() > open_frame);
        int current_val = Menu::cfg[open_config_id].i;

        for (size_t i = 0; i < items.size(); i++) {
            ImVec2 ip0 = ImVec2(p0.x + 1.0f, p0.y + 2.0f + static_cast<float>(i) * item_h);
            ImVec2 ip1 = ImVec2(p1.x - 1.0f, ip0.y + item_h);

            bool hov_i = (mouse.x >= ip0.x && mouse.x <= ip1.x && mouse.y >= ip0.y && mouse.y <= ip1.y);
            bool sel_i = (current_val & (1 << i));

            if (hov_i) dl->AddRectFilled(ip0, ip1, IM_COL32(35, 32, 44, 255), 2.0f);

            float ty = ip0.y + (item_h - ImGui::GetTextLineHeight()) * 0.5f;
            ImU32 tc = sel_i ? Colors::Accent : IM_COL32(255, 255, 255, 255);
            dl->AddText(ImVec2(ip0.x + 7.0f, ty), tc, items[i].c_str());

            if (hov_i && click_any) click_sel = static_cast<int>(i);
        }

        ImGui::End();
        ImGui::PopStyleVar(3);

        if (click_sel >= 0) {
            if (Menu::cfg[open_config_id].i & (1 << click_sel)) Menu::cfg[open_config_id].i &= ~(1 << click_sel);
            else Menu::cfg[open_config_id].i |= (1 << click_sel);
        } else if (click_any) {
            bool inside = (mouse.x >= p0.x && mouse.x <= p1.x && mouse.y >= p0.y && mouse.y <= p1.y);
            if (!inside) {
                open_id = 0;
                open_config_id = "";
            }
        }
    }
}

namespace Menu {
    std::unordered_map<std::string, ConfigValue> cfg;

    struct ElementDef {
        std::string type, id, label, condition, suffix, format = "%.1f";
        std::vector<std::string> color_ids;
        float min = 0.f, max = 100.f;
        std::vector<std::string> items;
    };
    struct GroupBoxDef {
        std::string name;
        std::vector<ElementDef> elements;
    };
    struct SubTabDef {
        std::string name;
        std::vector<GroupBoxDef> left, right;
    };
    struct TabDef {
        std::string name;
        std::vector<SubTabDef> subtabs;
        int active_subtab = 0;
    };

    std::vector<TabDef> g_MenuLayout;
    int g_ActiveTab = 1;

    static const float MENU_W = 660.0f, MENU_H = 560.0f, TAB_H = 26.0f, TITLE_H = 22.0f;
    static const float PADDING = 10.0f, GAP = 14.0f, BOX_PAD_X = 12.0f, BOX_PAD_Y = 8.0f, ITEM_SPACING = 34.0f;
    static const float L_W = (MENU_W - PADDING * 2.0f - GAP) / 2.0f;
    static const float R_X = PADDING + L_W + GAP, R_W = L_W;

    static const char* k_cfg_base_dir = "C:\\paragon\\";
    static const char* k_cfg_dir = "C:\\paragon\\settings\\";
    static const char* k_cfg_ext = ".json";
    static std::vector<std::string> s_cfg_list;
    static int s_cfg_sel = -1;
    static char s_cfg_input[64] = {};
    static bool s_cfg_dirty = true;

    static const char* k_img_dir = "C:\\paragon\\images\\";
    static std::vector<std::string> s_img_list;
    static int s_img_sel = -1;
    static bool s_img_dirty = true;

    static void CfgEnsureDir() {
        CreateDirectoryA(k_cfg_base_dir, nullptr);
        CreateDirectoryA(k_cfg_dir, nullptr);
    }

    static void CfgScan() {
        std::string prev;
        if (s_cfg_sel >= 0 && s_cfg_sel < static_cast<int>(s_cfg_list.size()))
            prev = s_cfg_list[s_cfg_sel];
        s_cfg_list.clear();

        WIN32_FIND_DATAA fd;
        std::string pattern = std::string(k_cfg_dir) + "*" + k_cfg_ext;
        HANDLE h = FindFirstFileA(pattern.c_str(), &fd);
        if (h != INVALID_HANDLE_VALUE) {
            do {
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
                std::string name = fd.cFileName;
                size_t ext_pos = name.rfind(k_cfg_ext);
                if (ext_pos != std::string::npos) name = name.substr(0, ext_pos);
                s_cfg_list.push_back(name);
            } while (FindNextFileA(h, &fd));
            FindClose(h);
        }
        s_cfg_sel = -1;
        s_cfg_dirty = false;
        if (!prev.empty()) {
            for (int i = 0; i < static_cast<int>(s_cfg_list.size()); i++) {
                if (s_cfg_list[i] == prev) { s_cfg_sel = i; break; }
            }
        }
    }

    static void CfgSaveInternal(const std::string& filename) {
        CfgEnsureDir();
        std::string path = std::string(k_cfg_dir) + filename + k_cfg_ext;
        json j;
        for (const auto& [k, v] : cfg) {
            j[k]["b"] = v.b;
            j[k]["f"] = v.f;
            j[k]["i"] = v.i;
            j[k]["c"] = { v.c.x, v.c.y, v.c.z, v.c.w };
            j[k]["i_c"] = v.i_c;
            j[k]["s"] = v.s;
        }
        std::ofstream out(path);
        if (out.is_open()) {
            out << j.dump(4);
            out.close();
        }
    }

    static void CfgCreate() {
        if (s_cfg_input[0] == '\0') return;
        std::string name = s_cfg_input;
        CfgSaveInternal(name);
        CfgScan();
        for (int i = 0; i < static_cast<int>(s_cfg_list.size()); i++) {
            if (s_cfg_list[i] == name) { s_cfg_sel = i; break; }
        }
        s_cfg_input[0] = '\0';
    }

    static void CfgSave() {
        if (s_cfg_sel < 0 || s_cfg_sel >= static_cast<int>(s_cfg_list.size())) return;
        CfgSaveInternal(s_cfg_list[s_cfg_sel]);
    }

    static void CfgLoad() {
        if (s_cfg_sel < 0 || s_cfg_sel >= static_cast<int>(s_cfg_list.size())) return;
        std::string path = std::string(k_cfg_dir) + s_cfg_list[s_cfg_sel] + k_cfg_ext;
        std::ifstream in(path);
        if (!in.is_open()) return;

        json j;
        try {
            in >> j;
            for (auto it = j.begin(); it != j.end(); ++it) {
                std::string key = it.key();
                if (cfg.find(key) != cfg.end()) {
                    auto& val = it.value();
                    if (val.contains("b")) cfg[key].b = val["b"];
                    if (val.contains("f")) cfg[key].f = val["f"];
                    if (val.contains("i")) cfg[key].i = val["i"];
                    if (val.contains("c") && val["c"].is_array() && val["c"].size() == 4) {
                        cfg[key].c = ImVec4(val["c"][0], val["c"][1], val["c"][2], val["c"][3]);
                    }
                    if (val.contains("i_c")) cfg[key].i_c = val["i_c"];
                    if (val.contains("s")) cfg[key].s = val["s"];
                }
            }
        } catch (...) {}
    }

    static void CfgOpenDir() {
        CfgEnsureDir();
        ShellExecuteA(nullptr, "open", k_cfg_dir, nullptr, nullptr, SW_SHOW);
    }

    static void ImgEnsureDir() {
        CreateDirectoryA(k_cfg_base_dir, nullptr);
        CreateDirectoryA(k_img_dir, nullptr);
    }

    static void ImgScan() {
        std::string prev;
        if (s_img_sel >= 0 && s_img_sel < static_cast<int>(s_img_list.size()))
            prev = s_img_list[s_img_sel];
        s_img_list.clear();

        WIN32_FIND_DATAA fd;
        std::string pattern = std::string(k_img_dir) + "*.*";
        HANDLE h = FindFirstFileA(pattern.c_str(), &fd);
        if (h != INVALID_HANDLE_VALUE) {
            do {
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
                std::string name = fd.cFileName;
                
                std::string lower_name = name;
                for (char& c : lower_name) c = std::tolower(c);
                
                if (lower_name.find(".png") != std::string::npos || 
                    lower_name.find(".jpg") != std::string::npos || 
                    lower_name.find(".jpeg") != std::string::npos) {
                    s_img_list.push_back(name);
                }
            } while (FindNextFileA(h, &fd));
            FindClose(h);
        }
        s_img_sel = -1;
        s_img_dirty = false;
        
        std::string current_cfg = cfg["custom_overlay_file"].s;
        if (!current_cfg.empty()) {
            for (int i = 0; i < static_cast<int>(s_img_list.size()); i++) {
                if (s_img_list[i] == current_cfg) { s_img_sel = i; break; }
            }
        } else if (!prev.empty()) {
            for (int i = 0; i < static_cast<int>(s_img_list.size()); i++) {
                if (s_img_list[i] == prev) { s_img_sel = i; break; }
            }
        }
    }

    void LoadConfigLayout() {
        try {
            json j = json::parse(json_layout);
            g_MenuLayout.clear();

            if (!j.contains("tabs") || !j["tabs"].is_array()) return;

            for (auto& t : j["tabs"]) {
                TabDef tab; 
                tab.name = t.value("name", "Unknown Tab");
                
                if (t.contains("subtabs") && t["subtabs"].is_array()) {
                    for (auto& st : t["subtabs"]) {
                        SubTabDef subtab; 
                        subtab.name = st.value("name", "Unknown Subtab");

                        auto parse_box = [&](json& boxes, std::vector<GroupBoxDef>& target) {
                            if (!boxes.is_array()) return;

                            for (auto& gb : boxes) {
                                GroupBoxDef box; 
                                box.name = gb.value("name", "Unknown Box");
                                
                                if (gb.contains("elements") && gb["elements"].is_array()) {
                                    for (auto& el : gb["elements"]) {
                                        ElementDef elem;
                                        elem.type = el.value("type", ""); 
                                        elem.id = el.value("id", ""); 
                                        elem.label = el.value("label", "");
                                        
                                        if (!elem.id.empty()) cfg[elem.id];

                                        elem.condition = el.value("condition", "");
                                        elem.suffix = el.value("suffix", ""); 
                                        elem.format = el.value("format", "%.1f");
                                        elem.min = el.value("min", 0.f); 
                                        elem.max = el.value("max", 100.f);
                                        
                                        if (el.contains("color") && el["color"].is_string()) {
                                            std::string cid = el["color"].get<std::string>();
                                            elem.color_ids.push_back(cid);
                                            cfg[cid];
                                        }
                                        if (el.contains("colors") && el["colors"].is_array()) {
                                            for (auto& c : el["colors"]) {
                                                if (c.is_string()) {
                                                    std::string cid = c.get<std::string>();
                                                    elem.color_ids.push_back(cid);
                                                    cfg[cid];
                                                }
                                            }
                                        }
                                        
                                        if (el.contains("items") && el["items"].is_array()) {
                                            for (auto& item : el["items"]) {
                                                if (item.is_string()) elem.items.push_back(item.get<std::string>());
                                            }
                                        }
                                        
                                        if (el.contains("default")) {
                                            try {
                                                if (elem.type == "slider") cfg[elem.id].f = el["default"].get<float>();
                                                else if (elem.type == "checkbox") cfg[elem.id].b = el["default"].get<bool>();
                                                else if (elem.type == "dropdown" || elem.type == "multidropdown") cfg[elem.id].i = el["default"].get<int>();
                                            } catch (...) {}
                                        }

                                        if (el.contains("default_color") && el["default_color"].is_array() && el["default_color"].size() >= 4 && !elem.color_ids.empty()) {
                                            try {
                                                auto c = el["default_color"];
                                                cfg[elem.color_ids[0]].c = ImVec4(c[0].get<float>(), c[1].get<float>(), c[2].get<float>(), c[3].get<float>());
                                            } catch (...) {}
                                        }
                                        box.elements.push_back(elem);
                                    }
                                }
                                target.push_back(box);
                            }
                        };
                        if (st.contains("left")) parse_box(st["left"], subtab.left);
                        if (st.contains("right")) parse_box(st["right"], subtab.right);
                        tab.subtabs.push_back(subtab);
                    }
                }
                g_MenuLayout.push_back(tab);
            }
        } catch (...) {
            return;
        }
    }

    void Initialize() {
        cfg["active_tab"].i = 1;
        cfg["visuals_subtab"].i = 0; 
        
        cfg["custom_overlay_type"].i = 0;
        cfg["custom_overlay_url"].s = "";
        cfg["custom_overlay_file"].s = "";
        cfg["custom_overlay_apply"].b = false;
        cfg["custom_overlay_scale"].f = 1.0f;
        cfg["custom_overlay_alpha"].f = 1.0f;

        LoadConfigLayout();
        
        g_ActiveTab = cfg["active_tab"].i;
        if (g_ActiveTab >= 0 && g_ActiveTab < static_cast<int>(g_MenuLayout.size())) {
            if (cfg["visuals_subtab"].i >= 0 && cfg["visuals_subtab"].i < static_cast<int>(g_MenuLayout[g_ActiveTab].subtabs.size())) {
                g_MenuLayout[g_ActiveTab].active_subtab = cfg["visuals_subtab"].i;
            }
        }
    }

    static void DrawBox(ImDrawList* dl, ImVec2 wpos, float lx, float box_top, float box_bottom, float w, const char* label) {
        auto S = [&](float x, float y) { return ImVec2(wpos.x + x, wpos.y + y); };
        float lh = ImGui::GetTextLineHeight(), gap = 5.0f;
        ImVec2 ts = ImGui::CalcTextSize(label);
        float tx = lx + 12.0f, ty = box_top - lh * 0.5f;

        dl->AddLine(S(lx, box_top), S(tx - gap, box_top), Colors::SectionBorder, 1.0f);
        dl->AddLine(S(tx + ts.x + gap, box_top), S(lx + w, box_top), Colors::SectionBorder, 1.0f);
        dl->AddLine(S(lx, box_top), S(lx, box_bottom), Colors::SectionBorder, 1.0f);
        dl->AddLine(S(lx + w, box_top), S(lx + w, box_bottom), Colors::SectionBorder, 1.0f);
        dl->AddLine(S(lx, box_bottom), S(lx + w, box_bottom), Colors::SectionBorder, 1.0f);
        dl->AddRectFilled(S(tx, ty), S(tx + ts.x, ty + lh), Colors::Bg);
        dl->AddText(S(tx, ty), IM_COL32(255, 255, 255, 255), label);
    }

    static void RenderConfigList(float inner_x, float inner_w, float& y, ImVec2 wpos, ImDrawList* dl) {
        if (s_cfg_dirty) CfgScan();

        const float item_h = 17.0f;
        const float input_h = 17.0f;
        const float btn_h = 18.0f;
        const float list_h = 240.0f;

        float list_w = inner_w;
        ImVec2 list_pos = ImVec2(wpos.x + inner_x, wpos.y + y);
        ImVec2 list_end = ImVec2(list_pos.x + list_w, list_pos.y + list_h);

        dl->AddRectFilled(list_pos, list_end, IM_COL32(6, 5, 7, 255), 3.0f);
        dl->AddRect(list_pos, list_end, Colors::SectionBorder, 3.0f, 0, 1.0f);

        ImVec2 mouse = ImGui::GetIO().MousePos;
        int hov_idx = -1;
        int max_vis = static_cast<int>(list_h / item_h);

        for (int i = 0; i < static_cast<int>(s_cfg_list.size()) && i < max_vis; i++) {
            ImVec2 ip0 = ImVec2(list_pos.x + 1, list_pos.y + 1 + i * item_h);
            ImVec2 ip1 = ImVec2(list_end.x - 1, ip0.y + item_h);

            bool hov_i = (mouse.x >= ip0.x && mouse.x <= ip1.x && mouse.y >= ip0.y && mouse.y <= ip1.y);
            bool sel_i = (s_cfg_sel == i);

            if (hov_i) hov_idx = i;

            if (sel_i)      dl->AddRectFilled(ip0, ip1, IM_COL32(35, 32, 44, 255), 2.0f);
            else if (hov_i) dl->AddRectFilled(ip0, ip1, IM_COL32(22, 20, 27, 255), 2.0f);

            float ty2 = ip0.y + (item_h - ImGui::GetTextLineHeight()) * 0.5f;
            ImU32 tc = sel_i ? Colors::Accent : (hov_i ? IM_COL32(255, 255, 255, 255) : IM_COL32(180, 180, 180, 255));
            dl->AddText(ImVec2(ip0.x + 6.0f, ty2), tc, s_cfg_list[i].c_str());
        }

        ImGui::SetCursorScreenPos(list_pos);
        ImGui::InvisibleButton("##cfg_list", ImVec2(list_w, list_h));
        if (ImGui::IsItemClicked() && hov_idx >= 0) s_cfg_sel = hov_idx;

        y += list_h + 8.0f;

        ImGui::SetCursorPos(ImVec2(inner_x, y));
        UI::TextInput("##cfg_name", s_cfg_input, sizeof(s_cfg_input), list_w);
        y += input_h + 4.0f;

        std::pair<const char*, void(*)()> buttons[] = {
            {"create##cfg", CfgCreate}, {"save##cfg", CfgSave}, {"load##cfg", CfgLoad}, {"open directory##cfg", CfgOpenDir}
        };

        for (const auto& btn : buttons) {
            ImGui::SetCursorPos(ImVec2(inner_x, y));
            if (UI::Button(btn.first, list_w)) btn.second();
            y += btn_h + (strcmp(btn.first, "open directory##cfg") == 0 ? 8.0f : 3.0f);
        }
    }

    static void RenderImageList(float inner_x, float inner_w, float& y, ImVec2 wpos, ImDrawList* dl) {
        if (s_img_dirty) ImgScan();

        const float item_h = 17.0f;
        const float list_h = 140.0f;
        const float btn_h = 18.0f;

        float list_w = inner_w;
        ImVec2 list_pos = ImVec2(wpos.x + inner_x, wpos.y + y);
        ImVec2 list_end = ImVec2(list_pos.x + list_w, list_pos.y + list_h);

        dl->AddRectFilled(list_pos, list_end, IM_COL32(6, 5, 7, 255), 3.0f);
        dl->AddRect(list_pos, list_end, Colors::SectionBorder, 3.0f, 0, 1.0f);

        ImVec2 mouse = ImGui::GetIO().MousePos;
        int hov_idx = -1;
        int max_vis = static_cast<int>(list_h / item_h);

        for (int i = 0; i < static_cast<int>(s_img_list.size()) && i < max_vis; i++) {
            ImVec2 ip0 = ImVec2(list_pos.x + 1, list_pos.y + 1 + i * item_h);
            ImVec2 ip1 = ImVec2(list_end.x - 1, ip0.y + item_h);

            bool hov_i = (mouse.x >= ip0.x && mouse.x <= ip1.x && mouse.y >= ip0.y && mouse.y <= ip1.y);
            bool sel_i = (s_img_sel == i) || (cfg["custom_overlay_file"].s == s_img_list[i]);

            if (hov_i) hov_idx = i;

            if (sel_i)      dl->AddRectFilled(ip0, ip1, IM_COL32(35, 32, 44, 255), 2.0f);
            else if (hov_i) dl->AddRectFilled(ip0, ip1, IM_COL32(22, 20, 27, 255), 2.0f);

            float ty2 = ip0.y + (item_h - ImGui::GetTextLineHeight()) * 0.5f;
            ImU32 tc = sel_i ? Colors::Accent : (hov_i ? IM_COL32(255, 255, 255, 255) : IM_COL32(180, 180, 180, 255));
            dl->AddText(ImVec2(ip0.x + 6.0f, ty2), tc, s_img_list[i].c_str());
        }

        ImGui::SetCursorScreenPos(list_pos);
        ImGui::InvisibleButton("##img_list", ImVec2(list_w, list_h));
        if (ImGui::IsItemClicked() && hov_idx >= 0) {
            s_img_sel = hov_idx;
            cfg["custom_overlay_file"].s = s_img_list[hov_idx];
        }

        y += list_h + 8.0f;

        ImGui::SetCursorPos(ImVec2(inner_x, y));
        if (UI::Button("open directory##img", list_w)) {
            ImgEnsureDir();
            ShellExecuteA(nullptr, "open", k_img_dir, nullptr, nullptr, SW_SHOW);
        }
        y += btn_h + 4.0f;
        
        ImGui::SetCursorPos(ImVec2(inner_x, y));
        if (UI::Button("refresh list##img", list_w)) {
            s_img_dirty = true;
        }
        y += btn_h + 8.0f;
    }

    void RenderElements(std::vector<ElementDef>& elements, float x, float w, float& y, ImVec2 wpos, ImDrawList* dl) {
        float inner_x = x + 4.0f;
        float inner_w = w - 8.0f;

        for (auto& el : elements) {
            if (!el.condition.empty() && !cfg[el.condition].b) continue;

            if (el.type == "checkbox") {
                ImGui::SetCursorPos(ImVec2(inner_x, y));
                float check_w = el.color_ids.empty() ? inner_w : inner_w - (22.0f * el.color_ids.size());
                UI::Checkbox(el.label.c_str(), &cfg[el.id].b, nullptr, nullptr, check_w);
                
                for (size_t i = 0; i < el.color_ids.size(); i++) {
                    ImGui::SetCursorPos(ImVec2(inner_x, y));
                    UI::ColorPicker(("##" + el.color_ids[i]).c_str(), &cfg[el.color_ids[i]].c, wpos.x + inner_w + inner_x - ((el.color_ids.size() - i - 1) * 22.0f), 0.0f, &cfg[el.color_ids[i]].i_c);
                }
                y += ITEM_SPACING;
            } 
            else if (el.type == "slider") {
                ImGui::SetCursorPos(ImVec2(inner_x, y));
                UI::SliderFloat(el.label.c_str(), &cfg[el.id].f, el.min, el.max, el.suffix.c_str(), el.format.c_str(), inner_w);
                y += ITEM_SPACING;
            } 
            else if (el.type == "dropdown") {
                if (!el.label.empty() && el.label.find("##") != 0) {
                    dl->AddText(ImVec2(wpos.x + inner_x, wpos.y + y), IM_COL32(255, 255, 255, 255), el.label.c_str());
                    y += ImGui::GetTextLineHeight() + 4.0f;
                }
                std::vector<const char*> cstrs;
                for (auto& str : el.items) cstrs.push_back(str.c_str());
                
                ImGui::SetCursorPos(ImVec2(inner_x, y));
                UI::Dropdown(("##" + el.id).c_str(), &cfg[el.id].i, cstrs.data(), static_cast<int>(cstrs.size()), inner_w);
                y += ITEM_SPACING;
            } 
            else if (el.type == "multidropdown") {
                dl->AddText(ImVec2(wpos.x + inner_x, wpos.y + y), IM_COL32(255, 255, 255, 255), el.label.c_str());
                y += ImGui::GetTextLineHeight() + 4.0f;
                
                UI_MDD::MultiDropdown(("##" + el.id).c_str(), el.id, el.items, inner_w, ImVec2(wpos.x + inner_x, wpos.y + y), dl);
                y += 17.0f + ITEM_SPACING - 10.0f;
            } 
            else if (el.type == "colorpicker") {
                if (!el.label.empty() && el.label.find("##") != 0) {
                    dl->AddText(ImVec2(wpos.x + inner_x, wpos.y + y + (16.0f - ImGui::GetTextLineHeight()) * 0.5f), IM_COL32(255, 255, 255, 255), el.label.c_str());
                }
                ImGui::SetCursorPos(ImVec2(inner_x, y));
                if (!el.color_ids.empty()) {
                    UI::ColorPicker(("##" + el.color_ids[0]).c_str(), &cfg[el.color_ids[0]].c, wpos.x + inner_x + inner_w, 0.0f, &cfg[el.color_ids[0]].i_c);
                }
                y += ITEM_SPACING;
            } 
            else if (el.type == "config_list") {
                RenderConfigList(inner_x, inner_w, y, wpos, dl);
            }
            else if (el.type == "watermark") {
                ImGui::SetCursorPos(ImVec2(inner_x, y));
                UI::Checkbox(el.label.c_str(), &cfg[el.id].b, nullptr, nullptr, inner_w);
                y += ITEM_SPACING;
            } 
            else if (el.type == "overlay_image") {
                ImGui::SetCursorPos(ImVec2(inner_x, y));
                UI::Checkbox(el.label.c_str(), &cfg[el.id].b, nullptr, nullptr, inner_w);
                y += ITEM_SPACING;
                
                if (cfg[el.id].b) {
                    ImGui::SetCursorPos(ImVec2(inner_x, y));
                    static const char* overlay_types[] = { "URL", "Local File" };
                    UI::Dropdown("##overlay_type", &cfg["custom_overlay_type"].i, overlay_types, 2, inner_w);
                    y += ITEM_SPACING;
                    
                    if (cfg["custom_overlay_type"].i == 0) {
                        char buf[1024] = {0};
                        if (!cfg["custom_overlay_url"].s.empty()) {
                            strncpy_s(buf, sizeof(buf), cfg["custom_overlay_url"].s.c_str(), _TRUNCATE);
                        }
                        ImGui::SetCursorPos(ImVec2(inner_x, y));
                        UI::TextInput("##overlay_url", buf, sizeof(buf), inner_w);
                        cfg["custom_overlay_url"].s = std::string(buf);
                        dl->AddText(ImVec2(wpos.x + inner_x, wpos.y + y - 16.0f), IM_COL32(150, 150, 150, 255), "Image URL:");
                        y += 24.0f;
                    } else {
                        dl->AddText(ImVec2(wpos.x + inner_x, wpos.y + y), IM_COL32(150, 150, 150, 255), "Select Image:");
                        y += ImGui::GetTextLineHeight() + 4.0f;
                        RenderImageList(inner_x, inner_w, y, wpos, dl);
                    }

                    ImGui::SetCursorPos(ImVec2(inner_x, y));
                    if (UI::Button("Apply Image", inner_w)) {
                        cfg["custom_overlay_apply"].b = true;
                    }
                    y += 24.0f;

                    ImGui::SetCursorPos(ImVec2(inner_x, y));
                    UI::SliderFloat("Scale", &cfg["custom_overlay_scale"].f, 0.1f, 5.0f, "x", "%.2f", inner_w);
                    y += ITEM_SPACING;

                    ImGui::SetCursorPos(ImVec2(inner_x, y));
                    UI::SliderFloat("Alpha", &cfg["custom_overlay_alpha"].f, 0.01f, 1.0f, "", "%.2f", inner_w);
                    y += ITEM_SPACING;
                }
            }
        }
    }

    static void ApplySettingsToGlobals() {
        auto to_c_color = [](const ConfigValue& cv) {
            return c_color(
                static_cast<int>(cv.c.x * 255.0f * cv.i_c), 
                static_cast<int>(cv.c.y * 255.0f * cv.i_c), 
                static_cast<int>(cv.c.z * 255.0f * cv.i_c), 
                static_cast<int>(cv.c.w * 255.0f)
            );
        };

        G::world.custom_light = cfg["override_lights"].b;
        G::world.light_color = to_c_color(cfg["override_lights_col"]);
        
        G::world.custom_wall_color = cfg["override_wall"].b;
        G::world.wall_color = to_c_color(cfg["override_wall_col"]);

        G::world.custom_skybox = cfg["override_skybox"].b;
        G::world.skybox_color = to_c_color(cfg["override_skybox_col1"]);
        G::world.skybox_color2 = to_c_color(cfg["override_skybox_col2"]);
        G::world.skybox_color3 = to_c_color(cfg["override_skybox_col3"]);

        G::removals.disable_smoke        = (cfg["removals"].i & (1 << 0)) != 0;
        G::removals.disable_flashbang    = (cfg["removals"].i & (1 << 1)) != 0;
        G::removals.disable_scopeoverlay = (cfg["removals"].i & (1 << 2)) != 0;
        G::removals.remove_legs          = (cfg["removals"].i & (1 << 3)) != 0;

        G::misc.rh_enabled = cfg["radar_hack"].b;
        G::viewmodel.is_enabled = cfg["override_viewmodel_pos"].b;
        if (G::viewmodel.xyz.size() < 3) G::viewmodel.xyz.resize(3);
        G::viewmodel.xyz[0] = cfg["viewmodel_x"].f;
        G::viewmodel.xyz[1] = cfg["viewmodel_y"].f;
        G::viewmodel.xyz[2] = cfg["viewmodel_z"].f;
        G::viewmodel.v_fov  = cfg["viewmodel_fov"].f;

        G::viewport.custom_fov   = cfg["override_fov"].b;
        G::viewport.fov          = cfg["fov_val"].f;
        G::viewport.aspect_ratio = cfg["aspect_ratio"].f;
        G::world.m_exposure = cfg["darkening_val"].f;

    }

    void Render() {
        ImGuiContext* ctx = ImGui::GetCurrentContext();
        if (!ctx || !ctx->WithinFrameScope) return;

        if (cfg.find("menu_color") != cfg.end()) {
            ImVec4 mc = cfg["menu_color"].c;
            Colors::Accent = ImGui::ColorConvertFloat4ToU32(mc);
            ImVec4 hov(ImMin(mc.x * 1.09f, 1.0f), ImMin(mc.y * 1.09f, 1.0f), ImMin(mc.z * 1.09f, 1.0f), mc.w);
            Colors::AccentHover = ImGui::ColorConvertFloat4ToU32(hov);
            ImVec4 dark(mc.x * 0.32f, mc.y * 0.32f, mc.z * 0.32f, mc.w);
            Colors::AccentDark = ImGui::ColorConvertFloat4ToU32(dark);
            ImVec4 bh(mc.x * 0.82f, mc.y * 0.82f, mc.z * 0.82f, mc.w);
            Colors::CbBorderHov = ImGui::ColorConvertFloat4ToU32(bh);
        }

        UI::UpdateBindCapture();
        ImGuiIO& io = ImGui::GetIO();
        
        ImGui::SetNextWindowPos(ImVec2((io.DisplaySize.x - MENU_W) * 0.5f, (io.DisplaySize.y - (MENU_H + TAB_H)) * 0.5f), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(MENU_W, MENU_H + TAB_H));

        ImGuiWindowFlags wf = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove;
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::Begin("##paragon", nullptr, wf);
        ImGui::PopStyleVar(2);

        float total_h = MENU_H + TAB_H;
        ImGui::SetCursorPos(ImVec2(0, 0));
        ImGui::Dummy(ImVec2(MENU_W, total_h));

        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 wpos = ImGui::GetWindowPos();

        dl->AddRectFilled(wpos, ImVec2(wpos.x + MENU_W, wpos.y + total_h), Colors::Bg, 0.0f);
        
        ImVec2 size = ImVec2(MENU_W, total_h);
        dl->AddRect(ImVec2(wpos.x - 1, wpos.y - 1), ImVec2(wpos.x + size.x + 1, wpos.y + size.y + 1), IM_COL32(0, 0, 0, 255), 0.0f, 0, 1.0f);
        dl->AddRect(ImVec2(wpos.x - 2, wpos.y - 2), ImVec2(wpos.x + size.x + 2, wpos.y + size.y + 2), IM_COL32(60, 60, 60, 255), 0.0f, 0, 1.0f);
        dl->AddRect(ImVec2(wpos.x - 3, wpos.y - 3), ImVec2(wpos.x + size.x + 3, wpos.y + size.y + 3), IM_COL32(0, 0, 0, 255), 0.0f, 0, 1.0f);

        dl->AddRectFilled(wpos, ImVec2(wpos.x + MENU_W, wpos.y + TITLE_H), Colors::TitleBg, 0.0f);
        dl->AddRectFilledMultiColor(wpos, ImVec2(wpos.x + MENU_W, wpos.y + TITLE_H), IM_COL32(20, 20, 20, 255), IM_COL32(20, 20, 20, 255), IM_COL32(7, 7, 7, 255), IM_COL32(7, 7, 7, 255));
        UI::DrawGradientLine(dl, wpos.x, wpos.y + TITLE_H, MENU_W, 1.0f);

        ImVec2 ts = ImGui::CalcTextSize("paragon");
        dl->AddText(ImVec2(wpos.x + (MENU_W - ts.x) * 0.5f, wpos.y + (TITLE_H - ts.y) * 0.5f), IM_COL32(185, 182, 196, 255), "paragon");

        ImGui::SetCursorPos(ImVec2(0, 0));
        ImGui::InvisibleButton("##titlebar", ImVec2(MENU_W, TITLE_H));
        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            ImVec2 delta = ImGui::GetIO().MouseDelta;
            ImGui::SetWindowPos(ImVec2(wpos.x + delta.x, wpos.y + delta.y));
        }

        std::vector<const char*> tab_names;
        for (auto& t : g_MenuLayout) tab_names.push_back(t.name.c_str());
        
        g_ActiveTab = UI::TabBar(tab_names.data(), static_cast<int>(tab_names.size()), g_ActiveTab, ImVec2(wpos.x, wpos.y + MENU_H), MENU_W, TAB_H);

        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));

        if (g_ActiveTab >= 0 && g_ActiveTab < static_cast<int>(g_MenuLayout.size())) {
            auto& active_tab_def = g_MenuLayout[g_ActiveTab];

            if (!active_tab_def.subtabs.empty()) {
                float content_y = TITLE_H + PADDING; 
                
                if (active_tab_def.subtabs.size() > 1) {
                    std::vector<const char*> subtab_names;
                    for (auto& st : active_tab_def.subtabs) subtab_names.push_back(st.name.c_str());
                    
                    active_tab_def.active_subtab = UI::SubTabBar(subtab_names.data(), static_cast<int>(subtab_names.size()), active_tab_def.active_subtab, ImVec2(wpos.x, wpos.y + TITLE_H + 4.0f), PADDING);
                    content_y = TITLE_H + 4.0f + ImGui::GetTextLineHeight() + PADDING; 
                } else {
                    active_tab_def.active_subtab = 0; 
                }

                if (active_tab_def.active_subtab >= 0 && active_tab_def.active_subtab < static_cast<int>(active_tab_def.subtabs.size())) {
                    auto& active_sub = active_tab_def.subtabs[active_tab_def.active_subtab];

                    float ly = content_y;
                    for (auto& box : active_sub.left) {
                        float box_top = ly; 
                        ly += BOX_PAD_Y + 12.0f; 
                        RenderElements(box.elements, PADDING + BOX_PAD_X, L_W - BOX_PAD_X * 2, ly, wpos, dl);
                        float box_bottom = ly + 4.0f;
                        DrawBox(dl, wpos, PADDING, box_top, box_bottom, L_W, box.name.c_str());
                        ly = box_bottom + 12.0f; 
                    }

                    float ry = content_y;
                    for (auto& box : active_sub.right) {
                        float box_top = ry;
                        ry += BOX_PAD_Y + 12.0f;
                        RenderElements(box.elements, R_X + BOX_PAD_X, R_W - BOX_PAD_X * 2, ry, wpos, dl);
                        float box_bottom = ry + 4.0f;
                        DrawBox(dl, wpos, R_X, box_top, box_bottom, R_W, box.name.c_str());
                        ry = box_bottom + 12.0f;
                    }
                }
            }
        }

        ImGui::PopStyleColor();

        static bool s_drag = false;
        ImVec2 mp = ImGui::GetIO().MousePos;
        bool overlay_open = (UI::_cp::open_id != 0) || (UI::_dd::open_id != 0) || (UI_MDD::open_id != 0);
        if (!overlay_open && (mp.y < wpos.y + MENU_H) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(0)) s_drag = true;
        if (!ImGui::IsMouseDown(0)) s_drag = false;
        if (s_drag && !overlay_open) ImGui::SetWindowPos(ImVec2(wpos.x + io.MouseDelta.x, wpos.y + io.MouseDelta.y));

        UI::RenderOpenDropdown();
        UI_MDD::RenderOpen();
        UI::RenderOpenColorPicker();

        ApplySettingsToGlobals();

        ImGui::End();
    }
}