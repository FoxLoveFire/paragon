#pragma once
#include "../../sdk/hooks.h"
#include "../../sdk/memory/memory.h"
#include "../../sdk/Interfaces/Interfaces.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace ModelChanger
{
	void* GetLocalPlayerInstance();

	void PrecacheResource(const std::string& path);
	void ChangeModelNow();
	void ScanModels();

	inline bool NeedSetModel = false;
	inline int SelectedIdx = 0;
	inline std::vector<std::pair<std::string, std::string>> Models;
};