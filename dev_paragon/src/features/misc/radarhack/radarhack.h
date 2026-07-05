#pragma once
#include "../../../../globals.h"

namespace radarhack
{
	using RadarFn = void(__fastcall*)(void* a1);
	inline RadarFn oRadar = nullptr;
	void __fastcall hkRadar(void* a1);
}