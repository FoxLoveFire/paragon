#include "C_CSPlayerPawn.h"
C_CSPlayerPawn::C_CSPlayerPawn(uintptr_t address) : address(address) {}

vector3d C_CSPlayerPawn::getPosition() const {
	return *(vector3d*)(address + SchemaFinder::Get(HASH("C_BasePlayerPawn->m_vOldOrigin")));
}

vector3d C_CSPlayerPawn::getEyePosition() const {
	vector3d view_;
	Memory::call_vfunc<void, vector3d&>(const_cast<C_CSPlayerPawn*>(this), 170, view_);
	return view_;
}

vector3d C_CSPlayerPawn::get_bone_position(int index) const {
	if (index < 0 || index >= 128)
		return {};

	vector3d pos{};
	__try {
		uintptr_t sceneNode = *reinterpret_cast<uintptr_t*>(address + SchemaFinder::Get(HASH("C_BaseEntity->m_pGameSceneNode")));
		if (!sceneNode) return pos;

		uintptr_t modelState = sceneNode + SchemaFinder::Get(HASH("CSkeletonInstance->m_modelState"));
		uintptr_t boneArray = *reinterpret_cast<uintptr_t*>(modelState + 0x80);
		if (!boneArray) return pos;

		pos = *reinterpret_cast<vector3d*>(boneArray + index * 32);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		return vector3d{};
	}

	return pos;
}

uintptr_t C_CSPlayerPawn::getAddress() const {
	return address;
}

int C_CSPlayerPawn::getMoveType() const {
	return *reinterpret_cast<int*>((uintptr_t)this + SchemaFinder::Get(HASH("C_BaseEntity->m_nActualMoveType")));
}

uint32_t C_CSPlayerPawn::getFlags() const {
	return *reinterpret_cast<uint32_t*>((uintptr_t)this + SchemaFinder::Get(HASH("C_BaseEntity->m_fFlags")));
}

bool C_CSPlayerPawn::getInScoped() const {
	return *reinterpret_cast<bool*>((uintptr_t)this + SchemaFinder::Get(HASH("C_CSPlayerPawn->m_bIsScoped")));
}

int C_CSPlayerPawn::getIDEntIndex() const {
	return *reinterpret_cast<int*>((uintptr_t)this + SchemaFinder::Get(HASH("C_CSPlayerPawnBase->m_iIDEntIndex")));
}

int32_t C_CSPlayerPawn::getHealth() const {
	return *reinterpret_cast<uint32_t*>((uintptr_t)this + SchemaFinder::Get(HASH("C_BaseEntity->m_iHealth")));
}

uint8_t C_CSPlayerPawn::getTeam() const {
	return *reinterpret_cast<uint8_t*>((uintptr_t)this + SchemaFinder::Get(HASH("C_BaseEntity->m_iTeamNum")));
}

vector3d C_CSPlayerPawn::getViewOffset() const {
	return *reinterpret_cast<vector3d*>((uintptr_t)this + SchemaFinder::Get(HASH("C_BaseModelEntity->m_vecViewOffset")));
}