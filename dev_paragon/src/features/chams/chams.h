#pragma once
#include <stdint.h>
#include "../../sdk/utils/CStrongHandle/CStrongHandle.h"
#include "../../sdk/cs2/datatypes/C_Material/C_Material.h"
#include "../../sdk/utils/keyvalues/keyvalues.h"
#include "../../sdk/Interfaces/Interfaces.h"
#include "../../sdk/utils/CHandle/handle.h"
#include "../../sdk/math/color.h"
#include "../../sdk/memory/memory.h"
#include "../../../globals.h"
enum ChamsType
{
	FLAT,
	ILLUMINATE,
	GLOW,
	BLOOM,
	MAXCOUNT
};

namespace Material
{
	CStrongHandle<CMaterial2> create(const char* name, const char szVmatBuffer[]);

	void material_init();
}

struct unk_data_t {
	char pad1[0x98];
	const char* m_panel_name;
};

class CSceneAnimatableObject {
public:
	char pad1[0xB8];
	unk_data_t* m_unk_data; // 0xB8
	CBaseHandle m_owner{}; // 0xC0
	char pad2[0x4C]; // align + reserved -> 0x110

	CBaseHandle Owner() const {
		return m_owner;
	}
};

class c_mesh_primitive {
public:
	char pad0[24];
	CSceneAnimatableObject* m_scene_animatable_object;
	CMaterial2* m_material;
	CMaterial2* m_material2;
	char pad1[32];
	c_color m_color;
	char pad2[16];
};

class c_mesh_primitive_output_buffer {
public:
	c_mesh_primitive* m_mesh_primitive_array;
	int m_arr_size;
	char pad0[4];

	c_mesh_primitive* get_primitive(int index) {
		return &m_mesh_primitive_array[index];
	}
};

class c_animatable_scene_object_desc {
public:
	CSceneAnimatableObject* create() {
		return Memory::call_vfunc<CSceneAnimatableObject*>(this, 21);
	}
};

namespace chams
{
	using OnGeneratePrimitivesFn = __int64(__fastcall*)(c_animatable_scene_object_desc* desc, CSceneAnimatableObject* object, __int64 a3, c_mesh_primitive_output_buffer* render_buf);
	inline OnGeneratePrimitivesFn oOnGeneratePrimitives = nullptr;
	__int64 hkOnGeneratePrimitives(c_animatable_scene_object_desc* desc, CSceneAnimatableObject* object, __int64 a3, c_mesh_primitive_output_buffer* render_buf);

}