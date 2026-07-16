#include "esp.h"

namespace ESP
{
    std::vector<PlayerData> players;
    bool esp_enabled = true;

    mat4x4_t Matrix = sdk::GetViewMatrix();

    bool world_to_screen(const vector3d& world, vector3d& screen)
    {
        ImVec2 display_size = ImGui::GetIO().DisplaySize;

        float SightX = display_size.x / 2.0f;
        float SightY = display_size.y / 2.0f;

        float View = Matrix.m[3][0] * world.x + Matrix.m[3][1] * world.y + Matrix.m[3][2] * world.z + Matrix.m[3][3];
        if (View < 0.01f)
            return false;

        float x = Matrix.m[0][0] * world.x + Matrix.m[0][1] * world.y + Matrix.m[0][2] * world.z + Matrix.m[0][3];
        float y = Matrix.m[1][0] * world.x + Matrix.m[1][1] * world.y + Matrix.m[1][2] * world.z + Matrix.m[1][3];

        screen.x = SightX + (x / View) * SightX;
        screen.y = SightY - (y / View) * SightY;
        screen.z = 0.0f;

        return true;
    }


    void store_players()
    {
        if (!I::engine_client->valid())
            return;

        players.clear();

        int highestIndex = I::GameEntity->Instance->GetHighestEntityIndex();

        for (int i = 0; i < highestIndex; i++)
        {
            C_CSPlayerPawn* pawn = I::GameEntity->Instance->get_entity(i);

            if (pawn)
            {
                if (pawn->m_iHealth() > 0)
                {
                    PlayerData player;
                    player.health = pawn->m_iHealth();
                    player.position = pawn->m_vOldOrigin();
                    
                    players.push_back(player);

                    std::cout << "Index -> " << i << " Health " << pawn->m_iHealth() << std::endl;
                    std::cout << player.position << std::endl;
                }
            }
        }
    }

    void render_esp()
    {
        if (!I::engine_client->valid())
            return;

        if (!esp_enabled)
            return;

        //std::cout << sdk::GetViewMatrix() << std::endl;

        store_players();

        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        if (!drawList)
            return;

        ImVec2 screenSize = ImGui::GetIO().DisplaySize;

        for (const auto& player : players)
        {
            vector3d screenPos;
            if (!world_to_screen(player.position, screenPos))
            {
                std::cout << "!world_to_screen" << std::endl;
                continue;
            }

            if (screenPos.z < 0.01f)
                continue;

            float height = 100.0f;
            float width = 50.0f;

            float x = screenPos.x - width / 2;
            float y = screenPos.y - height;

            drawList->AddRect(
                ImVec2(x, y),
                ImVec2(x + width, y + height),
                IM_COL32(255, 0, 0, 255),
                0.0f,
                0, 
                1.0f
            );

            char healthText[16];
            sprintf_s(healthText, "HP: %d", player.health);

            std::cout << "Drawwing" << std::endl;

            drawList->AddText(
                ImVec2(x + 5, y - 20),
                IM_COL32(0, 0, 0, 255),
                healthText
            );

            drawList->AddText(
                ImVec2(x + 4, y - 21),
                IM_COL32(255, 255, 255, 255),
                healthText
            );
        }
    }
}