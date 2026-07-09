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
    char pad_0000[7864]; //0x0000
    c_light_data_queue* light_data_queue; //0x2A08
};

class c_aggregate_object_data
{
public:
	char pad_0000[4]; //0x0000
	int count; //0x0004
	char pad_0008[40]; //0x0008
	int index; //0x0030
}; //Size: 0x0034

class c_aggregate_object_array
{
public:
	void* object; //0x0000
	c_aggregate_object_data* data; //0x0008
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