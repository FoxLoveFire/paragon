#pragma once
#include <Windows.h>
#include "Interfaces/Interfaces.h"
#include "utils/cutlbuffer/cutlbuffer.h"
#include "utils/keyvalues/keyvalues.h"
#include "math/mat4x4.h"

class CUtlBuffer;
class CUtlString;
class CKeyValues3;
struct KV3ID_t;

namespace sdk
{
	inline void(__fastcall* EnsureCapacityBuffer)(CUtlBuffer*, int) = nullptr;
	inline CUtlBuffer* (__fastcall* ConstructUtlBuffer)(CUtlBuffer*, int, int, int) = nullptr;
	inline void(__fastcall* PutUtlString)(CUtlBuffer*, const char*);
	inline bool(__fastcall* LoadKeyValues)(CKeyValues3*, void*, CUtlBuffer*, const KV3ID_t*, const char*);
	inline std::int64_t(__fastcall* CreateMaterial)(void*, void**, const char*, void*, void*, char);


	mat4x4_t GetViewMatrix();
	bool initialize();
};