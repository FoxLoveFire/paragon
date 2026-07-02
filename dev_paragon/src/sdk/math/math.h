#pragma once

#include "vector3d.h"
#include "vector2d.h"
#include "vector4d.h"
#include "color.h"
#include "mat3x4.h"
#include "mat2x4.h"
#include "mat4x4.h"

#include <xmmintrin.h>
#include <emmintrin.h>

constexpr auto PI = 3.14159265358979323846f;
constexpr auto RAD_PI = 180.f / PI;
constexpr auto DEG_PI = PI / 180.f;
constexpr auto PITCH_BOUNDS = 89.f;
constexpr auto YAW_BOUNDS = 360.f;

#define TIME_TO_TICKS(TIME) (static_cast<int>(0.5f + TIME / sdk::global_vars->interval_per_tick))
#define TICKS_TO_TIME(TICKS) (sdk::global_vars->interval_per_tick * static_cast<float>(TICKS))

struct c_bone_data;

namespace math
{
	void fast_sqrt(float a, float* out);

	float rad_to_deg(float radian);
	float deg_to_rad(float degree);

	float get_fov(vector3d& current, vector3d& start, vector3d& end);
	float get_fov_simple(vector3d& current, vector3d& start, vector3d& end);

	vector3d get_vector_transform(vector3d& in1, const matrix3x4_t& in2);
	void vector_transform(vector3d in1, const matrix3x4_t& in2, vector3d& out);

	void vector_to_angles(vector3d forward, vector3d& angles);
	void vector_to_angles(vector3d& forward, vector3d& up, vector3d& angles);
	void angle_to_vectors(vector3d angles, vector3d& forward);
	void angle_to_vectors(vector3d angles, vector3d& forward, vector3d& right, vector3d& up);

	bool is_near_equal(float v1, float v2, float toler);

	float angle_diff(float dest, float src);

	float interpolate_inversed(float percent, const float& A, const float& B);
	float interpolate(const float& from, const float& to, const float& percent);
	vector3d calculate_camera_pos(vector3d anchor_pos, float distance, vector3d view_angles);
	vector3d calculate_angles(vector3d view_pos, vector3d aim_pos);

	void rotate_triangle_points(vector2d points[3], float rotation);

	float smoothstep_bounds(float edge0, float edge1, float x);
	float approach_to_angle(float target, float value, float speed);
	vector3d approach_to_vector(vector3d target, vector3d value, float speed);

	float atanf(float y, float x);
	float normalize_yaw(float yaw);
	float random_float(float min, float max);

	//matrix3x4_t transform_to_matrix(c_bone_data& in);
	float delta_angle(float first, float second);
	vector3d normalize(vector3d ang, bool fix_pitch);
	//float linear_animate(float current, float next, float speed = 10.f);
}