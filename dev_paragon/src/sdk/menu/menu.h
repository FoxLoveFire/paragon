#pragma once
#include "../../../globals.h"
#include <windows.h>
#include <shellapi.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "elements/elements.h"
#include "elements/colors.h"
#include "../../../external/imgui/imgui.h"
#include "../../../external/imgui/imgui_internal.h"

namespace Menu {
    struct ConfigValue {
        bool b = false;
        float f = 0.0f;
        int i = 0;
        ImVec4 c = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        float i_c = 1.0f;
        std::string s = "";
    };

    extern std::unordered_map<std::string, ConfigValue> cfg;

    void Initialize();
    void Render();
}