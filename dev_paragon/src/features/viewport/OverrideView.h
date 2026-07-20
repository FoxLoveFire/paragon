#pragma once
#include "../../sdk/Interfaces/c_view_setup.h"
#include "../../../globals.h"

namespace OverrideView
{
	using oOverrideViewFn = void(__fastcall*)(void* pClientModeCSNormal, c_view_setup* pSetup);
	inline oOverrideViewFn oOverrideView = nullptr;
	void __fastcall hkOverrideView(void* pClientModeCSNormal, c_view_setup* pSetup);

	using oDrawCrosshairFn = bool(__fastcall*)(void* a1);
	inline oDrawCrosshairFn oDrawCrosshair = nullptr;
	bool __fastcall hkDrawCrosshair(void *a1);
};