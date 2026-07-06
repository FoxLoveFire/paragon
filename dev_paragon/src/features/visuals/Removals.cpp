#include "Removals.h"

__int64 __fastcall Removals::hkDrawSmoke(__int64 a1, __int64 a2, __int64 a3, __int64 a4, __int64 a5, unsigned int* a6)
{
	if (G::removals.disable_smoke)
		return 0;

	return oDrawSmoke(a1, a2, a3, a4, a5, a6);
}

__int64 __fastcall Removals::hkDrawScopeOverlay(__int64 a1, __int64 a2)
{
	if (G::removals.disable_scopeoverlay)
		return 0;

	return oDrawScopeOverlay(a1, a2);
}

void __fastcall Removals::hkDrawLegs(__int64* a1, __int64* a2, __int64 a3, __int64 a4, __int64 a5)
{
	if (G::removals.remove_legs)
		return;

	oDrawLegs(a1, a2, a3, a4, a5);
}

void __fastcall Removals::hkFlashbangOverlay(__int64 a1, int a2, __int64* a3, __int64 a4, __m128* a5)
{
	if (G::removals.disable_flashbang)
		return;

	oFlashbangOverlay(a1, a2, a3, a4, a5);
}