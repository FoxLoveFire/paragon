#include "silentaim.h"

vector3d GetEntityEyePos(const C_CSPlayerPawn* Entity)
{
    if (!Entity)
        return {};

    uintptr_t game_scene_node = *reinterpret_cast<uintptr_t*>((uintptr_t)Entity +
        SchemaFinder::Get(HASH("C_BaseEntity->m_pGameSceneNode")));

    auto Origin = *reinterpret_cast<vector3d*>(game_scene_node +
        SchemaFinder::Get(HASH("CGameSceneNode->m_vecAbsOrigin")));
    auto ViewOffset = *reinterpret_cast<vector3d*>((uintptr_t)Entity +
        SchemaFinder::Get(HASH("C_BaseModelEntity->m_vecViewOffset")));

    uintptr_t model = *reinterpret_cast<uintptr_t*>(game_scene_node +
        SchemaFinder::Get(HASH("CSkeletonInstance->m_modelState")) + 0x80);

    auto head = *reinterpret_cast<vector3d*>(model + static_cast<unsigned long long>(6) * 32);

    vector3d result;
    result = head;

    if (!std::isfinite(result.x) || !std::isfinite(result.y) || !std::isfinite(result.z))
        return {};

    return result;
}

inline QAngle_t CalcAngles(vector3d viewPos, vector3d aimPos) {
    QAngle_t angle = { 0, 0, 0 };
    vector3d delta = aimPos - viewPos;

    angle.x = -asin(delta.z / delta.length()) * (180.0f / 3.141592654f);
    angle.y = atan2(delta.y, delta.x) * (180.0f / 3.141592654f);

    return angle;
}

inline float GetFov(const QAngle_t& viewAngle, const QAngle_t& aimAngle) {
    QAngle_t delta = (aimAngle - viewAngle).Normalize();
    return sqrtf(powf(delta.x, 2.0f) + powf(delta.y, 2.0f));
}

void silentaim::runSilentAim(CCSGOInput* pInput, CUserCmd* pCmd)
{
    //std::cout << "[SilentAim] runSilentAim called" << std::endl;

    //if (!I::engine_client->valid())
    //{
    //    std::cout << "[SilentAim] Engine client invalid" << std::endl;
    //    return;
    //}

    //C_CSPlayerPawn* localPlayer = Hook::oGetLocalPlayer(0);
    //if (!localPlayer || localPlayer->m_iHealth() <= 0)
    //{
    //    std::cout << "[SilentAim] Local player invalid or dead" << std::endl;
    //    return;
    //}

    //std::cout << "[SilentAim] Local player valid, health: " << localPlayer->m_iHealth()
    //    << ", team: " << localPlayer->m_iTeamNum() << std::endl;

    //vector3d localEyePos = GetEntityEyePos(localPlayer);
    //QAngle_t viewAngles = pInput->GetViewAngles();
    //C_CSPlayerPawn* bestTarget = nullptr;
    //QAngle_t bestAngle = { 0, 0, 0 };
    //int playersChecked = 0;

    //for (int i = 1; i <= I::GameEntity->Instance->GetHighestEntityIndex(); i++)
    //{
    //    auto entity = I::GameEntity->Instance->Get(i);
    //    if (!entity || !entity->handle().valid())
    //        continue;

    //    const char* className = entity->get_class_name();
    //    if (!className || HASH(className) != HASH("C_CSPlayerPawn"))
    //        continue;

    //    C_CSPlayerPawn* pawn = reinterpret_cast<C_CSPlayerPawn*>(entity);

    //    if (pawn == localPlayer ||
    //        pawn->m_iHealth() <= 0 ||
    //        pawn->m_iTeamNum() == localPlayer->m_iTeamNum())
    //        continue;

    //    playersChecked++;
    //   
    //}
}