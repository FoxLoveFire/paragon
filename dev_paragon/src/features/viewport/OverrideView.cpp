#include "OverrideView.h"
#include "../../sdk/cs2/classes/C_CSPlayerPawn/C_CSPlayerPawn.h"
#include "../../sdk/hooks.h"
#include "../../sdk/Interfaces/Interfaces.h"
#include <numbers>

constexpr float degrees_to_radians(float degrees) noexcept {
    return degrees * (std::numbers::pi_v<float> / 180.0f);
}

void __fastcall OverrideView::hkOverrideView(void* pClientModeCSNormal, c_view_setup* pSetup)
{
    if (pSetup)
    {
        if (G::viewport.aspect_ratio > 0.01f)
        {
            if (pSetup->m_flAspectRatio > 0.01f)
            {
                float scale = pSetup->m_flAspectRatio / G::viewport.aspect_ratio;
                if (G::viewport.custom_fov && scale > 0.0f)
                    pSetup->m_flFov = G::viewport.fov * scale;
            }
            else if (G::viewport.custom_fov)
            {
                pSetup->m_flFov = G::viewport.fov;
            }

            pSetup->m_flAspectRatio = G::viewport.aspect_ratio;
            pSetup->m_nSomeFlags |= 2;
        }
        else if (G::viewport.custom_fov)
        {
            pSetup->m_flFov = G::viewport.fov;
        }

        if (G::viewport.thirdperson)
        {
            if (!I::engine_client || !I::engine_client->valid())
            {
                if (oOverrideView) oOverrideView(pClientModeCSNormal, pSetup);
                return;
            }

            if (!I::m_csgo_input)
            {
                if (oOverrideView) oOverrideView(pClientModeCSNormal, pSetup);
                return;
            }

            if (!Hook::oGetLocalPlayer)
            {
                if (oOverrideView) oOverrideView(pClientModeCSNormal, pSetup);
                return;
            }

            void* local_controller = Hook::oGetLocalPlayer(0);

            if (!local_controller)
            {
                if (oOverrideView) oOverrideView(pClientModeCSNormal, pSetup);
                return;
            }

            uint32_t pawn_handle = *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(local_controller) + 0x914);

            if (!I::GameEntity || !I::GameEntity->Instance)
            {
                if (oOverrideView) oOverrideView(pClientModeCSNormal, pSetup);
                return;
            }

            C_CSPlayerPawn* local_player = I::GameEntity->Instance->Get<C_CSPlayerPawn>(pawn_handle & 0x7FFF);

            if (!local_player || local_player->getHealth() <= 0)
            {
                if (oOverrideView) oOverrideView(pClientModeCSNormal, pSetup);
                return;
            }

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

            float distance = G::viewport.thirdperson_dist;
            pSetup->m_vecOrigin = eye_pos - (forward * distance);
        }
    }

    if (oOverrideView)
        oOverrideView(pClientModeCSNormal, pSetup);
}
