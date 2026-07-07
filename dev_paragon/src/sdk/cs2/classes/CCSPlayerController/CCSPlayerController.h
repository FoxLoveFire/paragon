#pragma once
#include "../../../utils/schema/schema.h"
#include "../../../utils/CHandle/handle.h"

class CCSPlayerController {
public:
	CCSPlayerController(uintptr_t address);
	const char* getName() const;
	uintptr_t getAddress() const;


private:
	uintptr_t address;
};
