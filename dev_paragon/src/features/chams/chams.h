#pragma once
#include <stdint.h>
namespace chams
{
	using OnGeneratePrimitivesFn = __int64(__fastcall*)(__int64 desc, __int64 object, __int64 a3, __int64 render_buf);
	inline OnGeneratePrimitivesFn oOnGeneratePrimitives = nullptr;

	__int64 hkOnGeneratePrimitives(__int64 desc, __int64 object, __int64 a3, __int64 render_buf);
}