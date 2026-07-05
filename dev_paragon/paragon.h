#pragma once
#include "src/sdk/memory/memory.h"
#include "src/sdk/sdk.h"
#include "src/sdk/hooks.h"
#include "src/sdk/utils/schema/schema.h"

namespace paragon
{
	inline void main(HMODULE module)
	{
		AllocConsole();
		SetConsoleCP(CP_UTF8);
		SetConsoleOutputCP(CP_UTF8);
		SetConsoleTitle(L"One love");
		FILE* f;
		freopen_s(&f, "CONOUT$", "w", stdout);

		if (!sdk::initialize())
		{
			std::cout << "sdk is not loaded!" << std::endl;
		}

		// Установка хуков
		if (!Hook::initialize())
		{
			std::cout << "hooks is not loaded!" << std::endl;
		}

		Schema schema;
		if (!schema.init("client.dll", 0))
		{
			std::cout << "Schema is not loaded by init()" << std::endl;
		}
	}
}