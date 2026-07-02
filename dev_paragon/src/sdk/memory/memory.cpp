#include "memory.h"
#include <windows.h>
#include <winternl.h>
#include <string>
#include <locale>

uintptr_t Memory::getAbsoluteAddress(uintptr_t addr, const int nPreOffset, const int nPostOffset)
{
	addr += nPreOffset;
	int32_t nRva = *reinterpret_cast<int32_t*>(addr);
	addr += nPostOffset + sizeof(uint32_t) + nRva;
	return addr;
}

std::wstring s2ws(const std::string& str)
{
	if (str.empty()) return L"";

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);

	std::wstring wstrTo(size_needed, 0);

	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);

	return wstrTo;
}

uintptr_t Memory::pattern_scan(std::string pattern, std::string module)
{
	if (module.empty() || pattern.empty())
		return 0;

	auto get_byte = [](const char* p) {
		if (*p == '?') return -1;
		return (int)strtol(p, nullptr, 16);
		};

	HMODULE handle = GetModuleHandleA(module.c_str());
	if (!handle) return 0;

	PIMAGE_DOS_HEADER dos_header = (PIMAGE_DOS_HEADER)handle;
	PIMAGE_NT_HEADERS nt_headers = (PIMAGE_NT_HEADERS)((uintptr_t)handle + dos_header->e_lfanew);

	uintptr_t size = nt_headers->OptionalHeader.SizeOfImage;
	uintptr_t start = (uintptr_t)handle;

	std::vector<int> pattern_bytes;
	const char* ptr = pattern.c_str();

	while (*ptr) {
		if (*ptr == ' ') { ptr++; continue; }

		pattern_bytes.push_back(get_byte(ptr));
		if (*ptr == '?') {
			ptr++;
			if (*ptr == '?') ptr++;
		}
		else {
			ptr += 2;
		}
	}

	if (pattern_bytes.empty())
		return 0;

	for (uintptr_t i = 0; i <= size - pattern_bytes.size(); i++) {
		bool found = true;
		for (size_t j = 0; j < pattern_bytes.size(); j++) {
			if (pattern_bytes[j] != -1 && *(uint8_t*)(start + i + j) != (uint8_t)pattern_bytes[j]) {
				found = false;
				break;
			}
		}
		if (found) return start + i;
	}

	return 0;
}

[[nodiscard]] bool Memory::isValid(uintptr_t address)
{
	if (!address || address < 0x10000)
		return false;
	MEMORY_BASIC_INFORMATION mbi = { 0 };
	if (VirtualQuery((void*)address, &mbi, sizeof(mbi))) {
		DWORD protect = mbi.Protect;
		bool isReadable = (protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE));
		bool isNotSafe = (protect & (PAGE_GUARD | PAGE_NOACCESS));

		if (mbi.State == MEM_COMMIT && isReadable && !isNotSafe) {
			return true;
		}
	}
	return false;
}

uintptr_t Memory::ResolveRelativeAddress(uintptr_t instruction_address, int offset_position, int instruction_length)
{
	uint32_t ripOffset = *reinterpret_cast<uint32_t*>(instruction_address + offset_position);
	return instruction_address + instruction_length + ripOffset;
}

std::uint8_t* Memory::ResolveRelativeAddressSchema(std::uint8_t* nAddressBytes, std::uint32_t nRVA0Offset, std::uint32_t nRipOffset)
{
	std::uint32_t nRVA = *reinterpret_cast<std::uint32_t*>(nAddressBytes + nRVA0Offset);
	std::uint64_t nRIP = reinterpret_cast<std::uint64_t>(nAddressBytes) + nRipOffset;
	return reinterpret_cast<std::uint8_t*>(nRVA + nRIP);
}
