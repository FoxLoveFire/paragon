#include "menu.h"
#include "menu_layout.h"
#include "../../../external/imgui/imgui.h"
#include "../../../external/imgui/imgui_internal.h"
#include "../../../globals.h"
#include "../../../external/nlohmann/single_include/nlohmann/json.hpp"

using json = nlohmann::json;

namespace UI_MDD {
    inline ImGuiID open_id = 0;
    inline int open_frame = -1;
    inline ImVec2 open_pos = {};
    inline float open_w = 0.f;
    inline std::vector<std::string> items;
    inline int* selected_mask = nullptr;

    inline bool MultiDropdown(const char* label, int* sel, const std::vector<std::string>& items, float width, ImVec2 pos, ImDrawList* dl) {
        const float h = 17.0f;
        ImGui::SetCursorScreenPos(pos);
        ImGui::InvisibleButton(label, ImVec2(width, h));
        bool clicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);
        ImGuiID my_id = ImGui::GetID(label);

        if (clicked) {
            if (open_id == my_id) {
                open_id = 0;
            } else {
                open_id = my_id;
                open_frame = ImGui::GetFrameCount();
                open_pos = ImVec2(pos.x, pos.y + h);
                open_w = width;
                UI_MDD::items = items;
                selected_mask = sel;
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
        for (size_t i = 0; i < items.size(); i++) {
            if (*sel & (1 << i)) {
                if (active_count > 0) preview += ", ";
                preview += items[i];
                active_count++;
            }
        }
        if (active_count == 0) preview = "none";
        else if (active_count == items.size()) preview = "all";

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
        if (open_id == 0) return;

        const float item_h = 16.0f;
        float total = (float)items.size() * item_h + 4.0f;
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

        for (size_t i = 0; i < items.size(); i++) {
            ImVec2 ip0 = ImVec2(p0.x + 1.0f, p0.y + 2.0f + (float)i * item_h);
            ImVec2 ip1 = ImVec2(p1.x - 1.0f, ip0.y + item_h);

            bool hov_i = (mouse.x >= ip0.x && mouse.x <= ip1.x && mouse.y >= ip0.y && mouse.y <= ip1.y);
            bool sel_i = (*selected_mask & (1 << i));

            if (hov_i) dl->AddRectFilled(ip0, ip1, IM_COL32(35, 32, 44, 255), 2.0f);

            float ty = ip0.y + (item_h - ImGui::GetTextLineHeight()) * 0.5f;
            ImU32 tc = sel_i ? Colors::Accent : IM_COL32(255, 255, 255, 255);
            dl->AddText(ImVec2(ip0.x + 7.0f, ty), tc, items[i].c_str());

            if (hov_i && click_any) click_sel = (int)i;
        }

        ImGui::End();
        ImGui::PopStyleVar(3);

        if (click_sel >= 0) {
            if (*selected_mask & (1 << click_sel)) *selected_mask &= ~(1 << click_sel);
            else *selected_mask |= (1 << click_sel);
        } else if (click_any) {
            bool inside = (mouse.x >= p0.x && mouse.x <= p1.x && mouse.y >= p0.y && mouse.y <= p1.y);
            if (!inside) open_id = 0;
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
    static const float PADDING = 10.0f, GAP = 14.0f, BOX_PAD = 8.0f, ITEM_SPACING = 34.0f;
    static const float L_W = (MENU_W - PADDING * 2.0f - GAP) / 2.0f;
    static const float R_X = PADDING + L_W + GAP, R_W = L_W;

    void LoadConfigLayout() {
        json j = json::parse(json_layout);
        g_MenuLayout.clear();

        for (auto& t : j["tabs"]) {
            TabDef tab; tab.name = t["name"].get<std::string>();
            if (t.contains("subtabs")) {
                for (auto& st : t["subtabs"]) {
                    SubTabDef subtab; subtab.name = st["name"].get<std::string>();

                    auto parse_box = [&](json& boxes, std::vector<GroupBoxDef>& target) {
                        for (auto& gb : boxes) {
                            GroupBoxDef box; box.name = gb["name"].get<std::string>();
                            for (auto& el : gb["elements"]) {
                                ElementDef elem;
                                elem.type = el["type"]; elem.id = el["id"]; elem.label = el.value("label", "");
                                elem.condition = el.value("condition", "");
                                elem.suffix = el.value("suffix", ""); elem.format = el.value("format", "%.1f");
                                elem.min = el.value("min", 0.f); elem.max = el.value("max", 100.f);
                                
                                if (el.contains("color")) {
                                    elem.color_ids.push_back(el["color"].get<std::string>());
                                }
                                if (el.contains("colors")) {
                                    for (auto& c : el["colors"]) elem.color_ids.push_back(c.get<std::string>());
                                }
                                
                                if (el.contains("items"))
                                    for (auto& item : el["items"]) elem.items.push_back(item.get<std::string>());
                                
                                if (el.contains("default")) {
                                    if (elem.type == "slider") cfg[elem.id].f = el["default"];
                                    else if (elem.type == "checkbox") cfg[elem.id].b = el["default"];
                                    else if (elem.type == "dropdown") cfg[elem.id].i = el["default"];
                                }
                                if (el.contains("default_color") && !elem.color_ids.empty()) {
                                    auto c = el["default_color"];
                                    cfg[elem.color_ids[0]].c = ImVec4(c[0], c[1], c[2], c[3]);
                                }
                                box.elements.push_back(elem);
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
    }

    void Initialize() {
        cfg["active_tab"].i = 1;
        cfg["visuals_subtab"].i = 2;
        LoadConfigLayout();
        g_ActiveTab = cfg["active_tab"].i;
        if (g_ActiveTab >= 0 && g_ActiveTab < g_MenuLayout.size()) {
            if (cfg["visuals_subtab"].i >= 0 && cfg["visuals_subtab"].i < g_MenuLayout[g_ActiveTab].subtabs.size()) {
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

    void RenderElements(std::vector<ElementDef>& elements, float x, float w, float& y, ImVec2 wpos, ImDrawList* dl) {
        for (auto& el : elements) {
            if (!el.condition.empty() && !cfg[el.condition].b) continue;

            ImGui::SetCursorPos(ImVec2(x, y));
            if (el.type == "checkbox") {
                float check_w = el.color_ids.empty() ? w : w - (22.0f * el.color_ids.size());
                UI::Checkbox(el.label.c_str(), &cfg[el.id].b, nullptr, nullptr, check_w);
                for (size_t i = 0; i < el.color_ids.size(); i++) {
                    ImGui::SetCursorPos(ImVec2(x, y));
                    UI::ColorPicker(("##" + el.color_ids[i]).c_str(), &cfg[el.color_ids[i]].c, wpos.x + x + w - ((el.color_ids.size() - i - 1) * 22.0f), 0.0f, &cfg[el.color_ids[i]].i_c);
                }
                y += ITEM_SPACING;
            } else if (el.type == "slider") {
                UI::SliderFloat(el.label.c_str(), &cfg[el.id].f, el.min, el.max, el.suffix.c_str(), el.format.c_str(), w);
                y += ITEM_SPACING;
            } else if (el.type == "dropdown") {
                std::vector<const char*> cstrs;
                for (auto& str : el.items) cstrs.push_back(str.c_str());
                UI::Dropdown(("##" + el.id).c_str(), &cfg[el.id].i, cstrs.data(), cstrs.size(), w);
                y += ITEM_SPACING;
            } else if (el.type == "multidropdown") {
                dl->AddText(ImVec2(wpos.x + x, wpos.y + y), IM_COL32(255, 255, 255, 255), el.label.c_str());
                y += ImGui::GetTextLineHeight() + 4.0f;
                
                float dd_w = w * 0.85f;
                float offset_x = (w - dd_w) * 0.5f;

                UI_MDD::MultiDropdown(("##" + el.id).c_str(), &cfg[el.id].i, el.items, dd_w, ImVec2(wpos.x + x + offset_x, wpos.y + y), dl);
                
                y += 17.0f + ITEM_SPACING - 10.0f;
            }
        }
    }

    void Render() {
        ImGuiContext* ctx = ImGui::GetCurrentContext();
        if (!ctx || !ctx->WithinFrameScope) return;

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

        dl->AddRectFilled(wpos, ImVec2(wpos.x + MENU_W, wpos.y + total_h), Colors::Bg, 4.0f);
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
        
        g_ActiveTab = UI::TabBar(tab_names.data(), tab_names.size(), g_ActiveTab, ImVec2(wpos.x, wpos.y + MENU_H), MENU_W, TAB_H);

        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));

        if (g_ActiveTab >= 0 && g_ActiveTab < g_MenuLayout.size()) {
            auto& active_tab_def = g_MenuLayout[g_ActiveTab];
            float content_y = TITLE_H + 18.0f;

            if (!active_tab_def.subtabs.empty()) {
                std::vector<const char*> subtab_names;
                for (auto& st : active_tab_def.subtabs) subtab_names.push_back(st.name.c_str());
                
                active_tab_def.active_subtab = UI::SubTabBar(subtab_names.data(), subtab_names.size(), active_tab_def.active_subtab, ImVec2(wpos.x, wpos.y + TITLE_H + 4.0f), PADDING);
                content_y += ImGui::GetTextLineHeight() + 18.0f;

                auto& active_sub = active_tab_def.subtabs[active_tab_def.active_subtab];

                float ly = content_y + 8.0f;
                for (auto& box : active_sub.left) {
                    float box_top = ly; 
                    ly += BOX_PAD + 12.0f; 
                    RenderElements(box.elements, PADDING + BOX_PAD, L_W - BOX_PAD * 2, ly, wpos, dl);
                    float box_bottom = ly + 4.0f;
                    DrawBox(dl, wpos, PADDING, box_top, box_bottom, L_W, box.name.c_str());
                    ly = box_bottom + 12.0f; 
                }

                float ry = content_y + 8.0f;
                for (auto& box : active_sub.right) {
                    float box_top = ry;
                    ry += BOX_PAD + 12.0f;
                    RenderElements(box.elements, R_X + BOX_PAD, R_W - BOX_PAD * 2, ry, wpos, dl);
                    float box_bottom = ry + 4.0f;
                    DrawBox(dl, wpos, R_X, box_top, box_bottom, R_W, box.name.c_str());
                    ry = box_bottom + 12.0f;
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

        auto to_c_color = [](const ConfigValue& cv) {
            return c_color(cv.c.x * 255.0f * cv.i_c, cv.c.y * 255.0f * cv.i_c, cv.c.z * 255.0f * cv.i_c, cv.c.w * 255.0f);
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

        ImGui::End();
    }
}