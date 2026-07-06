#include "OverrideView.h"

void __fastcall OverrideView::hkOverrideView(void* pClientModeCSNormal, c_view_setup* pSetup)
{
    if (pSetup)
    {
        if (G::viewport.aspect_ratio > 0.01f)
        {
            float scale = pSetup->m_flAspectRatio / G::viewport.aspect_ratio;
            pSetup->m_flAspectRatio = G::viewport.aspect_ratio;
            pSetup->m_nSomeFlags |= 2;

            if (G::viewport.custom_fov && scale > 0.0f)
                pSetup->m_flFov = G::viewport.fov * scale;
        }
        else if (G::viewport.custom_fov)
        {
            pSetup->m_flFov = G::viewport.fov;
        }
    }

    oOverrideView(pClientModeCSNormal, pSetup);
}