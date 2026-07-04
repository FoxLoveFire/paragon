#include "OverrideView.h"

void __fastcall OverrideView::hkOverrideView(void* pClientModeCSNormal, c_view_setup* pSetup)
{
	float scale = pSetup->m_flAspectRatio / G::viewport.aspect_ratio;
	pSetup->m_flAspectRatio = G::viewport.aspect_ratio;
	pSetup->m_nSomeFlags |= 2;
	
	if (G::viewport.custom_fov)
		pSetup->m_flFov = G::viewport.fov * scale;

	oOverrideView(pClientModeCSNormal, pSetup);
}