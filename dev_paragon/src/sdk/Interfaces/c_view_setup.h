#pragma once
#include <Windows.h>
#include "../math/vector3d.h"

class c_view_setup {
private:
    char pad0[0x450];          // 0x0000
public:
    float m_flOrthoLeft;       // 0x0450
    float m_flOrthoTop;        // 0x0454
    float m_flOrthoRight;      // 0x0458
    float m_flOrthoBottom;     // 0x045C
private:
    char pad1[0x38];           // 0x0460
public:
    float m_flFov;             // 0x0498
    float m_flFovViewmodel;    // 0x049C
    vector3d m_vecOrigin;      // 0x04A0
private:
    char pad2[0xC];            // 0x04AC
public:
    vector3d m_angView;        // 0x04B8
private:
    char pad3[0x14];           // 0x04C4
public:
    float m_flAspectRatio;     // 0x04D8
private:
    char pad4[0x79];           // 0x04DC
public:
    BYTE m_nSomeFlags;         // 0x0555
};