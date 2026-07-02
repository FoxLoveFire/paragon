#pragma once
#include "../math/vector4d.h"

class c_light_data_queue
{
public:
    char pad_0000[24]; //0x0000
    void* light_data; //0x0018
    char pad_0020[4]; //0x0020
};

class c_scene_system
{
public:
    char pad_0000[10792]; //0x0000
    c_light_data_queue* light_data_queue; //0x2A08
};


struct CSceneLightDesc
{
	char pad0[4];
	float colorR;
	float colorG;
	float colorB;
	float backup_colorR;
	float backup_colorG;
	float backup_colorB;
	char pad18[33];
	bool shadows;
	char pad3E[54];
	std::uint8_t flags;
	bool enabled;
	char pad76[44];
	vector4d vDirection;
};

struct CSceneLightObject
{
	uint64_t vtable;
	char pad8[216];
	CSceneLightDesc desc;
	char pad460[944];
};