#pragma once
#include "../../cs2/classes/C_BaseEntity/C_BaseEntity.h"
#include "../../cs2/classes/C_CSPlayerPawn/C_CSPlayerPawn.h"

class CGameEntitySystem
{
public:
	template <typename T = C_BaseEntity>
	T* Get(int nIndex)
	{
		if (nIndex <= 0 || nIndex >= 0x7FFF)
			return nullptr;
		int chunkIndex = nIndex >> 9;
		int offsetInChunk = nIndex & 0x1FF;
		uintptr_t chunkPtr = *(uintptr_t*)(uintptr_t(this) + 8 * chunkIndex + 16);
		if (!chunkPtr)
			return nullptr;
		uintptr_t identityBlock = chunkPtr + (112 * offsetInChunk);
		if (!identityBlock)
			return nullptr;
		uintptr_t entityPtr = *(uintptr_t*)(identityBlock);
		if (entityPtr <= 0x1000)
			return nullptr;
		return reinterpret_cast<T*>(entityPtr);
	}

	template <typename T = C_BaseEntity>
	T* Get(const CBaseHandle hHandle)
	{
		if (!hHandle.valid())
			return nullptr;
		return Get<T>(hHandle.index());
	}

	int GetHighestEntityIndex()
	{
		return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + 0x2090);
	}

	C_CSPlayerPawn* get_entity(int index)
	{
		if (index <= 0 || index >= 0x7FFF)
			return nullptr;
		int chunkIndex = index >> 9;
		int offsetInChunk = index & 0x1FF;
		uintptr_t chunkPtr = *(uintptr_t*)(uintptr_t(this) + 8 * chunkIndex + 16);
		if (!chunkPtr)
			return nullptr;
		uintptr_t identityBlock = chunkPtr + (112 * offsetInChunk);
		if (!identityBlock)
			return nullptr;
		uintptr_t entityPtr = *(uintptr_t*)(identityBlock);
		if (entityPtr <= 0x1000)
			return nullptr;
		return reinterpret_cast<C_CSPlayerPawn*>(entityPtr);
	}

private:
	void* GetEntityByIndex(int nIndex);
};

class IGameResourceService
{
public:
	char pad00[0x58];
	CGameEntitySystem* Instance;
};