#include "chams.h"

CStrongHandle<CMaterial2> Material::create(const char* name, const char szVmatBuffer[])
{
    CKeyValues3* pKeyValues3 = nullptr;

    pKeyValues3 = pKeyValues3->create_material_from_resource();

    if (!pKeyValues3) {
        printf("[!] Failed to create material from resource: %s\n", name);
        return CStrongHandle<CMaterial2>();
    }

    pKeyValues3->LoadFromBuffer(szVmatBuffer);

    CStrongHandle<CMaterial2> pCustomMaterial = {};

    sdk::CreateMaterial(nullptr, (void**)&pCustomMaterial, name, pKeyValues3, 0, 1);
    return pCustomMaterial;
}

struct resource_material_t
{
    CStrongHandle<CMaterial2> mat_invs;
    CStrongHandle<CMaterial2> mat;
};

static resource_material_t resourceMaterials[ChamsType::MAXCOUNT];

void Material::material_init()
{
    resourceMaterials[ChamsType::BLOOM] = resource_material_t
    {
        .mat_invs = create("materials/dev/metallic2_i.vmat",
           R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
        {
            shader = "csgo_complex.vfx"
            F_DISABLE_Z_BUFFERING = 1
            F_DISABLE_Z_PREPASS = 1
            F_DISABLE_Z_WRITE = 1
            F_BLEND_MODE = 1
            F_RENDER_BACKFACES = 0
            g_vColorTint = [1.0, 1.0, 1.0, 1.0]
            g_bFogEnabled = 0
            g_flMetalness = 1.000
            g_flModelTintAmount = 1.000
            g_nScaleTexCoordUByModelScaleAxis = 0
            g_nScaleTexCoordVByModelScaleAxis = 0
            g_nTextureAddressModeU = 0
            g_nTextureAddressModeV = 0
            g_flTexCoordRotation = 0.000
            g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
            g_tAmbientOcclusion = resource:"materials/default/default_ao_tga_559f1ac6.vtex"
            g_tNormal = resource:"materials/default/default_normal_tga_1b833b2a.vtex"
        })"),

        .mat = create("materials/dev/metallic2.vmat",
            R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
        {
            shader = "csgo_complex.vfx"
            F_DISABLE_Z_PREPASS = 1
            F_DISABLE_Z_WRITE = 1
            F_BLEND_MODE = 1
            F_RENDER_BACKFACES = 0
            g_vColorTint = [1.0, 1.0, 1.0, 1.0]
            g_bFogEnabled = 0
            g_flMetalness = 1.000
            g_flModelTintAmount = 1.000
            g_nScaleTexCoordUByModelScaleAxis = 0
            g_nScaleTexCoordVByModelScaleAxis = 0
            g_nTextureAddressModeU = 0
            g_nTextureAddressModeV = 0
            g_flTexCoordRotation = 0.000
            g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
            g_tAmbientOcclusion = resource:"materials/default/default_ao_tga_559f1ac6.vtex"
            g_tNormal = resource:"materials/default/default_normal_tga_1b833b2a.vtex"
        })"),
    };
}

__int64 chams::hkOnGeneratePrimitives(void* desc, CSceneAnimatableObject* object, __int64 a3, c_mesh_primitive_output_buffer* render_buf)
{
    if (!I::engine_client->valid() || !render_buf || render_buf->m_arr_size <= 0) {
        return oOnGeneratePrimitives(desc, object, a3, render_buf);
    }

    CBaseHandle render_ent = object->Owner();
    if (!render_ent.valid()) {
        return oOnGeneratePrimitives(desc, object, a3, render_buf);
    }

    auto entity = I::GameEntity->Instance->Get(render_ent);
    if (!entity) {
        return oOnGeneratePrimitives(desc, object, a3, render_buf);
    }

    C_CSPlayerPawn* local = Hook::oGetLocalPlayer(0);

    bool shouldProcess = G::g_chams_config.is_enabled &&
        entity != local &&
        entity->m_iHealth() > 0;

    // MODIFY BEFORE CALLING ORIGINAL
    if (shouldProcess)
    {
        for (int i = 0; i < render_buf->m_arr_size; i++)
        {
            c_mesh_primitive* mesh_primitives = render_buf->get_primitive(i);
            if (!mesh_primitives) continue;

            auto& matResource = resourceMaterials[ChamsType::BLOOM];
            if (matResource.mat_invs) {
                mesh_primitives->m_pMaterial = matResource.mat_invs;
                mesh_primitives->m_pColor = { 255, 0, 0 };
            }
        }
    }

    return oOnGeneratePrimitives(desc, object, a3, render_buf);
}