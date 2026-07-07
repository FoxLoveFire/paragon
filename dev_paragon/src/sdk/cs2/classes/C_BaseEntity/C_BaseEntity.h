#pragma once
#include "../../../utils/schema/schema.h"
#include "../../../cs2/datatypes/schema/ISchemaClass/ISchemaClass.h"
#include "../../../math/vector3d.h"
#include "../CEntityInstance/CEntityInstance.h"

class CGameSceneNode {
public:
	schema(vector3d, m_vecAbsOrigin, "CGameSceneNode->m_vecAbsOrigin");
};

class C_BaseEntity : public CEntityInstance
{
public:
	schema(CGameSceneNode*, m_pGameSceneNode, "C_BaseEntity->m_pGameSceneNode");

	schema(int, m_iMaxHealth, "C_BaseEntity->m_iMaxHealth");
	schema(int, m_iHealth, "C_BaseEntity->m_iHealth");
	schema(int, m_iTeamNum, "C_BaseEntity->m_iTeamNum");

	bool PostDataUpdate(int iUpdateType)
	{
		return Memory::call_vfunc<bool>(this, 10, iUpdateType);
	}

	bool IsBasePlayer()
	{
		const char* className = get_class_name();
		if (!className)
			return false;

		return HASH(className) == HASH("C_CSPlayerPawn");
	}

	bool IsViewmodelAttachment()
	{
		const char* className = get_class_name();
		if (!className)
			return false;

		return HASH(className) == HASH("C_CS2HudModelArms");
	}

	bool IsViewmodel()
	{
		const char* className = get_class_name();
		if (!className)
			return false;
		return HASH(className) == HASH("C_CS2HudModelWeapon");
	}

	bool IsPlayerController()
	{
		const char* className = get_class_name();
		if (!className)
			return false;

		return HASH(className) == HASH("CCSPlayerController");
	}
};
