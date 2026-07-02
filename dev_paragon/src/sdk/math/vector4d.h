#pragma once
#include "vector3d.h"

class matrix3x4_t;
class vector3d;

class vector4d
{
public:
	float x, y, z, w;
};

class alignas(16) quaternion : public vector4d
{
public:
	matrix3x4_t to_matrix(const vector3d& origin = {});
};