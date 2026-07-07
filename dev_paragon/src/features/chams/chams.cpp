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

    sdk::CreateMaterial(nullptr, &pCustomMaterial, name, pKeyValues3, 0, 1);
    return pCustomMaterial;
}

struct resource_material_t
{
    CStrongHandle<CMaterial2> mat;
    CStrongHandle<CMaterial2> mat_invs;
};

static resource_material_t resourceMaterials[ChamsType::MAXCOUNT];

void Material::material_init()
{
    resourceMaterials[ChamsType::BLOOM] = resource_material_t
    {
        .mat = create("materials/dev/primary_white.vmat",
                        R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
			format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
			{
                shader = "solidcolor.vfx"
                g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
                g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
                g_tRoughness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
                g_tMetalness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
                g_tAmbientOcclusion = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
                F_IGNOREZ = 0
                F_DISABLE_Z_WRITE = 0
                F_DISABLE_Z_BUFFERING = 0
                F_RENDER_BACKFACES = 1
                F_BLEND_MODE = 0 
                F_TRANSLUCENT = 0
                g_vColorTint = [9.0, 9.0, 9.0, 9.0]
			})"),
        .mat_invs = create("materials/dev/primary_white.vmat",
            R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
			format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
			{
				shader = "solidcolor.vfx"
				g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
				g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
				g_tRoughness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
				g_tMetalness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
				g_tAmbientOcclusion = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
				F_IGNOREZ = 1
				F_DISABLE_Z_WRITE = 1
				F_DISABLE_Z_BUFFERING = 1
                F_BLEND_MODE = 1
                F_TRANSLUCENT = 1
				F_RENDER_BACKFACES = 1
				g_vColorTint = [9.0, 9.0, 9.0, 9.0]
			})")
    };

    
}

__int64 chams::hkOnGeneratePrimitives(__int64 desc, __int64 object, __int64 a3, __int64 render_buf)
{
    return oOnGeneratePrimitives(desc, object, a3, render_buf);
}