#pragma once
#include <stdint.h>
#include <string>
#include <windows.h>
#include <psapi.h>
#include <vector>
#include <algorithm>


namespace Memory
{
	template <typename T>
	T Read(uintptr_t address, T defaultValue = T())
	{
		if (address < 0x10000 || address > 0x7FFFFFFEFFFF)
			return defaultValue;
		__try {
			return *reinterpret_cast<T*>(address);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			return defaultValue;
		}
	}

	template <typename T>
	bool Write(uintptr_t address, T value)
	{
		if (address < 0x10000)
			return false;

		T* ptr = reinterpret_cast<T*>(address);
		DWORD oldProtect;

		if (VirtualProtect(reinterpret_cast<void*>(address), sizeof(T), PAGE_EXECUTE_READWRITE, &oldProtect))
		{
			__try {
				*ptr = value;

				VirtualProtect(reinterpret_cast<void*>(address), sizeof(T), oldProtect, &oldProtect);
				return true;
			}
			__except (EXCEPTION_EXECUTE_HANDLER) {
				return false;
			}
		}

		return false;
	}

	uintptr_t pattern_scan(std::string pattern, std::string module);

	[[nodiscard]] bool isValid(uintptr_t address);

	template <typename T, typename... Args>
	auto call_vfunc(void* instance, int index, Args... args) {
		auto vtable = *static_cast<void***>(instance);
		auto fn = static_cast<T(__thiscall*)(void*, Args...)>(vtable[index]);
		return fn(instance, args...);
	}

	template <typename T = void*>
	inline T get_vfunc(void* class_, unsigned int index)
	{
		if (!class_)
			return T{};

		void** table = *reinterpret_cast<void***>(class_);
		if (!table)
			return T{};

		return reinterpret_cast<T>(table[index]);
	}


	uintptr_t ResolveRelativeAddress(uintptr_t instruction_address, int offset_position, int instruction_length);

	std::uint8_t* ResolveRelativeAddressSchema(std::uint8_t* nAddressBytes, std::uint32_t nRVA0Offset, std::uint32_t nRipOffset);


	uintptr_t getAbsoluteAddress(uintptr_t addr, const int nPreOffset, const int nPostOffset = 0);
}