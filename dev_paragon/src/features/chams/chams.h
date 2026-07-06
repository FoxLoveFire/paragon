#pragma once
#include <stdint.h>
#include "../../sdk/utils/CStrongHandle/CStrongHandle.h"
#include "../../sdk/cs2/datatypes/C_Material/C_Material.h"
#include "../../sdk/utils/keyvalues/keyvalues.h"
#include "../../sdk/Interfaces/Interfaces.h"

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

namespace chams
{
	using OnGeneratePrimitivesFn = __int64(__fastcall*)(__int64 desc, __int64 object, __int64 a3, __int64 render_buf);
	inline OnGeneratePrimitivesFn oOnGeneratePrimitives = nullptr;

	__int64 hkOnGeneratePrimitives(__int64 desc, __int64 object, __int64 a3, __int64 render_buf);

}