#pragma once
#include <cstdint>
#include "../../../memory/memory.h"

class CMaterial2
{
public:
	virtual const char* GetName() = 0;
	virtual const char* GetShareName() = 0;
};

struct MaterialKeyVar_t
{
	std::uint64_t uKey;
	const char* szName;

	MaterialKeyVar_t(std::uint64_t uKey, const char* szName) :
		uKey(uKey), szName(szName) {}

	MaterialKeyVar_t(const char* szName, bool bShouldFindKey = false) :
		szName(szName)
	{
		uKey = bShouldFindKey ? FindKey(szName) : 0x0;
	}

	std::uint64_t FindKey(const char* szName)
	{
		using fn = std::uint64_t(__fastcall*)(const char*, unsigned int, int);
		static auto find = reinterpret_cast<fn>(Memory::pattern_scan("48 89 5C 24 ? 57 48 81 EC ? ? ? ? 33 C0 8B DA", "particles.dll"));
		return find(szName, 0x12, 0x31415926);
	}
};