#pragma once
#include "../../../utils/schema/schema.h"
#include "../../../math/vector3d.h"
#include "../../classes/C_BaseEntity/C_BaseEntity.h"
#include "../../../memory/memory.h"
enum e_bones {
	BONE_ORIGIN = 0,
	BONE_PELVIS = 1,
	BONE_SPINE0 = 2,
	BONE_SPINE1 = 3,
	BONE_SPINE2 = 4,
	BONE_NECK = 6,
	BONE_HEAD = 7,
	BONE_LEFT_CLAVICLE = 8,
	BONE_LEFT_SHOULDER = 9,
	BONE_LEFT_ELBOW = 10,
	BONE_LEFT_HAND = 11,
	BONE_RIGHT_CLAVICLE = 12,
	BONE_RIGHT_SHOULDER = 13,
	BONE_RIGHT_ELBOW = 14,
	BONE_RIGHT_HAND = 15,
	BONE_LEFT_HIP = 17,
	BONE_LEFT_KNEE = 18,
	BONE_LEFT_FOOT_HEEL = 19,
	BONE_RIGHT_HIP = 20,
	BONE_RIGHT_KNEE = 21,
	BONE_RIGHT_FOOT_HEEL = 22,
	BONE_CHEST = 23,
	BONE_GUN = 24,
	BONE_LEFT_EYE = 25,
	BONE_RIGHT_EYE = 26,
	BONE_RANDOM = 27,
	BONE_CVJ_BONE = 28,
	BONE_LEFT_FOOT_TOES_T = 74,
	BONE_RIGHT_FOOT_TOES_T = 77,
	BONE_LEFT_FOOT_TOES_CT = 81,
	BONE_RIGHT_FOOT_TOES_CT = 86,
	BONE_MAX = 128
};

class C_CSPlayerPawn : public C_BaseEntity {
public:
	schema(vector3d, m_vOldOrigin, "C_BasePlayerPawn->m_vOldOrigin");
	schema(vector3d, m_vecViewOffset, "C_BaseModelEntity->m_vecViewOffset");
	C_CSPlayerPawn(uintptr_t address);

	vector3d getPosition() const;
	vector3d getEyePosition() const;
	vector3d getViewOffset() const;
	vector3d get_bone_position(int bone_index) const;

	int getIDEntIndex() const;
	int getMoveType() const;
	uintptr_t getAddress() const;
	int32_t getHealth() const;
	uint32_t getFlags() const;
	uint8_t getTeam() const;
	bool getInScoped() const;
private:
	uintptr_t address;
};