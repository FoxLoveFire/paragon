#pragma once
#include "src/sdk/memory/memory.h"
#include "src/sdk/sdk.h"
#include "src/sdk/hooks.h"

namespace paragon
{
	inline void main(HMODULE module)
	{
		AllocConsole();
		SetConsoleTitle(L"One love");
		FILE* f;
		freopen_s(&f, "CONOUT$", "w", stdout);

		if (!sdk::initialize())
		{
			std::cout << "sdk is not loaded!" << std::endl;
		}

		if (!Hook::initialize())
		{
			std::cout << "hooks is not loaded!" << std::endl;
		}
	}
}