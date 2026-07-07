#pragma once
#include <string>

inline const std::string json_layout = R"(
{
  "tabs": [
    { "name": "aim" },
    {
      "name": "visuals",
      "subtabs": [
        {
          "name": "world",
          "right": [
            {
              "name": "effects",
              "elements": [
                { "type": "multidropdown", "id": "removals", "label": "removals", "items": ["smoke", "flashbang", "scope overlay", "legs"] },
                { "type": "checkbox", "id": "override_lights", "label": "override lights", "color": "override_lights_col" },
                { "type": "checkbox", "id": "world_effects", "label": "effects" },
                { "type": "dropdown", "id": "effects_type", "items": ["ash", "none", "rain", "snow"], "condition": "world_effects" },
                { "type": "checkbox", "id": "override_fog", "label": "override fog", "color": "override_fog_col" },
                { "type": "checkbox", "id": "override_wall", "label": "override wall", "color": "override_wall_col" },
                { "type": "checkbox", "id": "override_skybox", "label": "override skybox", "colors": ["override_skybox_col1", "override_skybox_col2", "override_skybox_col3"] }
              ]
            }
          ]
        },
        {
          "name": "extra",
          "left": [
            {
              "name": "view",
              "elements": [
                { "type": "checkbox", "id": "override_viewmodel_pos", "label": "override viewmodel position" },
                { "type": "slider", "id": "viewmodel_x", "label": "##vm_x", "min": -20.0, "max": 20.0, "format": "%.1f", "suffix": "°", "condition": "override_viewmodel_pos" },
                { "type": "slider", "id": "viewmodel_y", "label": "##vm_y", "min": -20.0, "max": 20.0, "format": "%.1f", "suffix": "°", "condition": "override_viewmodel_pos" },
                { "type": "slider", "id": "viewmodel_z", "label": "##vm_z", "min": -20.0, "max": 20.0, "format": "%.1f", "suffix": "°", "condition": "override_viewmodel_pos" },
                { "type": "slider", "id": "viewmodel_fov", "label": "##vm_fov", "min": -30.0, "max": 30.0, "format": "%.1f", "suffix": "°", "condition": "override_viewmodel_pos" },
                { "type": "checkbox", "id": "override_fov", "label": "override fov" },
                { "type": "slider", "id": "fov_val", "label": "##fov", "min": 0.0, "max": 120.0, "default": 90.0, "format": "%.1f", "suffix": "°", "condition": "override_fov" },
                { "type": "slider", "id": "aspect_ratio", "label": "##aspect", "min": 0.5, "max": 2.5, "format": "%.2f", "suffix": "ratio", "condition": "override_fov" }
              ]
            }
          ],
          "right": [
            {
              "name": "hud",
              "elements": [
                { "type": "checkbox", "id": "watermark", "label": "watermark", "color": "watermark_color", "default": true },
                { "type": "multidropdown", "id": "watermark_options", "label": "options", "items": ["name", "ping", "time", "sync color"], "condition": "watermark", "default": 15 },
                { "type": "dropdown", "id": "watermark_name_source", "label": "name source", "items": ["loader", "steam"], "condition": "watermark", "default": 0 },
                { "type": "checkbox", "id": "radar_hack", "label": "radar hack" }
              ]
            }
          ]
        }
      ]
    },
    { "name": "players" },
    { "name": "skins" },
    { "name": "misc" },
    { 
      "name": "config",
      "subtabs": [
        {
          "name": "main",
          "left": [
            {
              "name": "settings",
              "elements": [
                { "type": "colorpicker", "id": "menu_color", "label": "menu color", "color": "menu_color", "default_color": [0.686, 0.392, 0.784, 1.0] },
                { "type": "checkbox", "id": "keybinds_list", "label": "keybinds list" },
                { "type": "checkbox", "id": "spectator_list", "label": "spectator list" },
                { "type": "overlay_image", "id": "custom_overlay", "label": "overlay image" }
              ]
            }
          ],
          "right": [
            {
              "name": "configs",
              "elements": [
                { "type": "config_list", "id": "cfg_sys" }
              ]
            }
          ]
        }
      ]
    },
    { "name": "lua" }
  ]
}
)";