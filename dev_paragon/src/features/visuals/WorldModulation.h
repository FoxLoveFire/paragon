#pragma once
#include "../../sdk/Interfaces/c_scene_object.h"
#include "../../sdk/Interfaces/Interfaces.h"
#include "../../sdk/memory/memory.h"
#include "../../sdk/math/color.h"
#include "../../../globals.h"

namespace WorldModulation
{
	
	//#define SKYBOX "45 85 C9 0F 8E ? ? ? ? 4C 8B DC 55"
	//#define WALLCOLOR "48 8B C4 48 89 50 ? 48 89 48 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70"
	//#define DRAWARRAY "48 8B C4 53 57 41 54 48 81 EC ?? ?? ?? ?? 49 63 F9 49" chams
	//#define LigthColor "48 89 54 24 ?? 55 57 41 56 48 83 EC"

	using oDrawArrayLightFn = void(__fastcall*)(__int64 a1, CSceneLightObject* a2, __int64 a3);
	inline oDrawArrayLightFn oDrawArrayLight = nullptr;
	void __fastcall hkDrawArrayLight(__int64 a1, CSceneLightObject* a2, __int64 a3);

	using oWallColorFn = __int64(__fastcall*)(__int64 a1, __int64 a2, c_aggregate_object_array* a3);
	inline oWallColorFn oWallColor = nullptr;
	__int64 __fastcall hkWallColor(__int64 a1, __int64 a2, c_aggregate_object_array* a3);

	using oToneMapFn = void(__fastcall*)(void* a1, void* a2, void* a3);
	inline oToneMapFn oToneMap = nullptr;
	void __fastcall hk_tonemap_render_debug(void* a1, void* a2, void* a3);
}