#pragma once 
#include <Windows.h>
#include <string> 
#include <limits> 
#include <cstdint>
#include "../../../memory/memory.h"
#include "../../../math/QAngle_t.h"
#include "../../../math/vector3d.h"

enum ECommandButtons : std::uint64_t
{
	IN_ATTACK = 1ULL << 0,
	IN_JUMP = 1ULL << 1,
	IN_DUCK = 1ULL << 2,
	IN_FORWARD = 1ULL << 3,
	IN_BACK = 1ULL << 4,
	IN_USE = 1ULL << 5,
	IN_LEFT = 1ULL << 7,
	IN_RIGHT = 1ULL << 8,
	IN_MOVELEFT = 1ULL << 9,
	IN_MOVERIGHT = 1ULL << 10,
	IN_SECOND_ATTACK = 1ULL << 11,
	IN_RELOAD = 1ULL << 13,
	IN_SPRINT = 1ULL << 16,
	IN_JOYAUTOSPRINT = 1ULL << 17,
	IN_SHOWSCORES = 1ULL << 33,
	IN_ZOOM = 1ULL << 34,
	IN_LOOKATWEAPON = 1ULL << 35
};

enum ESubtickMoveStepBits : std::uint32_t
{
	MOVESTEP_BITS_BUTTON = 0x1U,
	MOVESTEP_BITS_PRESSED = 0x2U,
	MOVESTEP_BITS_WHEN = 0x4U,
	MOVESTEP_BITS_ANALOG_FORWARD_DELTA = 0x8U,
	MOVESTEP_BITS_ANALOG_LEFT_DELTA = 0x10U
};

enum EInputHistoryBits : std::uint32_t
{
	INPUT_HISTORY_BITS_VIEWANGLES = 0x1U,
	INPUT_HISTORY_BITS_SHOOTPOSITION = 0x2U,
	INPUT_HISTORY_BITS_TARGETHEADPOSITIONCHECK = 0x4U,
	INPUT_HISTORY_BITS_TARGETABSPOSITIONCHECK = 0x8U,
	INPUT_HISTORY_BITS_TARGETANGCHECK = 0x10U,
	INPUT_HISTORY_BITS_CL_INTERP = 0x20U,
	INPUT_HISTORY_BITS_SV_INTERP0 = 0x40U,
	INPUT_HISTORY_BITS_SV_INTERP1 = 0x80U,
	INPUT_HISTORY_BITS_PLAYER_INTERP = 0x100U,
	INPUT_HISTORY_BITS_RENDERTICKCOUNT = 0x200U,
	INPUT_HISTORY_BITS_RENDERTICKFRACTION = 0x400U,
	INPUT_HISTORY_BITS_PLAYERTICKCOUNT = 0x800U,
	INPUT_HISTORY_BITS_PLAYERTICKFRACTION = 0x1000U,
	INPUT_HISTORY_BITS_FRAMENUMBER = 0x2000U,
	INPUT_HISTORY_BITS_TARGETENTINDEX = 0x4000U
};

enum EButtonStatePBBits : std::uint32_t
{
	BUTTON_STATE_PB_BITS_BUTTONSTATE1 = 0x1U,
	BUTTON_STATE_PB_BITS_BUTTONSTATE2 = 0x2U,
	BUTTON_STATE_PB_BITS_BUTTONSTATE3 = 0x4U
};

enum EBaseCmdBits : std::uint32_t
{
	BASE_BITS_MOVE_CRC = 0x1U,
	BASE_BITS_BUTTONPB = 0x2U,
	BASE_BITS_VIEWANGLES = 0x4U,
	BASE_BITS_COMMAND_NUMBER = 0x8U,
	BASE_BITS_CLIENT_TICK = 0x10U,
	BASE_BITS_FORWARDMOVE = 0x20U,
	BASE_BITS_LEFTMOVE = 0x40U,
	BASE_BITS_UPMOVE = 0x80U,
	BASE_BITS_IMPULSE = 0x100U,
	BASE_BITS_WEAPON_SELECT = 0x200U,
	BASE_BITS_RANDOM_SEED = 0x400U,
	BASE_BITS_MOUSEDX = 0x800U,
	BASE_BITS_MOUSEDY = 0x1000U,
	BASE_BITS_CONSUMED_SERVER_ANGLE = 0x2000U,
	BASE_BITS_CMD_FLAGS = 0x4000U,
	BASE_BITS_ENTITY_HANDLE = 0x8000U
};

enum ECSGOUserCmdBits : std::uint32_t
{
	CSGOUSERCMD_BITS_BASECMD = 0x1U,
	CSGOUSERCMD_BITS_LEFTHAND = 0x2U,
	CSGOUSERCMD_BITS_ATTACK3START = 0x4U,
	CSGOUSERCMD_BITS_ATTACK1START = 0x8U,
	CSGOUSERCMD_BITS_ATTACK2START = 0x10U
};


template <typename T>
struct RepeatedPtrField_t
{
	struct Rep_t
	{
		int nAllocatedSize;
		T* tElements[1];
	};

	void* pArena;
	int   nCurrentSize;
	int   nTotalSize;
	Rep_t* pRep;

	T* add(T* element)
	{
		static auto add_to_rep_addr = reinterpret_cast<T * (__fastcall*)(RepeatedPtrField_t*, T*)>
		(Memory::getAbsoluteAddress(Memory::pattern_scan("E8 ? ? ? ? 4C 8B E0 48 8B 44 24 ? 4C 8B CF", "client.dll"), 
		0x1));
		return add_to_rep_addr(this, element);
	}
};

class CBasePB
{
public:
	char pad00[0x8];
	std::uint32_t nHasBits;   // 0x8 
	std::uint64_t nCachedBits;// 0x10 

	void SetBits(std::uint64_t nBits)
	{
		nCachedBits |= nBits;
	}
};
static_assert(sizeof(CBasePB) == 0x18);

class CMsgQAngle : public CBasePB
{
public:
	QAngle_t angValue; // 0x18 
};
static_assert(sizeof(CMsgQAngle) == 0x28);

class CMsgVector : public CBasePB
{
public:
	vector3d vecValue; // 0x18 
};

class CCSGOInterpolationInfoPB : public CBasePB
{
public:
	float flFraction; // 0x18 
	int   nSrcTick;   // 0x1C 
	int   nDstTick;   // 0x20 
};
static_assert(sizeof(CCSGOInterpolationInfoPB) == 0x28);

class CCSGOInputHistoryEntryPB : public CBasePB
{
public:
	CMsgQAngle* pViewAngles;              // 0x18 
	CMsgVector* pShootPosition;           // 0x20 
	CMsgVector* pTargetHeadPositionCheck; // 0x28 
	CMsgVector* pTargetAbsPositionCheck;  // 0x30 
	CMsgQAngle* pTargetAngPositionCheck;  // 0x38 
	CCSGOInterpolationInfoPB* cl_interp;                // 0x40 
	CCSGOInterpolationInfoPB* sv_interp0;               // 0x48 
	CCSGOInterpolationInfoPB* sv_interp1;               // 0x50 
	CCSGOInterpolationInfoPB* player_interp;            // 0x58 
	int   nRenderTickCount;    // 0x60 
	float flRenderTickFraction;// 0x64 
	int   nPlayerTickCount;    // 0x68 
	float flPlayerTickFraction;// 0x6C 
	int   nFrameNumber;        // 0x70 
	int   nTargetEntIndex;     // 0x74 
};
static_assert(sizeof(CCSGOInputHistoryEntryPB) == 0x78);

struct CInButtonStatePB : CBasePB
{
	std::uint64_t nValue;
	std::uint64_t nValueChanged;
	std::uint64_t nValueScroll;
};
static_assert(sizeof(CInButtonStatePB) == 0x30);

struct CSubtickMoveStep : CBasePB
{
	std::uint64_t nButton;
	bool          bPressed;
	float         flWhen;
	float         flAnalogForwardDelta;
	float         flAnalogLeftDelta;
};
static_assert(sizeof(CSubtickMoveStep) == 0x30);

class CBaseUserCmdPB : public CBasePB
{
public:
	RepeatedPtrField_t<CSubtickMoveStep> subtickMovesField; // 0x18 
	std::string* strMoveCrc;        // 0x30 
	CInButtonStatePB* pInButtonState;    // 0x38 
	CMsgQAngle* pViewAngles;       // 0x40 
	std::int32_t       nLegacyCommandNumber;
	std::int32_t       nClientTick;
	float              flForwardMove;
	float              flSideMove;
	float              flUpMove;
	std::int32_t       nImpulse;
	std::int32_t       nWeaponSelect;
	std::int32_t       nRandomSeed;
	std::int32_t       nMousedX;
	std::int32_t       nMousedY;
	std::uint32_t      nConsumedServerAngleChanges;
	std::int32_t       nCmdFlags;
	std::uint32_t      nPawnEntityHandle;

	CSubtickMoveStep* add_subtick_move()
	{
		
		using fn_t = CSubtickMoveStep * (__fastcall*)(void*);
		static fn_t fn = reinterpret_cast<fn_t>(
			Memory::getAbsoluteAddress(Memory::pattern_scan("E8 ? ? ? ? 48 8B D0 48 8D 4F 18 E8 ? ? ? ? 48 8B D0", "client.dll"), 0x1));

		if (subtickMovesField.pRep && subtickMovesField.nCurrentSize < subtickMovesField.pRep->nAllocatedSize)
			return subtickMovesField.pRep->tElements[subtickMovesField.nCurrentSize++];

		CSubtickMoveStep* subtick = fn(nullptr);
		subtickMovesField.add(subtick);
		return subtick;
	}
};
static_assert(sizeof(CBaseUserCmdPB) == 0x80);

class CCSGOUserCmdPB
{
public:
	std::uint32_t nHasBits;   // 0x0 
	std::uint64_t nCachedSize;// 0x4 
	RepeatedPtrField_t<CCSGOInputHistoryEntryPB> inputHistoryField; // 0xC 
	CBaseUserCmdPB* pBaseCmd; // 0x24 
	bool            bLeftHandDesired;
	std::int32_t    nAttack3StartHistoryIndex;
	std::int32_t    nAttack1StartHistoryIndex;
	std::int32_t    nAttack2StartHistoryIndex;

	void CheckAndSetBits(std::uint32_t nBits)
	{
		if (!(nHasBits & nBits))
			nHasBits |= nBits;
	}
};
static_assert(sizeof(CCSGOUserCmdPB) == 0x40);

struct CInButtonState
{
public:
	char pad00[0x8];
	std::uint64_t nValue;        // 0x8 
	std::uint64_t nValueChanged; // 0x10 
	std::uint64_t nValueScroll;  // 0x18 
};
static_assert(sizeof(CInButtonState) == 0x20);

class CUserCmd
{
public:
	char pad00[0x8];
	char pad01[0x10];
	CCSGOUserCmdPB csgoUserCmd; // 0x18
	CInButtonState nButtons;    // 0x58
	char pad02[0x20];

	CCSGOInputHistoryEntryPB* GetInputHistoryEntry(int nIndex)
	{
		if (!csgoUserCmd.inputHistoryField.pRep)
			return nullptr;
		if (nIndex >= csgoUserCmd.inputHistoryField.pRep->nAllocatedSize ||
			nIndex >= csgoUserCmd.inputHistoryField.nCurrentSize)
			return nullptr;
		return csgoUserCmd.inputHistoryField.pRep->tElements[nIndex];
	}

	bool SetSubTickAngle(const QAngle_t& angView)
	{
		if (!csgoUserCmd.inputHistoryField.pRep)
			return false;

		bool bWroteAny = false;
		for (int i = 0; i < csgoUserCmd.inputHistoryField.nCurrentSize; i++)
		{
			CCSGOInputHistoryEntryPB* pEntry = GetInputHistoryEntry(i);
			if (!pEntry || !pEntry->pViewAngles)
				continue;
			pEntry->pViewAngles->angValue = angView;
			pEntry->SetBits(INPUT_HISTORY_BITS_VIEWANGLES);
			bWroteAny = true;
		}

		return bWroteAny;
	}

	bool IsButtonPressed(std::uint64_t button) const
	{
		if (!csgoUserCmd.pBaseCmd || !csgoUserCmd.pBaseCmd->pInButtonState)
			return false;
		return (csgoUserCmd.pBaseCmd->pInButtonState->nValue & button) != 0;
	}
};
static_assert(sizeof(CUserCmd) == 0x98);


class CCSGOInput
{
public:
	char pad00[0x250];                     // 0x0000 
	bool          block_shot;           // 0x0250 
	bool          in_thirdperson;       // 0x0251 
	char pad01[0x6];                       // 0x0252 
	vector3d	  third_person_angles;  // 0x0258 
	char pad02[0x14];                      // 0x0264 
	std::uint64_t button_pressed;       // 0x0278 
	std::uint64_t mouse_button_pressed; // 0x0280 
	std::uint64_t button_un_pressed;    // 0x0288 
	std::uint64_t keyboard_copy;        // 0x0290 
	float         forward_move;         // 0x0298 
	float         left_move;            // 0x029C 
	float         up_move;              // 0x02A0 
	std::int32_t  mouse_delta_x;        // 0x02A4 
	std::int32_t  mouse_delta_y;        // 0x02A8 
	std::int32_t  subtick_count;        // 0x02AC 
	void* subticks[0xC];        // 0x02B0 
	QAngle_t      view_angles;          // 0x03D0 
	std::int32_t  target_entity_index;  // 0x03DC 
	char pad03[0x230];                     // 0x03E0 
	std::int32_t  attack_history_1;     // 0x0610 
	std::int32_t  attack_history_2;     // 0x0614 
	std::int32_t  attack_history_3;     // 0x0618 
	char pad04[0x4];                       // 0x061C 
	std::int32_t  message_size;         // 0x0620 
	char pad05[0x4];                       // 0x0624 
	void* message;              // 0x0628 

	void SetViewAngle(vector3d& angView)
	{
		using set_view_angles_t = void(__fastcall*)(CCSGOInput*, int, vector3d&);
		static set_view_angles_t set_view_angles = reinterpret_cast<set_view_angles_t>
		(Memory::pattern_scan("85 D2 75 3D 48 63 81 ? ? ? ?", "client.dll"));

		set_view_angles(this, 0, angView);
	}

	vector3d& GetViewAngles()
	{
		using get_view_angles_t = void* (__fastcall*)(CCSGOInput*, int);
		static get_view_angles_t get_view_angles = reinterpret_cast<get_view_angles_t>
		(Memory::pattern_scan("4C 8B C1 85 D2 74 08 48 8D 05 ? ? ? ? C3", "client.dll"));
		
		return *reinterpret_cast<vector3d*>(get_view_angles(this, 0));
	}
};
