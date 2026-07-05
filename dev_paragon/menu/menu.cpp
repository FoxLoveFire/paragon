#include "menu.h"
#include "../external/imgui/imgui.h"
#include "../external/imgui/imgui_internal.h"
#include "elements/elements.h"
#include "../globals.h"
#include "elements/colors.h"
#include <windows.h>
#include <shellapi.h>
#include <vector>
#include <string>

static const float MENU_W       = 660.0f;
static const float MENU_H       = 560.0f;
static const float TAB_H        = 26.0f;
static const float TITLE_H      = 22.0f;
static const float PADDING      = 10.0f;
static const float GAP          = 14.0f;
static const float L_W          = (MENU_W - PADDING * 2.0f - GAP) / 2.0f;
static const float R_X          = PADDING + L_W + GAP;
static const float R_W          = L_W;
static const float BOX_PAD      = 8.0f;
static const float ITEM_SPACING = 34.0f;

static struct State {
    int   active_tab      = 1;
    int   visuals_subtab  = 2;

    bool   override_lights     = false;
    ImVec4 override_lights_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    bool   world_effects       = false;
    int    effects_type        = 0;
    bool   override_fog        = false;
    ImVec4 override_fog_col    = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    bool   override_wall       = false;
    ImVec4 override_wall_col   = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    bool  override_viewmodel_pos = false;
    float viewmodel_x = 0.0f;
    float viewmodel_y = 0.0f;
    float viewmodel_z = 0.0f;
    float viewmodel_fov = 0.0f;

    bool  override_fov = false;
    float fov_val = 90.0f;
    float aspect_ratio = 1.77778f;

    bool  radar_hack = false;

    // removals
    bool disable_smoke         = false;
    bool disable_scopeoverlay  = false;
    bool remove_legs           = false;
    bool disable_flashbang     = false;
} s;

static const char* k_tabs[]            = { "aim", "visuals", "skins", "misc", "config", "lua" };
static const char* k_visuals_subtabs[] = { "enemy", "friendly", "world", "extra" };
static const char* k_effects_types[]   = { "ash", "none", "rain", "snow" };

static void DrawBox(ImDrawList* dl, ImVec2 wpos,
                    float lx, float box_top, float box_bottom, float w,
                    const char* label, ImU32 label_col) {
    auto S = [&](float x, float y) { return ImVec2(wpos.x + x, wpos.y + y); };

    float lh  = ImGui::GetTextLineHeight();
    float gap = 5.0f;

    ImVec2 ts = ImGui::CalcTextSize(label);
    float  tx = lx + 12.0f;
    float  ty = box_top - lh * 0.5f;

    dl->AddLine(S(lx, box_top), S(tx - gap, box_top), Colors::SectionBorder, 1.0f);
    dl->AddLine(S(tx + ts.x + gap, box_top), S(lx + w, box_top), Colors::SectionBorder, 1.0f);

    dl->AddLine(S(lx, box_top), S(lx, box_bottom), Colors::SectionBorder, 1.0f);
    dl->AddLine(S(lx + w, box_top), S(lx + w, box_bottom), Colors::SectionBorder, 1.0f);
    dl->AddLine(S(lx, box_bottom), S(lx + w, box_bottom), Colors::SectionBorder, 1.0f);

    dl->AddRectFilled(S(tx, ty), S(tx + ts.x, ty + lh), Colors::Bg);
    dl->AddText(S(tx, ty), label_col, label);
}

void Menu::Render() {
    UI::UpdateBindCapture();

    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(
        ImVec2((io.DisplaySize.x - MENU_W) * 0.5f,
               (io.DisplaySize.y - (MENU_H + TAB_H)) * 0.5f),
        ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(MENU_W, MENU_H + TAB_H));

    ImGuiWindowFlags wf =
        ImGuiWindowFlags_NoTitleBar        |
        ImGuiWindowFlags_NoResize          |
        ImGuiWindowFlags_NoScrollbar       |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoBackground      |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoMove;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,   ImVec2(0, 0));
    ImGui::Begin("##paragon", nullptr, wf);
    ImGui::PopStyleVar(2);

    float total_h = MENU_H + TAB_H;

    ImGui::SetCursorPos(ImVec2(0, 0));
    ImGui::Dummy(ImVec2(MENU_W, total_h));
    ImGui::SetCursorPos(ImVec2(0, 0));

    ImDrawList* dl   = ImGui::GetWindowDrawList();
    ImVec2      wpos = ImGui::GetWindowPos();
    auto S = [&](float x, float y) { return ImVec2(wpos.x + x, wpos.y + y); };

    dl->AddRectFilled(wpos, ImVec2(wpos.x + MENU_W, wpos.y + total_h), Colors::Bg, 4.0f);
    dl->AddRectFilled(wpos, ImVec2(wpos.x + MENU_W, wpos.y + TITLE_H), Colors::TitleBg, 0.0f);

    ImVec2 grad_top = wpos;
    ImVec2 grad_bot = ImVec2(wpos.x + MENU_W, wpos.y + TITLE_H);
    dl->AddRectFilledMultiColor(
        grad_top, grad_bot,
        IM_COL32(20, 20, 20, 255), IM_COL32(20, 20, 20, 255),
        IM_COL32(7, 7, 7, 255), IM_COL32(7, 7, 7, 255));

    UI::DrawGradientLine(dl, wpos.x, wpos.y + TITLE_H, MENU_W, 1.0f);

    {
        const char* title = "paragon";
        ImVec2 ts = ImGui::CalcTextSize(title);
        dl->AddText(S((MENU_W - ts.x) * 0.5f, (TITLE_H - ts.y) * 0.5f),
                    IM_COL32(185, 182, 196, 255), title);
    }

    ImGui::SetCursorPos(ImVec2(0, 0));
    ImGui::InvisibleButton("##titlebar", ImVec2(MENU_W, TITLE_H));
    if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        ImVec2 delta = ImGui::GetIO().MouseDelta;
        ImVec2 new_pos = ImVec2(wpos.x + delta.x, wpos.y + delta.y);
        ImGui::SetWindowPos(new_pos);
    }

    const float lh = ImGui::GetTextLineHeight();

    if (s.active_tab == 1) {
        float subtab_y = TITLE_H + 4.0f;
        s.visuals_subtab = UI::SubTabBar(k_visuals_subtabs, 4, s.visuals_subtab,
                                         ImVec2(wpos.x, wpos.y + subtab_y), PADDING);

        float content_y = TITLE_H + lh + 18.0f;
        float usable_h = MENU_H - content_y - PADDING;

        ImGui::SetCursorPos(ImVec2(0, content_y));

        if (s.visuals_subtab == 2) {
            float r_sr = wpos.x + R_X + R_W - BOX_PAD;
            float rbox_top = content_y;
            float rbox_bottom = content_y + usable_h;
            float ry = rbox_top + BOX_PAD + 3.0f;

            ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
            UI::Checkbox("override lights", &s.override_lights, nullptr, nullptr, R_W - BOX_PAD * 2 - 22.0f);
            ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
            UI::ColorPicker("##lights_col", &s.override_lights_col, r_sr, 0.0f);
            ry += ITEM_SPACING;

            ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
            UI::Checkbox("effects", &s.world_effects, nullptr, nullptr, R_W - BOX_PAD * 2);
            ry += ITEM_SPACING;

            if (s.world_effects) {
                ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
                UI::Dropdown("##effects_type", &s.effects_type, k_effects_types, 4, R_W - BOX_PAD * 2);
                ry += ITEM_SPACING;
            }

            ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
            UI::Checkbox("override fog", &s.override_fog, nullptr, nullptr, R_W - BOX_PAD * 2 - 22.0f);
            ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
            UI::ColorPicker("##fog_col", &s.override_fog_col, r_sr, 0.0f);
            ry += ITEM_SPACING;

            ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
            UI::Checkbox("override wall", &s.override_wall, nullptr, nullptr, R_W - BOX_PAD * 2 - 22.0f);
            ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
            UI::ColorPicker("##wall_col", &s.override_wall_col, r_sr, 0.0f);
            ry += ITEM_SPACING;

            DrawBox(dl, wpos, R_X, rbox_top, rbox_bottom, R_W, "effects", Colors::ColHdr);
        }
        else if (s.visuals_subtab == 3) {
            float lbox_top = content_y;
            float lbox_bottom = content_y + usable_h;
            float ly = lbox_top + BOX_PAD + 3.0f;

            ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
            UI::Checkbox("override viewmodel position", &s.override_viewmodel_pos, nullptr, nullptr, L_W - BOX_PAD * 2);
            ly += ITEM_SPACING;

            if (s.override_viewmodel_pos) {
                ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
                UI::SliderFloat("##vm_x", &s.viewmodel_x, -20.0f, 20.0f, "\xC2\xB0", "%.1f", L_W - BOX_PAD * 2);
                ly += ITEM_SPACING;

                ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
                UI::SliderFloat("##vm_y", &s.viewmodel_y, -20.0f, 20.0f, "\xC2\xB0", "%.1f", L_W - BOX_PAD * 2);
                ly += ITEM_SPACING;

                ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
                UI::SliderFloat("##vm_z", &s.viewmodel_z, -20.0f, 20.0f, "\xC2\xB0", "%.1f", L_W - BOX_PAD * 2);
                ly += ITEM_SPACING;

                ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
                UI::SliderFloat("##vm_fov", &s.viewmodel_fov, -30.0f, 30.0f, "\xC2\xB0", "%.1f", L_W - BOX_PAD * 2);
                ly += ITEM_SPACING;
            }

            ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
            UI::Checkbox("override fov", &s.override_fov, nullptr, nullptr, L_W - BOX_PAD * 2);
            ly += ITEM_SPACING;

            if (s.override_fov) {
                ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
                UI::SliderFloat("##fov", &s.fov_val, 0.0f, 100.0f, "\xC2\xB0", "%.1f", L_W - BOX_PAD * 2);
                ly += ITEM_SPACING;

                ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
                UI::SliderFloat("##aspect", &s.aspect_ratio, 0.5f, 2.5f, "ratio", "%.2f", L_W - BOX_PAD * 2);
                ly += ITEM_SPACING;
            }

            DrawBox(dl, wpos, PADDING, lbox_top, lbox_bottom, L_W, "view", Colors::ColHdr);

            float rbox_top = content_y;
            float rbox_bottom = content_y + usable_h;
            float ry = rbox_top + BOX_PAD + 3.0f;

            ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
            UI::Checkbox("radar hack", &s.radar_hack, nullptr, nullptr, R_W - BOX_PAD * 2);
            ry += ITEM_SPACING;

            ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
            UI::Checkbox("disable smoke", &s.disable_smoke, nullptr, nullptr, R_W - BOX_PAD * 2);
            ry += ITEM_SPACING;

            ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
            UI::Checkbox("disable scope overlay", &s.disable_scopeoverlay, nullptr, nullptr, R_W - BOX_PAD * 2);
            ry += ITEM_SPACING;

            ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
            UI::Checkbox("remove legs", &s.remove_legs, nullptr, nullptr, R_W - BOX_PAD * 2);
            ry += ITEM_SPACING;

            ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
            UI::Checkbox("disable flashbang", &s.disable_flashbang, nullptr, nullptr, R_W - BOX_PAD * 2);
            ry += ITEM_SPACING;

            DrawBox(dl, wpos, R_X, rbox_top, rbox_bottom, R_W, "hud", Colors::ColHdr);
        }
    }

    s.active_tab = UI::TabBar(k_tabs, 6, s.active_tab,
                               ImVec2(wpos.x, wpos.y + MENU_H), MENU_W, TAB_H);

    {
        static bool s_drag = false;
        ImVec2 mp = ImGui::GetIO().MousePos;
        bool overlay_open = (UI::_cp::open_id != 0) || (UI::_dd::open_id != 0);
        bool over_tabbar  = (mp.y >= wpos.y + MENU_H);

        if (!overlay_open && !over_tabbar &&
            ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() &&
            ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            s_drag = true;
        }
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
            s_drag = false;

        if (s_drag && !overlay_open) {
            ImVec2 d = ImGui::GetIO().MouseDelta;
            ImGui::SetWindowPos(ImVec2(wpos.x + d.x, wpos.y + d.y));
        }
    }

    UI::RenderOpenDropdown();
    UI::RenderOpenColorPicker();

    G::world.light_enabled = s.override_lights;
    G::world.light_intensity = s.override_lights_col.w;
    G::world.light_color = c_color(
        (int)(s.override_lights_col.x * 255.0f),
        (int)(s.override_lights_col.y * 255.0f),
        (int)(s.override_lights_col.z * 255.0f),
        255
    );

    G::world.wall_enabled = s.override_wall;
    G::world.wall_brightness = s.override_wall_col.w;
    G::world.wall_color = c_color(
        (int)(s.override_wall_col.x * 255.0f),
        (int)(s.override_wall_col.y * 255.0f),
        (int)(s.override_wall_col.z * 255.0f),
        255
    );

    G::world.enabled = G::world.light_enabled || G::world.wall_enabled;

    // --- RADAR ---
    G::misc.rh_enabled = s.radar_hack;

    // --- VIEWMODEL ---
    G::viewmodel.is_enabled = s.override_viewmodel_pos;

    if (G::viewmodel.xyz.size() < 3)
        G::viewmodel.xyz.resize(3);

    G::viewmodel.xyz[0] = s.viewmodel_x;
    G::viewmodel.xyz[1] = s.viewmodel_y;
    G::viewmodel.xyz[2] = s.viewmodel_z;
    G::viewmodel.v_fov  = s.viewmodel_fov;

    // --- OVERRIDE VIEW ---
    G::viewport.custom_fov   = s.override_fov;
    G::viewport.fov          = s.fov_val;
    G::viewport.aspect_ratio = s.aspect_ratio;

    // --- REMOVALS ---
    G::removals.disable_smoke        = s.disable_smoke;
    G::removals.disable_scopeoverlay = s.disable_scopeoverlay;
    G::removals.remove_legs          = s.remove_legs;
    G::removals.disable_flashbang    = s.disable_flashbang;

    ImGui::End();
}