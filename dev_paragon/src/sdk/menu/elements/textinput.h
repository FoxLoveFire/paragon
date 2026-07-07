#pragma once
#include "../../../../external/imgui/imgui.h"
#include "../../../../external/imgui/imgui_internal.h"
#include "colors.h"
#include <cstring>

namespace UI {

namespace _ti {
    inline ImGuiID focused_id = 0;
}

inline bool TextInput(const char* id, char* buf, int buf_size, float w, float h = 17.0f) {
    ImDrawList* dl    = ImGui::GetWindowDrawList();
    ImVec2      pos   = ImGui::GetCursorScreenPos();
    ImGuiID     my_id = ImGui::GetID(id);
    ImVec2      p1    = ImVec2(pos.x + w, pos.y + h);

    ImGui::InvisibleButton(id, ImVec2(w, h));
    bool hov     = ImGui::IsItemHovered();
    bool clicked = ImGui::IsItemClicked();

    if (clicked)
        _ti::focused_id = (_ti::focused_id == my_id) ? 0 : my_id;
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !hov && _ti::focused_id == my_id)
        _ti::focused_id = 0;

    bool focused = (_ti::focused_id == my_id);
    bool changed = false;

    if (focused) {
        ImGuiIO& io = ImGui::GetIO();
        io.WantCaptureKeyboard = true;
        int len = (int)strlen(buf);

        if (ImGui::IsKeyPressed(ImGuiKey_Backspace) && len > 0) {
            while (len > 0) {
                len--;
                if ((buf[len] & 0xC0) != 0x80)
                    break;
            }
            buf[len] = '\0';
            changed = true;
        }

        if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_V)) {
            if (const char* clip = ImGui::GetClipboardText()) {
                while (*clip && len < buf_size - 1) {
                    buf[len++] = *clip++;
                }
                buf[len] = '\0';
                changed = true;
            }
        }

        for (int i = 0; i < io.InputQueueCharacters.Size; i++) {
            ImWchar c = io.InputQueueCharacters[i];
            if (c < 32) continue;

            char char_buf[4];
            int bytes = 0;
            if (c < 0x80) {
                char_buf[0] = (char)c;
                bytes = 1;
            } else if (c < 0x800) {
                char_buf[0] = (char)(0xC0 + (c >> 6));
                char_buf[1] = (char)(0x80 + (c & 0x3F));
                bytes = 2;
            } else if (c < 0x10000) {
                char_buf[0] = (char)(0xE0 + (c >> 12));
                char_buf[1] = (char)(0x80 + ((c >> 6) & 0x3F));
                char_buf[2] = (char)(0x80 + (c & 0x3F));
                bytes = 3;
            }

            if (len + bytes < buf_size) {
                for (int b = 0; b < bytes; b++) {
                    buf[len++] = char_buf[b];
                }
                buf[len] = '\0';
                changed = true;
            }
        }
        if (changed) io.InputQueueCharacters.resize(0);
    }

    dl->AddRectFilledMultiColor(pos, p1,
        IM_COL32(20, 18, 23, 255), IM_COL32(20, 18, 23, 255),
        IM_COL32(9, 8, 10, 255),   IM_COL32(9, 8, 10, 255));

    ImU32 bord = focused ? Colors::Accent
               : hov     ? Colors::CbBorderHov
               :            Colors::CbBorder;
    dl->AddRect(pos, p1, bord, 2.0f, 0, 1.0f);

    int   len  = (int)strlen(buf);
    float ty   = pos.y + (h - ImGui::GetTextLineHeight()) * 0.5f;
    
    ImGui::PushClipRect(ImVec2(pos.x + 2.0f, pos.y), ImVec2(p1.x - 2.0f, p1.y), true);
    
    if (len > 0)
        dl->AddText(ImVec2(pos.x + 6.0f, ty), Colors::TextBright, buf);
    else if (!focused)
        dl->AddText(ImVec2(pos.x + 6.0f, ty), Colors::TextDim, "...");

    if (focused && (int)(ImGui::GetTime() * 2.0) % 2 == 0) {
        ImVec2 ts = ImGui::CalcTextSize(buf, buf + len);
        float  cx = pos.x + 6.0f + ts.x + 1.0f;
        dl->AddLine(ImVec2(cx, ty), ImVec2(cx, ty + ImGui::GetTextLineHeight()),
                    Colors::TextBright, 1.0f);
    }
    
    ImGui::PopClipRect();

    return changed;
}

}