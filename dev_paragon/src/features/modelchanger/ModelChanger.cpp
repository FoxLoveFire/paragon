#include "ModelChanger.h"

void* ModelChanger::GetLocalPlayerInstance()
{
	return Hook::oGetLocalPlayer(0);
}

void ModelChanger::PrecacheResource(const std::string& path)
{
	if (!Hook::fnPrecache || !I::IRS) return;
	CBufferString names;
	names.Insert(0, path.c_str(), -1, false);
	Hook::fnPrecache(I::IRS, &names, "");
}

void ModelChanger::ChangeModelNow()
{
    if (SelectedIdx <= 0 || SelectedIdx >= Models.size()) return;
    std::string path = Models[SelectedIdx].second;
    if (path.empty()) return;

    void* pawn = GetLocalPlayerInstance();
    if (!pawn || !Hook::fnSetModel) {
        if (!pawn) std::cout << "Local Player Pawn not found. Can't set model." << std::endl;
        if (!Hook::fnSetModel) std::cout << "SetModel function not hooked properly." << std::endl;
        return;
    }

    std::cout << "Applying model: " << path.c_str() << std::endl;
    PrecacheResource(path);
    Hook::fnSetModel(pawn, path.c_str());
    NeedSetModel = false;
}

void ModelChanger::ScanModels()
{
    Models.clear();
    Models.push_back({ "[ OFF ]", "" });

    char cwd[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, cwd);
    std::string root = cwd;

    auto pos = root.find("bin\\win64");
    if (pos != std::string::npos) {
        root.replace(pos, 9, "csgo\\characters\\models");
    }

    std::cout << "Scanning models in root: %s" << root.c_str() << std::endl;

    int scannedCount = 0;
    if (fs::exists(root)) {
        for (auto& p : fs::recursive_directory_iterator(root)) {
            if (p.path().extension() == ".vmdl_c") {
                std::string full = p.path().string();
                std::string rel = full.substr(full.find("characters\\"));
                std::replace(rel.begin(), rel.end(), '\\', '/');
                rel = rel.substr(0, rel.find(".vmdl_c")) + ".vmdl";

                std::string fname = p.path().stem().string();
                Models.push_back({ fname, rel });
                scannedCount++;
            }
        }
    }

    std::cout << "Scan completed. Found %d custom models." << scannedCount << std::endl;
}