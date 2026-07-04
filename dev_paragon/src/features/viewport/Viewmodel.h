#pragma once
#include "../../../globals.h"

namespace Viewmodel
{
	using ViewModelFn = void(__fastcall*)(__int64 a1, float* a2, float* a3);
	inline ViewModelFn oViewModel = nullptr;
	void __fastcall hkViewModel(__int64 a1, float* a2, float* a3);
}