#pragma once
#include "../../sdk/Interfaces/c_scene_object.h"
#include "../../sdk/Interfaces/Interfaces.h"
#include "../../sdk/math/color.h"
#include "../../../globals.h"
#include <unordered_map>

namespace WorldModulation
{
    typedef void(__fastcall* DrawArrayLightFn)(__int64 a1, CSceneLightObject* a2, __int64 a3);
    inline DrawArrayLightFn oDrawArrayLight = nullptr;
    void __fastcall hkDrawArrayLight(__int64 a1, CSceneLightObject* a2, __int64 a3);

    typedef __int64(__fastcall* WallColor)(__int64 a1, __int64 a2, c_aggregate_object_array* a3);
    inline WallColor oWallColor = nullptr;
    __int64 __fastcall hkWallColor(__int64 a1, __int64 a2, c_aggregate_object_array* a3);

    typedef void(__fastcall* ToneMapFn)(void*, void*, float);
    inline ToneMapFn oToneMap = nullptr;
    void __fastcall hk_tonemap_render_debug(void* rcx, void* rdx, float scalar);

    struct LightColor {
        float r, g, b;
    };

    inline std::unordered_map<uintptr_t, LightColor> light_cache;
}