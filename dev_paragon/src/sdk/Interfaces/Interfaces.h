#pragma once
#include <Windows.h>
#include "engine.h"
#include "c_scene_object.h"
#include "../cs2/datatypes/schema/ISchemaClass/ISchemaClass.h"
#include "CGameEntitySystem/CGameEntitySystem.h"
#include "../memory/memory.h"
#include "../../sdk/cs2/datatypes/usercmd/usercmd.h"

class IGameResourceService;
namespace I
{
	template <typename T = void*>
	T find_interface(HMODULE module_handle, const char* name)
	{
		using fn = T(*)(const char*, int*);
		const auto func = reinterpret_cast<fn>((GetProcAddress)(module_handle, ("CreateInterface")));
		if (!func)
			return nullptr;

		return func(name, nullptr);
	}

	inline c_engine_client* engine_client = nullptr;
	inline c_scene_system* scene_system = nullptr;
	inline c_engine_pvs* pvs = nullptr;
	inline ISchemaSystem* schema_system = nullptr;
	inline IGameResourceService* GameEntity = nullptr;
	inline CCSGOInput* m_csgo_input = nullptr;
	inline void* input_system = nullptr;
	inline void* IRS = nullptr;
	
	bool initialize();
}