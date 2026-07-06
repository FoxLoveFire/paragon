#include "Viewmodel.h"

void __fastcall Viewmodel::hkViewModel(__int64 a1, float* a2, float* a3)
{
    if (!G::viewmodel.is_enabled || !a2 || !a3)
    {
        oViewModel(a1, a2, a3);
        return;
    }

    oViewModel(a1, a2, a3);

    if (G::viewmodel.xyz.size() >= 3)
    {
        a2[0] += G::viewmodel.xyz[0];
        a2[1] += G::viewmodel.xyz[1];
        a2[2] += G::viewmodel.xyz[2];
    }

    a3[0] += G::viewmodel.v_fov;
}