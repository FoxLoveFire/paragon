#include "mat3x4.h"

constexpr matrix3x4_t::matrix3x4_t(
	const float m00, const float m01, const float m02, const float m03,
	const float m10, const float m11, const float m12, const float m13,
	const float m20, const float m21, const float m22, const float m23)
{
	arr_data[0][0] = m00;
	arr_data[0][1] = m01;
	arr_data[0][2] = m02;
	arr_data[0][3] = m03;
	arr_data[1][0] = m10;
	arr_data[1][1] = m11;
	arr_data[1][2] = m12;
	arr_data[1][3] = m13;
	arr_data[2][0] = m20;
	arr_data[2][1] = m21;
	arr_data[2][2] = m22;
	arr_data[2][3] = m23;
}

constexpr matrix3x4_t::matrix3x4_t(const vector3d& vec_forward, const vector3d& vec_left, const vector3d& vec_up, const vector3d& vec_origin)
{
	set_forward(vec_forward);
	set_left(vec_left);
	set_up(vec_up);
	set_origin(vec_origin);
}

float* matrix3x4_t::operator[](const int index)
{
	return arr_data[index];
}

const float* matrix3x4_t::operator[](const int index) const
{
	return arr_data[index];
}

constexpr void matrix3x4_t::set_forward(const vector3d& vec_forward)
{
	arr_data[0][0] = vec_forward.x;
	arr_data[1][0] = vec_forward.y;
	arr_data[2][0] = vec_forward.z;
}

constexpr void matrix3x4_t::set_left(const vector3d& vec_left)
{
	arr_data[0][1] = vec_left.x;
	arr_data[1][1] = vec_left.y;
	arr_data[2][1] = vec_left.z;
}

constexpr void matrix3x4_t::set_up(const vector3d& vec_up)
{
	arr_data[0][2] = vec_up.x;
	arr_data[1][2] = vec_up.y;
	arr_data[2][2] = vec_up.z;
}

constexpr void matrix3x4_t::set_origin(const vector3d& vec_origin)
{
	arr_data[0][3] = vec_origin.x;
	arr_data[1][3] = vec_origin.y;
	arr_data[2][3] = vec_origin.z;
}

vector3d matrix3x4_t::get_forward() const
{
	return { arr_data[0][0], arr_data[1][0], arr_data[2][0] };
}

vector3d matrix3x4_t::get_left() const
{
	return { arr_data[0][1], arr_data[1][1], arr_data[2][1] };
}

vector3d matrix3x4_t::get_up() const
{
	return { arr_data[0][2], arr_data[1][2], arr_data[2][2] };
}

vector3d matrix3x4_t::get_origin() const
{
	return { arr_data[0][3], arr_data[1][3], arr_data[2][3] };
}

constexpr matrix3x4_t matrix3x4_t::concat_transforms(const matrix3x4_t& mat_other) const
{
	return {
		arr_data[0][0] * mat_other.arr_data[0][0] + arr_data[0][1] * mat_other.arr_data[1][0] + arr_data[0][2] * mat_other.arr_data[2][0],
		arr_data[0][0] * mat_other.arr_data[0][1] + arr_data[0][1] * mat_other.arr_data[1][1] + arr_data[0][2] * mat_other.arr_data[2][1],
		arr_data[0][0] * mat_other.arr_data[0][2] + arr_data[0][1] * mat_other.arr_data[1][2] + arr_data[0][2] * mat_other.arr_data[2][2],
		arr_data[0][0] * mat_other.arr_data[0][3] + arr_data[0][1] * mat_other.arr_data[1][3] + arr_data[0][2] * mat_other.arr_data[2][3] + arr_data[0][3],

		arr_data[1][0] * mat_other.arr_data[0][0] + arr_data[1][1] * mat_other.arr_data[1][0] + arr_data[1][2] * mat_other.arr_data[2][0],
		arr_data[1][0] * mat_other.arr_data[0][1] + arr_data[1][1] * mat_other.arr_data[1][1] + arr_data[1][2] * mat_other.arr_data[2][1],
		arr_data[1][0] * mat_other.arr_data[0][2] + arr_data[1][1] * mat_other.arr_data[1][2] + arr_data[1][2] * mat_other.arr_data[2][2],
		arr_data[1][0] * mat_other.arr_data[0][3] + arr_data[1][1] * mat_other.arr_data[1][3] + arr_data[1][2] * mat_other.arr_data[2][3] + arr_data[1][3],

		arr_data[2][0] * mat_other.arr_data[0][0] + arr_data[2][1] * mat_other.arr_data[1][0] + arr_data[2][2] * mat_other.arr_data[2][0],
		arr_data[2][0] * mat_other.arr_data[0][1] + arr_data[2][1] * mat_other.arr_data[1][1] + arr_data[2][2] * mat_other.arr_data[2][1],
		arr_data[2][0] * mat_other.arr_data[0][2] + arr_data[2][1] * mat_other.arr_data[1][2] + arr_data[2][2] * mat_other.arr_data[2][2],
		arr_data[2][0] * mat_other.arr_data[0][3] + arr_data[2][1] * mat_other.arr_data[1][3] + arr_data[2][2] * mat_other.arr_data[2][3] + arr_data[2][3]
	};
}