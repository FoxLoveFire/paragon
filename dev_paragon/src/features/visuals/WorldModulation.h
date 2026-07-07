#pragma once
#include "../../sdk/Interfaces/c_scene_object.h"
#include "../../sdk/Interfaces/Interfaces.h"
#include "../../sdk/memory/memory.h"
#include "../../sdk/math/color.h"
#include "../../../globals.h"

namespace WorldModulation
{
	using oDrawArrayLightFn = void(__fastcall*)(__int64 a1, CSceneLightObject* a2, __int64 a3);
	inline oDrawArrayLightFn oDrawArrayLight = nullptr;
	void __fastcall hkDrawArrayLight(__int64 a1, CSceneLightObject* a2, __int64 a3);

	using oWallColorFn = __int64(__fastcall*)(__int64 a1, __int64 a2, c_aggregate_object_array* a3);
	inline oWallColorFn oWallColor = nullptr;
	__int64 __fastcall hkWallColor(__int64 a1, __int64 a2, c_aggregate_object_array* a3);

	using oToneMapFn = void(__fastcall*)(void* a1, void* a2, void* a3);
	inline oToneMapFn oToneMap = nullptr;
	void __fastcall hk_tonemap_render_debug(void* a1, void* a2, void* a3);

	using SkyboxArrayFn = void(__fastcall*)(__int64 ecx, __int64 render, __int64 primitive, int nCount, int RenderFlag, __int64 view_info, __int64 render_stats);
	inline SkyboxArrayFn oSkyboxArray = nullptr;
	void __fastcall hkSkyboxArray(__int64 ecx, __int64 render, __int64 primitive, int nCount, int RenderFlag, __int64 view_info, __int64 render_stats);
}