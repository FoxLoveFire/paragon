#pragma once

#include "vector3d.h"

#pragma pack(push, 4)
using matrix3x3_t = float[3][3];

class matrix3x4_t
{
public:
	matrix3x4_t() = default;
	constexpr matrix3x4_t(
		const float m00, const float m01, const float m02, const float m03,
		const float m10, const float m11, const float m12, const float m13,
		const float m20, const float m21, const float m22, const float m23);

	constexpr matrix3x4_t(const vector3d& vec_forward, const vector3d& vec_left, const vector3d& vec_up, const vector3d& vec_origin);

	float* operator[](const int index);
	const float* operator[](const int index) const;

	constexpr void set_forward(const vector3d& vec_forward);
	constexpr void set_left(const vector3d& vec_left);
	constexpr void set_up(const vector3d& vec_up);
	constexpr void set_origin(const vector3d& vec_origin);

	vector3d get_forward() const;
	vector3d get_left() const;
	vector3d get_up() const;
	vector3d get_origin() const;

	constexpr matrix3x4_t concat_transforms(const matrix3x4_t& mat_other) const;

	float arr_data[3][4] = {};
};

#pragma pack(pop)