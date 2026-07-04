#include "Viewmodel.h"

void __fastcall Viewmodel::hkViewModel(__int64 a1, float* a2, float* a3)
{
	if (!G::viewmodel.is_enabled)
		oViewModel(a1, a2, a3);

	oViewModel(a1, a2, a3);

	if (!a2 || !a3)
		return;
	
	for (int i = 0; i < G::viewmodel.xyz.size(); i++)
	{
		a2[i] += G::viewmodel.xyz[i];
	}

	a3[0] += G::viewmodel.v_fov;
}