#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdint>

typedef int32_t HSteamUser;
typedef int32_t HSteamPipe;

class ISteamFriends {
public:
    virtual const char* GetPersonaName() = 0;
};

class ISteamClient {
public:
    virtual HSteamPipe CreateSteamPipe() = 0;                                                                    
    virtual bool BReleaseSteamPipe(HSteamPipe hSteamPipe) = 0;                                                   
    virtual HSteamUser ConnectToGlobalUser(HSteamPipe hSteamPipe) = 0;                                           
    virtual HSteamUser CreateLocalUser(HSteamPipe* phSteamPipe, int eAccountType) = 0;                           
    virtual void ReleaseUser(HSteamPipe hSteamPipe, HSteamUser hUser) = 0;                                       
    virtual void* GetISteamUser(HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion) = 0;       
    virtual void* GetISteamGameServer(HSteamUser hUser, HSteamPipe hPipe, const char* pchVer) = 0;               
    virtual void SetLocalIPBinding(void* unIP, uint16_t usPort) = 0;                                             
    virtual ISteamFriends* GetISteamFriends(HSteamUser hUser, HSteamPipe hPipe, const char* pchVersion) = 0;     
};

inline ISteamFriends* GetSteamFriendsInterface() {
    HMODULE hSteamApi = GetModuleHandleA("steam_api64.dll");
    if (!hSteamApi) return nullptr;

    using SteamClientFn = ISteamClient* (*)();
    SteamClientFn pSteamClient = reinterpret_cast<SteamClientFn>(GetProcAddress(hSteamApi, "SteamClient"));
    if (!pSteamClient) return nullptr;

    ISteamClient* pClient = pSteamClient();
    if (!pClient) return nullptr;

    HSteamPipe hPipe = pClient->CreateSteamPipe();
    if (!hPipe) return nullptr;

    HSteamUser hUser = pClient->ConnectToGlobalUser(hPipe);
    if (!hUser) return nullptr;

    return pClient->GetISteamFriends(hUser, hPipe, "SteamFriends017");
}