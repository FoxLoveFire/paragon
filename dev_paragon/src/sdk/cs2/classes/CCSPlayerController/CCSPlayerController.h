#pragma once
#include "../../../utils/schema/schema.h"
#include "../../../utils/CHandle/handle.h"

class CCSPlayerController {
public:
	CCSPlayerController(uintptr_t address);
	schema(int, m_iPing, "CCSPlayerController->m_iPing");

	const char* getName() const;
	uintptr_t getAddress() const;

	SCHEMA_ADD_OFFSET(bool, IsLocalPlayer, 0x780);
	SCHEMA_ADD_OFFSET(CBaseHandle, m_hPawn, 0x6BC);

private:
	uintptr_t address;
};
