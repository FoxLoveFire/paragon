#pragma once
#include "../../../../external/imgui/imgui.h"
#include "../../../../external/imgui/imgui_internal.h"
#include "colors.h"
#include "bind.h"
#include <unordered_map>
#include <string>
#include <cmath>

namespace UI {

inline bool Checkbox(const char* label, bool* v,
                     const char* bind_id      = nullptr,
                     const char* default_bind = nullptr,
                     float       col_w        = -1.f) {
    ImDrawList* dl    = ImGui::GetWindowDrawList();
    ImVec2      pos   = ImGui::GetCursorScreenPos();
    float       width = (col_w > 0.f) ? col_w : ImGui::GetContentRegionAvail().x;

    const float row_h = 16.0f;
    const float cb    = 9.0f;

    if (bind_id) UI::InitBind(bind_id, default_bind);

    float bind_w = 0.f;
    if (bind_id) {
        const char* bd = UI::GetBindDisplay(bind_id);
        bind_w = ImGui::CalcTextSize(bd).x + 6.f;
    }

    float cb_row_w = width - bind_w;
    ImGui::SetCursorScreenPos(pos);
    ImGui::InvisibleButton(label, ImVec2(cb_row_w, row_h));
    bool hov     = ImGui::IsItemHovered();
    bool clicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);
    if (clicked) *v = !(*v);

    float anim = *v ? 1.0f : 0.0f;

    ImVec2 cp0 = ImVec2(pos.x,      pos.y + (row_h - cb) * 0.5f);
    ImVec2 cp1 = ImVec2(pos.x + cb, cp0.y + cb);

    dl->AddRectFilled(cp0, cp1, Colors::CbBg, 1.5f);

    if (anim > 0.02f) {
        float pad = 0.0f;
        ImVec4 bg  = ImGui::ColorConvertU32ToFloat4(Colors::CbBg);
        ImVec4 acc = ImGui::ColorConvertU32ToFloat4(Colors::Accent);
        ImU32 fill = ImGui::ColorConvertFloat4ToU32(ImLerp(bg, acc, anim));
        dl->AddRectFilled(
            ImVec2(cp0.x + 1.f + pad, cp0.y + 1.f + pad),
            ImVec2(cp1.x - 1.f - pad, cp1.y - 1.f - pad),
            fill, 1.0f);
    }

    ImVec4 acc4  = ImGui::ColorConvertU32ToFloat4(Colors::Accent);
    ImU32  bord  = hov
        ? Colors::CbBorderHov
        : (anim > 0.5f
            ? ImGui::ColorConvertFloat4ToU32(ImVec4(acc4.x, acc4.y, acc4.z, 0.72f))
            : Colors::CbBorder);
    dl->AddRect(cp0, cp1, bord, 1.5f, 0, 1.0f);

    if (anim > 0.02f) {
        ImVec2 grad_top = ImVec2(cp0.x + 1.f, cp0.y + 1.f);
        ImVec2 grad_bot = ImVec2(cp1.x - 1.f, cp0.y + 1.f + (cb - 2.f) * 0.35f);
        dl->AddRectFilledMultiColor(
            grad_top, grad_bot,
            IM_COL32(255, 255, 255, 20), IM_COL32(255, 255, 255, 20),
            IM_COL32(255, 255, 255, 60), IM_COL32(255, 255, 255, 60));
    }

    float  tly = pos.y + (row_h - ImGui::GetTextLineHeight()) * 0.5f;
    dl->AddText(ImVec2(pos.x + cb + 6.f, tly),
                hov ? Colors::TextBright : Colors::Text, label);

    if (bind_id) {
        bool    waiting  = UI::IsBindWaiting(bind_id);
        const char* bd   = UI::GetBindDisplay(bind_id);
        ImVec2  bts      = ImGui::CalcTextSize(bd);
        float   bx       = pos.x + width - bts.x;

        std::string btn_id = std::string("##bnd_") + bind_id;
        ImGui::SetCursorScreenPos(ImVec2(bx - 3.f, pos.y));
        ImGui::InvisibleButton(btn_id.c_str(), ImVec2(bts.x + 6.f, row_h));
        bool bhov     = ImGui::IsItemHovered();
        
        bool bclicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);
        bool rclicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);

        if (bclicked) {
            if (waiting) UI::g_binds[bind_id].waiting = false;
            else         UI::StartBindWaiting(bind_id);
        }

        std::string popup_id = btn_id + "_mode_popup";
        if (rclicked) {
            ImGui::OpenPopup(popup_id.c_str());
        }

        static std::unordered_map<std::string, float> pulse;
        float& ph = pulse[bind_id];
        if (waiting) ph = fmodf(ph + ImGui::GetIO().DeltaTime * 3.f, 6.2832f);

        ImU32 bc = waiting
            ? ImGui::ColorConvertFloat4ToU32(ImVec4(
                acc4.x, acc4.y, acc4.z,
                0.55f + 0.45f * sinf(ph)))
            : (bhov ? Colors::Text : Colors::TextBind);

        dl->AddText(ImVec2(bx, tly), bc, bd);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 4.0f);
        ImGui::PushStyleColor(ImGuiCol_PopupBg, ImGui::ColorConvertU32ToFloat4(Colors::Bg));
        ImGui::PushStyleColor(ImGuiCol_Border, ImGui::ColorConvertU32ToFloat4(Colors::CbBorder));
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::ColorConvertU32ToFloat4(Colors::TextBright));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImGui::ColorConvertU32ToFloat4(Colors::CbBg));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImGui::ColorConvertU32ToFloat4(Colors::AccentDark));

        if (ImGui::BeginPopup(popup_id.c_str())) {
            auto& bnd = UI::g_binds[bind_id];

            if (ImGui::Selectable("Toggle", bnd.mode == BindMode::TOGGLE)) {
                bnd.mode = BindMode::TOGGLE;
                bnd.waiting = false;
            }
            if (ImGui::Selectable("Hold", bnd.mode == BindMode::HOLD)) {
                bnd.mode = BindMode::HOLD;
                bnd.waiting = false;
            }
            if (ImGui::Selectable("Always On", bnd.mode == BindMode::ALWAYS_ON)) {
                bnd.mode = BindMode::ALWAYS_ON;
                bnd.waiting = false;
            }
            if (ImGui::Selectable("Disabled", bnd.mode == BindMode::DISABLED)) {
                bnd.mode = BindMode::DISABLED;
                bnd.waiting = false;
            }

            ImGui::EndPopup();
        }

        ImGui::PopStyleColor(5);
        ImGui::PopStyleVar(2);
    }

    ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y + row_h + 3.0f));
    return clicked;
}

}