#include "esp.h"

static const int k_bone_connections[][2] = {
    // Spine
    { 1, 3 },   // PELVIS -> SPINE1
    { 3, 4 },   // SPINE1 -> SPINE2
    { 4, 23 },  // SPINE2 -> CHEST
    { 23, 6 },  // CHEST -> NECK
    { 6, 7 },   // NECK -> HEAD

    // Left arm
    { 6, 9 },    // NECK -> SHOULDER_L
    { 9, 10 },   // SHOULDER_L -> ELBOW_L
    { 10, 11 },  // ELBOW_L -> HAND_L

    // Right arm
    { 6, 13 },   // NECK -> SHOULDER_R
    { 13, 14 },  // SHOULDER_R -> ELBOW_R
    { 14, 15 },  // ELBOW_R -> HAND_R

    // Left leg
    { 1, 17 },   // PELVIS -> HIP_L
    { 17, 18 },  // HIP_L -> KNEE_L
    { 18, 19 },  // KNEE_L -> FOOT_HEEL_L
    { 19, 74 },  // FOOT_HEEL_L -> FOOT_TOES_L_T

    // Right leg
    { 1, 20 },   // PELVIS -> HIP_R
    { 20, 21 },  // HIP_R -> KNEE_R
    { 21, 22 },  // KNEE_R -> FOOT_HEEL_R
    { 22, 77 },  // FOOT_HEEL_R -> FOOT_TOES_R_T
};

using namespace Esp;

LocalPlayerCached cached_local;
std::vector<PlayerCache> cached_players;

void Visuals::init() {
    uintptr_t scan = Memory::pattern_scan("48 8D 0D ? ? ? ? 48 C1 E0 06 48 03 C1", "client.dll");
    if (!scan) scan = Memory::pattern_scan("48 8D 0D ? ? ? ? 48 C1 E0 06", "client.dll");
    if (scan) {
        viewMatrix.viewMatrix = (viewmatrix_t*)Memory::getAbsoluteAddress(scan, 3, 0);
    }
    else {
        printf("[!] Failed to find ViewMatrix pattern!\n");
    }
}

void Esp::cache()
{
    if (!I::engine_client->valid())
        return;

    cached_players.clear();

    int nMaxHighestEntity = I::GameEntity->Instance->GetHighestEntityIndex();


    int found_controllers = 0;
    int alive_cached = 0;

    for (int i = 1; i <= nMaxHighestEntity; i++)
    {
        auto Entity = I::GameEntity->Instance->Get(i);
        if (!Entity)
            continue;

        if (!Entity->handle().valid())
            continue;

        const char* className = Entity->get_class_name();
        if (!className)
            continue;

        const uint32_t hash = HASH(className);

        PlayerType_t type = none;

        if (hash == HASH("CCSPlayerController"))
        {
            found_controllers++;
            type = none; int health = 0;
            vector3d position; vector3d viewOffset;
            const char* name = "none"; const char* weapon_name = "none";

            CCSPlayerController* Controller = reinterpret_cast<CCSPlayerController*>(Entity);
            if (!Controller)
                continue;

            if (!Controller->m_hPawn().valid()) {
                continue;
            }

            //@handle caching local player 
            if (Controller->IsLocalPlayer()) {
                auto LocalPlayer = I::GameEntity->Instance->Get<C_CSPlayerPawn>(Controller->m_hPawn().index());
                if (!LocalPlayer) {
                    cached_local.reset();
                    continue;
                }

                cached_local.alive = LocalPlayer->m_iHealth() > 0;
                if (LocalPlayer->m_iHealth() > 0) {
                    __try {
                        CGameSceneNode* nodeLocal = LocalPlayer->m_pGameSceneNode();
                        if (nodeLocal) {
                            cached_local.poisition = nodeLocal->m_vecAbsOrigin();
                        }
                        else {
                            cached_local.poisition = LocalPlayer->m_vOldOrigin();
                        }
                    }
                    __except (EXCEPTION_EXECUTE_HANDLER) {
                        cached_local.poisition = vector3d(0, 0, 0);
                    }

                    cached_local.health = LocalPlayer->m_iHealth();
                    cached_local.handle = LocalPlayer->handle().index();
                    cached_local.team = LocalPlayer->m_iTeamNum();

                    vector3d localView = LocalPlayer->m_vecViewOffset();
                    if (localView.z < 0.0f || localView.z > 100.0f) localView = vector3d(0, 0, 65.0f);
                }
                else {
                    cached_local.reset();
                }
            }
            else // @handle only players
            {
                auto Player = I::GameEntity->Instance->Get<C_CSPlayerPawn>(Controller->m_hPawn().index());
                if (!Player) {
                    continue;
                }

                if (Player->m_iHealth() <= 0)
                    continue;

                health = Player->m_iHealth();
                name = Controller->getName();

                __try {
                    CGameSceneNode* node = Player->m_pGameSceneNode();
                    if (node) {
                        position = node->m_vecAbsOrigin();
                    }
                    else {
                        position = Player->m_vOldOrigin();
                    }
                }
                __except (EXCEPTION_EXECUTE_HANDLER) {
                    position = vector3d(0, 0, 0);
                }

                viewOffset = Player->m_vecViewOffset();
                if (viewOffset.z < 0.0f || viewOffset.z > 100.0f) {
                    viewOffset = vector3d(0.0f, 0.0f, 65.0f); // Default standard eye offset
                }

                cached_players.emplace_back(Entity, Player, Player->handle(),
                    type, health, name,
                    weapon_name, position, viewOffset, Player->m_iTeamNum());
                alive_cached++;
            }
        }
    }
}

void Visuals::esp() {


    Esp::cache();

    C_CSPlayerPawn* localPawn = Hook::oGetLocalPlayer(0);
    if (!localPawn) {
        return;
    }

    if (cached_players.empty())
        return;

    for (const auto& Player : cached_players)
    {
        if (!Player.handle.valid() || Player.health <= 0 || Player.handle.index() == INVALID_EHANDLE_INDEX)
            continue;

        if (G::esp_config.teamcheck && (Player.team_num == localPawn->getTeam()))
            continue;

        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        vector3d feetPos = Player.position;
        vector3d headPos = Player.position + Player.viewOffset;

        vector3d feetScreen, headScreen;
        if (!viewMatrix.WorldToScreen(feetPos, feetScreen) ||
            !viewMatrix.WorldToScreen(headPos, headScreen))
            continue;

        float boxHeight = (feetScreen.y - headScreen.y) * 1.3f;
        float boxWidth = boxHeight / 2.0f;

        float centerX = feetScreen.x;
        float boxX = centerX - (boxWidth / 2.0f);

        float boxY = headScreen.y - (boxHeight - (feetScreen.y - headScreen.y)) / 2.0f;

        ImGuiIO& io = ImGui::GetIO();
        ImVec4 espColorWithAlpha = {
            static_cast<float>(G::esp_config.enemy.r()) / 255.0f,
            static_cast<float>(G::esp_config.enemy.g()) / 255.0f,
            static_cast<float>(G::esp_config.enemy.b()) / 255.0f,
            static_cast<float>(G::esp_config.enemy.a()) / 255.0f
        };
        ImU32 boxColor = ImGui::ColorConvertFloat4ToU32({ 1, 0, 0, 1 });
        ImU32 fillColor = ImGui::ColorConvertFloat4ToU32(espColorWithAlpha);

        if (G::esp_config.espFill) {
            drawList->AddRectFilled(
                ImVec2(boxX, boxY),
                ImVec2(boxX + boxWidth, boxY + boxHeight),
                fillColor
            );
        }

        if (G::esp_config.esp) {
            drawList->AddRect(
                ImVec2(boxX, boxY),
                ImVec2(boxX + boxWidth, boxY + boxHeight),
                boxColor,
                0.0f,
                0,
                G::esp_config.espThickness
            );
        }

        if (G::esp_config.showHealth) {
            int health = Player.health;
            float healthHeight = boxHeight * (static_cast<float>(health) / 100.0f);
            float barWidth = 4.0f;
            float barX = boxX - (barWidth + 2);
            float barY = boxY + (boxHeight - healthHeight);

            drawList->AddRectFilled(
                ImVec2(barX, boxY),
                ImVec2(barX + barWidth, boxY + boxHeight),
                IM_COL32(70, 70, 70, 255)
            );

            ImU32 healthColor = IM_COL32(
                static_cast<int>((100 - health) * 2.55f),
                static_cast<int>(health * 2.55f),
                0,
                255
            );
            drawList->AddRectFilled(
                ImVec2(barX, barY),
                ImVec2(barX + barWidth, barY + healthHeight),
                healthColor
            );

            std::string displayText = "[" + std::to_string(health) + "HP]";
            ImVec2 textSize = ImGui::CalcTextSize(displayText.c_str());
            float textX = boxX + (boxWidth - textSize.x) / 2;
            float textY = boxY + boxHeight + 2;
            drawList->AddText(
                ImVec2(textX + 1, textY + 1),
                IM_COL32(0, 0, 0, 255),
                displayText.c_str()
            );
            drawList->AddText(
                ImVec2(textX, textY),
                IM_COL32(255, 255, 255, 255),
                displayText.c_str()
            );
        }

        if (G::esp_config.showNameTags) {
            const char* playerNameUtf8 = Player.name;
            ImVec2 nameSize = ImGui::CalcTextSize(playerNameUtf8);

            float nameX = boxX + (boxWidth - nameSize.x) / 2;
            float nameY = boxY - nameSize.y - 2;

            drawList->AddText(
                ImVec2(nameX + 1, nameY + 1),
                IM_COL32(0, 0, 0, 255),
                playerNameUtf8
            );

            drawList->AddText(
                ImVec2(nameX, nameY),
                IM_COL32(255, 255, 255, 255),
                playerNameUtf8
            );
        }
    }
}