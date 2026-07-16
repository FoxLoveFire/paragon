#include "sdk.h"

bool sdk::initialize()
{
	if (!I::initialize())
		return false;

	const HMODULE tier0_base = GetModuleHandleA("tier0.dll");
	ConstructUtlBuffer = reinterpret_cast<decltype(ConstructUtlBuffer)>(GetProcAddress(tier0_base, "??0CUtlBuffer@@QEAA@HHW4BufferFlags_t@0@@Z"));
	EnsureCapacityBuffer = reinterpret_cast<decltype(EnsureCapacityBuffer)>(GetProcAddress(tier0_base, "?EnsureCapacity@CUtlBuffer@@QEAAXH@Z"));
	PutUtlString = reinterpret_cast<decltype(PutUtlString)>(GetProcAddress(tier0_base, "?PutString@CUtlBuffer@@QEAAXPEBD@Z"));
	LoadKeyValues = reinterpret_cast<decltype(LoadKeyValues)>(GetProcAddress(tier0_base, "?LoadKV3@@YA_NPEAVKeyValues3@@PEAVCUtlString@@PEAVCUtlBuffer@@AEBUKV3ID_t@@PEBDI@Z"));

	CreateMaterial = reinterpret_cast<decltype(CreateMaterial)>(Memory::pattern_scan("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 56 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 4C 8B F2", "materialsystem2.dll"));
	if (CreateMaterial) {
		std::cout << "CreateMaterial " << CreateMaterial << std::endl;
	}

	return true;
}

mat4x4_t sdk::GetViewMatrix()
{
	mat4x4_t matrix{};

	uintptr_t address = Memory::pattern_scan("48 8D 0D ? ? ? ? 48 C1 E0 06", "client.dll");
	if (!address)
		return matrix;


	matrix = *reinterpret_cast<mat4x4_t*>(Memory::getAbsoluteAddress(
	Memory::pattern_scan("48 8D 0D ? ? ? ? 48 C1 E0 06", "client.dll"), 3, 0));

	return matrix;
}
