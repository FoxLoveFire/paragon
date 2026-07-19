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
	CBaseHandle m_owner{};
	char pad2[0x1C];

	CBaseHandle Owner() const {
		return m_owner;
	}
};

class c_mesh_primitive {
public:
	void* m_pData;
	char pad0[0x10];
	CSceneAnimatableObject* m_pSceneAnimatableObject;
	CMaterial2 *m_pMaterial;
	CMaterial2* m_pMaterial2;
	char pad1[0x10];
	void* some_data;
	char pad2[0x8];
	c_color m_pColor;
	char pad3[0x1C];
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

namespace chams
{
	using OnGeneratePrimitivesFn = __int64(__fastcall*)(void* desc, CSceneAnimatableObject* object, __int64 a3, c_mesh_primitive_output_buffer* render_buf);
	inline OnGeneratePrimitivesFn oOnGeneratePrimitives = nullptr;
	__int64 hkOnGeneratePrimitives(void* desc, CSceneAnimatableObject* object, __int64 a3, c_mesh_primitive_output_buffer* render_buf);

}