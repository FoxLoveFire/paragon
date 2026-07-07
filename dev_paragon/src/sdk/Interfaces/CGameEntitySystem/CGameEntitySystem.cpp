#include "CGameEntitySystem.h"
#include "../../hooks.h"

void* CGameEntitySystem::GetEntityByIndex(int nIndex)
{
    return Hook::ogGetBaseEntity(this, nIndex);
}