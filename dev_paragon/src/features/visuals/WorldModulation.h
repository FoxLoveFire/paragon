#pragma once
#include "../../sdk/Interfaces/c_scene_object.h"
#include "../../sdk/Interfaces/Interfaces.h"
#include "../../sdk/math/color.h"
#include "../../../globals.h"

namespace WorldModulation
{
	
	//#define SKYBOX "45 85 C9 0F 8E ? ? ? ? 4C 8B DC 55"
	//#define WALLCOLOR "48 8B C4 48 89 50 ? 48 89 48 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70"
	//#define DRAWARRAY "48 8B C4 53 57 41 54 48 81 EC ?? ?? ?? ?? 49 63 F9 49" chams
	//#define LigthColor "48 89 54 24 ?? 55 57 41 56 48 83 EC"
	typedef void(__fastcall* DrawArrayLightFn)(__int64 a1, CSceneLightObject* a2, __int64 a3);
	inline  DrawArrayLightFn oDrawArrayLight = nullptr;
	void __fastcall hkDrawArrayLight(__int64 a1, CSceneLightObject* a2, __int64 a3);

	typedef __int64(__fastcall* WallColor)(__int64 a1, __int64 a2, c_aggregate_object_array* a3);
	inline WallColor oWallColor = nullptr;
	__int64 __fastcall hkWallColor(__int64 a1, __int64 a2, c_aggregate_object_array* a3);
}