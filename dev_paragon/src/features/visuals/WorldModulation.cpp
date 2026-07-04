#include "WorldModulation.h"

void WorldModulation::hkDrawArrayLight(__int64 var1, CSceneLightObject* pSceneLight, __int64 var3)
{
    if (pSceneLight) {
        pSceneLight->desc.colorR = G::world.light_color.r();
        pSceneLight->desc.colorG = G::world.light_color.g();
        pSceneLight->desc.colorB = G::world.light_color.b();
    }

    if (oDrawArrayLight)
        oDrawArrayLight(var1, pSceneLight, var3);
}

__int64 __fastcall WorldModulation::hkWallColor(__int64 var1, __int64 var2, c_aggregate_object_array* var3)
{
    __int64 original = oWallColor(var1, var2, var3);

    for (int i = 0; i < var3->data->count; i++)
    {
        int index = (var3->data->index + i) << 5;
        *reinterpret_cast<c_color*>(static_cast<char*>(I::scene_system->light_data_queue->light_data) + index) = G::world.wall_color;
    }

    return original;
}

void __fastcall WorldModulation::hk_tonemap_render_debug(void* a1, void* a2, void* a3)
{
    float brightness = G::world.m_exposure == 0 ? 1.0f : (G::world.m_exposure) / 100.f;
    Memory::call_vfunc<void>(a1, 5, brightness);

    oToneMap(a1, a2, a3);
}