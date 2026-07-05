#pragma once
#include <Windows.h>
#include "../../../globals.h"

namespace Removals
{
	using DrawSmokeFn = __int64(__fastcall*)(__int64 a1, __int64 a2, __int64 a3, __int64 a4, __int64 a5, unsigned int* a6);
	inline DrawSmokeFn oDrawSmoke = nullptr;
	__int64 __fastcall hkDrawSmoke(__int64 a1, __int64 a2, __int64 a3, __int64 a4, __int64 a5, unsigned int* a6);

	using DrawScopeOverlayFn = __int64(__fastcall*)(__int64 a1, __int64 a2);
	inline DrawScopeOverlayFn oDrawScopeOverlay = nullptr;
	__int64 __fastcall hkDrawScopeOverlay(__int64 a1, __int64 a2);

	using DrawLegsFn = void(__fastcall*)(__int64* a1, __int64* a2, __int64 a3, __int64 a4, __int64 a5);
	inline DrawLegsFn oDrawLegs = nullptr;
	void __fastcall hkDrawLegs(__int64* a1, __int64* a2, __int64 a3, __int64 a4, __int64 a5);

	using FlashbangOverlayFn = void(__fastcall*)(__int64 a1, int a2, __int64* a3, __int64 a4, __m128* a5);
	inline FlashbangOverlayFn oFlashbangOverlay = nullptr;
	void __fastcall hkFlashbangOverlay(__int64 a1, int a2, __int64* a3, __int64 a4, __m128* a5);
}