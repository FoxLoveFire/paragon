#include "WorldModulation.h"

void WorldModulation::hkDrawArrayLight(__int64 var1, CSceneLightObject* pSceneLight, __int64 var3)
{
    if (!pSceneLight)
    {
        if (oDrawArrayLight)
            oDrawArrayLight(var1, pSceneLight, var3);
        return;
    }

    uintptr_t light_ptr = reinterpret_cast<uintptr_t>(pSceneLight);

    if (WorldModulation::light_cache.find(light_ptr) == WorldModulation::light_cache.end())
    {
        WorldModulation::light_cache[light_ptr] = {pSceneLight->desc.colorR, pSceneLight->desc.colorG, pSceneLight->desc.colorB};
    }

    if (G::world.enabled && G::world.light_enabled)
    {
        float intensity = G::world.light_intensity;
        pSceneLight->desc.colorR = (G::world.light_color.r() / 255.0f) * intensity;
        pSceneLight->desc.colorG = (G::world.light_color.g() / 255.0f) * intensity;
        pSceneLight->desc.colorB = (G::world.light_color.b() / 255.0f) * intensity;
    }
    else
    {
        pSceneLight->desc.colorR = WorldModulation::light_cache[light_ptr].r;
        pSceneLight->desc.colorG = WorldModulation::light_cache[light_ptr].g;
        pSceneLight->desc.colorB = WorldModulation::light_cache[light_ptr].b;
    }

    if (oDrawArrayLight)
        oDrawArrayLight(var1, pSceneLight, var3);
}

__int64 __fastcall WorldModulation::hkWallColor(__int64 var1, __int64 var2, c_aggregate_object_array* var3)
{
    __int64 original = oWallColor(var1, var2, var3);

    if (G::world.enabled && G::world.wall_enabled && var3 && I::scene_system && I::scene_system->light_data_queue)
    {
        float brightness = G::world.wall_brightness;
        c_color adjusted = c_color(
            (int)(G::world.wall_color.r() * brightness),
            (int)(G::world.wall_color.g() * brightness),
            (int)(G::world.wall_color.b() * brightness),
            G::world.wall_color.a()
        );

        for (int i = 0; i < var3->data->count; i++)
        {
            int index = (var3->data->index + i) << 5;
            void* data_ptr = static_cast<char*>(I::scene_system->light_data_queue->light_data) + index;
            *reinterpret_cast<c_color*>(data_ptr) = adjusted;
        }
    }

    return original;
}

void __fastcall WorldModulation::hk_tonemap_render_debug(void* rcx, void* rdx, float scalar)
{
    if (G::world.enabled && G::world.m_exposure > 0)
    {
        scalar = (float)G::world.m_exposure;
    }
    
    return oToneMap(rcx, rdx, scalar);
}