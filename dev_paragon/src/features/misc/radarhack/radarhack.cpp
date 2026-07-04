#include "radarhack.h"

void __fastcall radarhack::hkRadar(void* a1)
{
	int* radar = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(a1) + 0x17760);

	*radar = G::misc.rh_enabled ? 1 : 0;

	oRadar(a1);
}
