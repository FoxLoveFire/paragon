#pragma once
#include <vector>
#include "src/sdk/math/color.h"

namespace G
{
    struct Menu_t
    {
        bool show = false;
    };

    inline Menu_t menu = Menu_t();

    struct WorldModulation_t
    {
        bool    enabled         = false;
        bool    light_enabled   = false;
        bool    wall_enabled    = false;
        
        c_color light_color     = c_color(255, 0, 255);
        c_color wall_color      = c_color(0, 255, 0);
        c_color prop_color      = c_color(255, 255, 255);
        
        float   light_intensity = 1.0f;
        float   wall_brightness = 1.0f;

        int     m_exposure      = 0; 
    };

    inline WorldModulation_t world = WorldModulation_t();

    struct Viewport_t
    {
        bool  custom_fov   = false;
        float fov          = 90.0f;
        float aspect_ratio = 1.77778f;
    };

    inline Viewport_t viewport = Viewport_t();

    struct Viewmodel_t
    {
        bool               is_enabled = false;
        std::vector<float> xyz        = {0.0f, 0.0f, 0.0f};
        float              v_fov      = 0.0f;
    };

    inline Viewmodel_t viewmodel = Viewmodel_t();

    struct Removals_t
    {
        bool disable_smoke        = false;
        bool disable_scopeoverlay = false;
        bool remove_legs          = false;
        bool disable_flashbang    = false;
    };

    inline Removals_t removals = Removals_t();

    struct Misc_t
    {
        bool rh_enabled = false;
    };

    inline Misc_t misc = Misc_t();
}