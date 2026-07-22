#include "OverrideView.h"
#include "../../sdk/cs2/classes/C_CSPlayerPawn/C_CSPlayerPawn.h"
#include "../../sdk/hooks.h"
#include "../../sdk/Interfaces/Interfaces.h"
#include <numbers>

constexpr double degrees_to_radians(double degrees) noexcept {
	return degrees * (std::numbers::pi / 180.0);
}

void __fastcall OverrideView::hkOverrideView(void* pClientModeCSNormal, c_view_setup* pSetup)
{
    if (pSetup)
    {
        if (G::viewport.aspect_ratio > 0.01f)
        {
            float scale = pSetup->m_flAspectRatio / G::viewport.aspect_ratio;
            pSetup->m_flAspectRatio = G::viewport.aspect_ratio;
            pSetup->m_nSomeFlags |= 2;

	        oOverrideView(pClientModeCSNormal, pSetup);
            if (G::viewport.custom_fov && scale > 0.0f)
                pSetup->m_flFov = G::viewport.fov * scale;
        }
        else if (G::viewport.custom_fov)
        {
            pSetup->m_flFov = G::viewport.fov;
        }

		C_CSPlayerPawn* local_player = Hook::oGetLocalPlayer(0);
        if (local_player && G::viewport.thirdperson)
        {
            if (!I::m_csgo_input)
                return oOverrideView(pClientModeCSNormal, pSetup);

            if (!I::engine_client || !I::engine_client->valid() || local_player->getHealth() <= 0)
                return oOverrideView(pClientModeCSNormal, pSetup);

            vector3d angles = I::m_csgo_input->GetViewAngles();

            float rad_pitch = degrees_to_radians(angles.x);
            float rad_yaw = degrees_to_radians(angles.y);

            vector3d forward = {
                cosf(rad_pitch) * cosf(rad_yaw),
                cosf(rad_pitch) * sinf(rad_yaw),
                -sinf(rad_pitch)
            };

            vector3d eye_pos = local_player->getEyePosition();
            eye_pos.z += 5.f;

            float distance = 120.f;

            pSetup->m_vecOrigin = eye_pos - (forward * distance);
        }

    }
    oOverrideView(pClientModeCSNormal, pSetup);
}

bool __fastcall OverrideView::hkDrawCrosshair(void* a1)
{
    if (G::viewport.enable_crosshair)
        return true;
    return oDrawCrosshair(a1);
}