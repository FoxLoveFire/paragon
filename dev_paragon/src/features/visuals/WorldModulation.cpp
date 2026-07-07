#include "WorldModulation.h"
#include <unordered_map>

struct LightColor_t {
    float r, g, b;
};

struct SkyboxData_t {
    float r, g, b;
};

static std::unordered_map<void*, LightColor_t> saved_lights;
static std::unordered_map<void*, SkyboxData_t> saved_skyboxes;

void WorldModulation::hkDrawArrayLight(__int64 var1, CSceneLightObject* pSceneLight, __int64 var3)
{
    if (pSceneLight)
    {
        if (G::world.custom_light)
        {
            if (saved_lights.find(pSceneLight) == saved_lights.end())
            {
                saved_lights[pSceneLight] = { pSceneLight->desc.colorR, pSceneLight->desc.colorG, pSceneLight->desc.colorB };
            }

            // Получаем прозрачность от 0.0f до 1.0f
            float alpha = G::world.light_color.a() / 255.0f;
            
            // Множитель для HDR-эффекта (сделай больше, если нужно еще ярче)
            float intensity_boost = 3.5f; 

            // Смешиваем оригинальный свет движка с кастомным, учитывая альфу и повышенную яркость
            pSceneLight->desc.colorR = (saved_lights[pSceneLight].r * (1.0f - alpha)) + ((G::world.light_color.r() / 255.0f) * alpha * intensity_boost);
            pSceneLight->desc.colorG = (saved_lights[pSceneLight].g * (1.0f - alpha)) + ((G::world.light_color.g() / 255.0f) * alpha * intensity_boost);
            pSceneLight->desc.colorB = (saved_lights[pSceneLight].b * (1.0f - alpha)) + ((G::world.light_color.b() / 255.0f) * alpha * intensity_boost);
        }
        else
        {
            auto it = saved_lights.find(pSceneLight);
            if (it != saved_lights.end())
            {
                pSceneLight->desc.colorR = it->second.r;
                pSceneLight->desc.colorG = it->second.g;
                pSceneLight->desc.colorB = it->second.b;
                saved_lights.erase(it);
            }
        }
    }
    if (oDrawArrayLight)
        oDrawArrayLight(var1, pSceneLight, var3);
}

__int64 __fastcall WorldModulation::hkWallColor(__int64 var1, __int64 var2, c_aggregate_object_array* var3)
{
    if (!G::world.custom_wall_color)
        return oWallColor(var1, var2, var3);

    __int64 original = oWallColor(var1, var2, var3);

    float alpha = G::world.wall_color.a() / 255.0f;

    for (int i = 0; i < var3->data->count; i++)
    {
        int index = (var3->data->index + i) << 5;
        auto* wall_color_ptr = reinterpret_cast<c_color*>(static_cast<char*>(I::scene_system->light_data_queue->light_data) + index);
        
        // Читаем изначальный цвет стены, подготовленный движком
        c_color original_color = *wall_color_ptr;
        
        // Интерполируем RGB каналы на основе ползунка прозрачности
        *wall_color_ptr = c_color(
            static_cast<int>(original_color.r() * (1.0f - alpha) + G::world.wall_color.r() * alpha),
            static_cast<int>(original_color.g() * (1.0f - alpha) + G::world.wall_color.g() * alpha),
            static_cast<int>(original_color.b() * (1.0f - alpha) + G::world.wall_color.b() * alpha),
            255
        );
    }

    return original;
}

void __fastcall WorldModulation::hk_tonemap_render_debug(void* a1, void* a2, void* a3)
{
    float brightness = G::world.m_exposure == 0 ? 1.0f : (G::world.m_exposure) / 100.f;
    Memory::call_vfunc<void>(a1, 5, brightness);

    oToneMap(a1, a2, a3);
}

void __fastcall WorldModulation::hkSkyboxArray(__int64 ecx, __int64 render, __int64 primitive, int nCount, int RenderFlag, __int64 view_info, __int64 render_stats)
{
    if (nCount > 0 && nCount < 100 && primitive)
    {
        uintptr_t skybox_ptr_address = primitive + (static_cast<size_t>(nCount) * 0x68) - 0x50;
        if (auto* const skybox_obj = *reinterpret_cast<void**>(skybox_ptr_address))
        {
            if (auto* const color = reinterpret_cast<float*>(static_cast<char*>(skybox_obj) + 0xE8))
            {
                if (G::world.custom_skybox)
                {
                    if (saved_skyboxes.find(skybox_obj) == saved_skyboxes.end())
                    {
                        saved_skyboxes[skybox_obj] = { color[0], color[1], color[2] };
                    }

                    float alpha = G::world.skybox_color.a() / 255.0f;

                    // Плавное наложение цвета на скайбокс через Alpha-канал
                    color[0] = (saved_skyboxes[skybox_obj].r * (1.0f - alpha)) + ((G::world.skybox_color.r() / 255.0f) * alpha);
                    color[1] = (saved_skyboxes[skybox_obj].g * (1.0f - alpha)) + ((G::world.skybox_color.g() / 255.0f) * alpha);
                    color[2] = (saved_skyboxes[skybox_obj].b * (1.0f - alpha)) + ((G::world.skybox_color.b() / 255.0f) * alpha);
                }
                else
                {
                    auto it = saved_skyboxes.find(skybox_obj);
                    if (it != saved_skyboxes.end())
                    {
                        color[0] = it->second.r;
                        color[1] = it->second.g;
                        color[2] = it->second.b;
                        saved_skyboxes.erase(it);
                    }
                }
            }
        }
    }
    oSkyboxArray(ecx, render, primitive, nCount, RenderFlag, view_info, render_stats);
}