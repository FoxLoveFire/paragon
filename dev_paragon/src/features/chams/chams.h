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
#include "../../sdk/hooks.h"

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
	char pad0[0x18];
	CSceneAnimatableObject* m_pSceneAnimatableObject;
	CMaterial2* m_pMaterial;
	CMaterial2* m_pMaterial2;
	char pad1[0x20];
	c_color m_pColor;
	char pad3[0x10];
};

class c_mesh_primitive_output_buffer {
public:
	c_mesh_primitive* m_mesh_primitive_array;
	char pad0[4];
	int m_arr_size;

	c_mesh_primitive* get_primitive(int index) {
		return &m_mesh_primitive_array[index];
	}
};

namespace chams
{
	using OnGeneratePrimitivesFn = __int64(__fastcall*)(void* desc, CSceneAnimatableObject* object, __int64 a3, c_mesh_primitive_output_buffer* render_buf);
	inline OnGeneratePrimitivesFn oOnGeneratePrimitives = nullptr;
	__int64 hkOnGeneratePrimitives(void* desc, CSceneAnimatableObject* object, __int64 a3, c_mesh_primitive_output_buffer* render_buf);

	using DrawArrayFn = void*(__fastcall*)(uint64_t a1, uint64_t a2, c_mesh_primitive* a3, int32_t a4, uint64_t a5, uint64_t a6, uint64_t a7);
	inline DrawArrayFn oDrawArray = nullptr;
	void* hkDrawArray(uint64_t a1, uint64_t a2, c_mesh_primitive* a3, int32_t a4, uint64_t a5, uint64_t a6, uint64_t a7);
}