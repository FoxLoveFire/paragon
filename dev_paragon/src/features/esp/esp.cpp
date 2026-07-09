#include "esp.h"

void ESP::store_players()
{
	if (!I::engine_client->valid())
		return;

	int highestIndex = I::GameEntity->Instance->GetHighestEntityIndex();

	for (int i = 0; i < highestIndex; i++)
	{
		C_CSPlayerPawn* pawn = I::GameEntity->Instance->get_entity(i);

		if (pawn)
		{
			if (pawn->m_iHealth() > 0)
			{
				std::cout << "Index -> " << i << " Health " << pawn->m_iHealth() << std::endl;

				const char* className = pawn->get_class_name();

				const uint32_t hash = HASH(className);

				if (hash == HASH("CCSPlayerController"))
				{
					CCSPlayerController* Controller = reinterpret_cast<CCSPlayerController*>(pawn);
					std::cout << Controller->getName() << std::endl;
				}
			}
		}

	}
}