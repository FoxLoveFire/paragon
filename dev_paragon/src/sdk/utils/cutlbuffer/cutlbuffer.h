#pragma once
#include <cstdint>
#include "../../sdk.h"

class CUtlBuffer
{
public:
	char _pad[0x80];

	CUtlBuffer(int a1 = 0, int size = 0, int a3 = 0);
	void ensure(int size);

	void PutString(const char* szString);
};
