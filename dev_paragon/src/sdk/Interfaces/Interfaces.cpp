#include "Interfaces.h"

bool I::initialize()
{
	std::string engine_str = "Source2EngineToClient001";
	engine_client = find_interface<c_engine_client*>(GetModuleHandleA("engine2.dll"), engine_str.c_str());
	if (!engine_client)
	{
		std::cout << engine_str << " is not loaded!" << std::endl;
		return false;
	}

	std::string scene_str = "SceneSystem_002";
	scene_system = find_interface<c_scene_system*>(GetModuleHandleA("scenesystem.dll"), scene_str.c_str());
	if (!scene_system)
	{
		std::cout << scene_str << " is not loaded!" << std::endl;
		return false;
	}

	std::string input_str = "InputSystemVersion001";
	input_system = find_interface<void*>(GetModuleHandleA("inputsystem.dll"), input_str.c_str());
	if (!input_system)
	{
		std::cout << input_str << " is not loaded!" << std::endl;
		return false;
	}

	pvs = reinterpret_cast<c_engine_pvs*>(Memory::ResolveRelativeAddress(Memory::pattern_scan("48 8D 0D ? ? ? ? 33 D2 FF 50", "engine2.dll"), 0x3, 0x7));
	if (!pvs)
	{
		std::cout << "pvs is not initialized!" << std::endl;
		return false;
	}

	std::string schema_str = "SchemaSystem_001";
	schema_system = find_interface<ISchemaSystem*>(GetModuleHandleA("schemasystem.dll"), schema_str.c_str());
	if (!schema_system)
	{
		std::cout << "schema is not initialized!" << std::endl;
		return false;
	}

	std::string entity_str = "GameResourceServiceClientV001";
	GameEntity = find_interface<IGameResourceService*>(GetModuleHandleA("engine2.dll"), entity_str.c_str());
	if (!GameEntity)
	{
		std::cout << "GameEntity is not initialized!" << std::endl;
		return false;
	}

	std::uintptr_t instruction_address = Memory::pattern_scan("48 8B 0D ? ? ? ? 8B 10 E8 ? ? ? ? 45 32 FF", "client.dll");

	if (instruction_address) {
		std::int32_t rip_offset = *reinterpret_cast<std::int32_t*>(instruction_address + 3);
		std::uintptr_t client_input_ptr = instruction_address + 7 + rip_offset;

		m_csgo_input = *reinterpret_cast<CCSGOInput**>(client_input_ptr);

		std::cout << "CCSGOInput " << m_csgo_input << std::endl;
	}


	return true;
}
