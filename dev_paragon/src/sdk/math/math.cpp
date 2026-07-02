#include <algorithm>
#include <random>
#include <DirectXMath.h>

#include "math.h"

namespace math
{
	void fast_sqrt(float a, float* out)
	{
		const auto xx = _mm_load_ss(&a);
		auto xr = _mm_rsqrt_ss(xx);
		auto xt = _mm_mul_ss(xr, xr);
		xt = _mm_mul_ss(xt, xx);
		xt = _mm_sub_ss(_mm_set_ss(3.f), xt);
		xt = _mm_mul_ss(xt, _mm_set_ss(0.5f));
		xr = _mm_mul_ss(xr, xt);
		_mm_store_ss(out, xr);
	}

	float fsqrt(float value)
	{
		float out{};
		fast_sqrt(value, &out);
		return out;
	}

	float rad_to_deg(float radian)
	{
		return radian * RAD_PI;
	}

	float deg_to_rad(float degree)
	{
		return degree * DEG_PI;
	}

	float get_fov(vector3d& current, vector3d& start, vector3d& end)
	{
		vector3d v;
		angle_to_vectors(current, v);
		if ((end - start).normalized().dot_product(v) < .3f)
			return FLT_MAX;

		const auto target = calculate_angles(start, end);
		const auto dist = (end - start).length();
		const auto pitch = sin(deg_to_rad(fabsf(current.x - target.x))) * dist;
		const auto yaw = sin(deg_to_rad(fabsf(current.y - target.y))) * dist;
		return vector3d(pitch, yaw, 0).length();
	}

	float get_fov_simple(vector3d& current, vector3d& start, vector3d& end)
	{
		vector3d current_aim, target = (end - start).normalized();
		angle_to_vectors(current, current_aim);
		return rad_to_deg(acos(target.dot_product(current_aim)));
	}

	void vector_transform(vector3d in1, const matrix3x4_t& in2, vector3d& out)
	{
		out =
		{
			in1.dot_product(vector3d(in2.arr_data[0][0], in2.arr_data[0][1], in2.arr_data[0][2])) + in2.arr_data[0][3],
			in1.dot_product(vector3d(in2.arr_data[1][0], in2.arr_data[1][1], in2.arr_data[1][2])) + in2.arr_data[1][3],
			in1.dot_product(vector3d(in2.arr_data[2][0], in2.arr_data[2][1], in2.arr_data[2][2])) + in2.arr_data[2][3]
		};
	}

	vector3d get_vector_transform(vector3d& in1, const matrix3x4_t& in2)
	{
		return
		{
			in1.dot_product(vector3d(in2.arr_data[0][0], in2.arr_data[0][1], in2.arr_data[0][2])) + in2.arr_data[0][3],
			in1.dot_product(vector3d(in2.arr_data[1][0], in2.arr_data[1][1], in2.arr_data[1][2])) + in2.arr_data[1][3],
			in1.dot_product(vector3d(in2.arr_data[2][0], in2.arr_data[2][1], in2.arr_data[2][2])) + in2.arr_data[2][3]
		};
	}

	void vector_to_angles(vector3d forward, vector3d& angles)
	{
		if (forward.x == 0.f && forward.y == 0.f)
		{
			angles.y = 0.f;
			angles.x = forward.z > 0.f ? 270.f : 90.f;
		}
		else
		{
			angles.y = rad_to_deg(std::atan2(forward.y, forward.x));
			if (angles.y < 0.f)
				angles.y += 360.f;

			angles.x = rad_to_deg(std::atan2(-forward.z, forward.length_2d()));
			if (angles.x < 0.f)
				angles.x += 360.f;
		}

		angles.x = std::remainder(angles.x, 360.f);
		angles.y = std::remainder(angles.y, 360.f);
		angles.z = std::remainder(angles.z, 360.f);
	}

	vector3d cross_product(const vector3d& a, const vector3d& b)
	{
		return vector3d(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
	}

	void vector_to_angles(vector3d& forward, vector3d& up, vector3d& angles)
	{
		vector3d left = cross_product(up, forward);
		left = left.normalized();

		float forward_dist = forward.length_2d();
		if (forward_dist > 0.001f)
		{
			angles.x = atanf(-forward.z, forward_dist) * RAD_PI;
			angles.y = atanf(forward.y, forward.x) * RAD_PI;

			float upz = (left.y * forward.x) - (left.x * forward.y);
			angles.z = atanf(left.z, upz) * RAD_PI;
		}
		else
		{
			angles.x = atanf(-forward.z, forward_dist) * RAD_PI;
			angles.y = atanf(-left.x, left.y) * RAD_PI;
			angles.z = 0;
		}
	}

	void angle_to_vectors(vector3d angles, vector3d& forward)
	{
		float sp{}, sy{}, cp{}, cy{};

		sy = std::sinf(math::deg_to_rad(angles.y));
		cy = std::cosf(math::deg_to_rad(angles.y));

		sp = std::sinf(math::deg_to_rad(angles.x));
		cp = std::cosf(math::deg_to_rad(angles.x));

		forward.x = cp * cy;
		forward.y = cp * sy;
		forward.z = -sp;
	}

	void angle_to_vectors(vector3d angles, vector3d& forward, vector3d& right, vector3d& up)
	{
		float angle;
		static float sp, sy, cp, cy, sr, cr;

		angle = angles.x * (PI / 180.f);
		sp = sin(angle);
		cp = cos(angle);

		angle = angles.y * (PI / 180.f);
		sy = sin(angle);
		cy = cos(angle);

		angle = angles.z * (PI / 180.f);
		sr = sin(angle);
		cr = cos(angle);

		forward.x = cp * cy;
		forward.y = cp * sy;
		forward.z = -sp;

		right.x = -1 * sr * sp * cy + -1 * cr * -sy;
		right.y = -1 * sr * sp * sy + -1 * cr * cy;
		right.z = -1 * sr * cp;

		up.x = cr * sp * cy + -sr * -sy;
		up.y = cr * sp * sy + -sr * cy;
		up.z = cr * cp;
	}

	bool is_near_equal(float v1, float v2, float toler)
	{
		return std::abs(v1 - v2) <= std::abs(toler);
	}

	float angle_diff(float dest, float src)
	{
		float delta = std::fmodf(dest - src, 360.0f);
		if (dest > src)
		{
			if (delta >= 180.f)
				delta -= 360.f;
		}
		else
		{
			if (delta <= -180.f)
				delta += 360.f;
		}
		return delta;
	}

	float interpolate_inversed(float percent, const float& A, const float& B)
	{
		return A + (B - A) * percent;
	}

	float interpolate(const float& from, const float& to, const float& percent)
	{
		return to * percent + from * (1.f - percent);
	}

	vector3d interpolate(vector3d from, vector3d to, const float& percent)
	{
		return to * percent + from * (1.f - percent);
	}

	vector3d calculate_camera_pos(vector3d anchor_pos, float distance, vector3d view_angles)
	{
		float yaw = DirectX::XMConvertToRadians(view_angles.y);
		float pitch = DirectX::XMConvertToRadians(view_angles.x);

		float x = anchor_pos.x + distance * cosf(yaw) * cosf(pitch);
		float y = anchor_pos.y + distance * sinf(yaw) * cosf(pitch);
		float z = anchor_pos.z + distance * sinf(pitch);

		return vector3d{ x, y, z };
	}

	vector3d calculate_angles(vector3d view_pos, vector3d aim_pos)
	{
		static const auto ang_zero = vector3d(0.0f, 0.0f, 0.0f);

		auto delta = view_pos - aim_pos;
		if (delta.length() <= 0.f)
			return ang_zero;

		if (delta.z == 0.f && delta.length() == 0.f)
			return ang_zero;

		if (delta.y == 0.f && delta.x == 0.f)
			return ang_zero;

		vector3d angles;
		angles.x = std::asinf(delta.z / delta.length()) * RAD_PI;
		angles.y = std::atanf(delta.y / delta.x) * RAD_PI;
		angles.z = 0.f;

		if (delta.x >= 0.f)
			angles.y += 180.f;

		return angles;
	}

	void rotate_triangle_points(vector2d points[3], float rotation)
	{
		const auto points_center = (points[0] + points[1] + points[2]) / vector2d(3.f, 3.f);
		for (int i = 0; i < 3; i++)
		{
			vector2d& point = points[i];

			point -= points_center;

			const auto temp_x = point.x;
			const auto temp_y = point.y;

			const auto theta = rotation;
			const auto c = std::cos(theta);
			const auto s = std::sin(theta);

			point.x = temp_x * c - temp_y * s;
			point.y = temp_x * s + temp_y * c;

			point += points_center;
		}
	}

	float smoothstep_bounds(float edge0, float edge1, float x)
	{
		x = std::clamp<float>((x - edge0) / (edge1 - edge0), 0, 1);
		return x * x * (3 - 2 * x);
	}

	float approach_to_angle(float target, float value, float speed)
	{
		float delta = target - value;

		if (delta > speed)
			value += speed;
		else if (delta < -speed)
			value -= speed;
		else
			value = target;

		return value;
	}

	vector3d approach_to_vector(vector3d target, vector3d value, float speed)
	{
		vector3d diff = (target - value);
		float delta = diff.length();

		if (delta > speed)
			value += diff.normalized() * speed;
		else if (delta < -speed)
			value -= diff.normalized() * speed;
		else
			value = target;

		return value;
	}

	float atanf(float y, float x)
	{
		if (x > 0.f)
			return std::atanf(y / x);
		else if (x < 0.f && y >= 0.f)
			return std::atanf(y / x) + PI;
		else if (x < 0.f && y < 0.f)
			return std::atanf(y / x) - PI;
		else if (x == 0.f && y > 0.f)
			return PI / 2.f;
		else if (x == 0.f && y < 0.f)
			return -PI / 2.f;

		return 0;
	}

	float normalize_yaw(float yaw)
	{
		yaw = fmodf(yaw, 360.f);
		if (yaw > 180.f)
			yaw -= 360.f;
		if (yaw < -180.f)
			yaw += 360.f;

		return yaw;
	}

	float random_float(float min, float max)
	{
		//return sdk::random_float(min, max);
		return 0;
	}

	/*matrix3x4_t transform_to_matrix(c_bone_data& in)
	{
		matrix3x4_t matrix{};

		const auto& rot = in.rot;
		const auto& pos = in.pos;

		matrix[0][0] = 1.0f - 2.0f * rot.y * rot.y - 2.0f * rot.z * rot.z;
		matrix[1][0] = 2.0f * rot.x * rot.y + 2.0f * rot.w * rot.z;
		matrix[2][0] = 2.0f * rot.x * rot.z - 2.0f * rot.w * rot.y;

		matrix[0][1] = 2.0f * rot.x * rot.y - 2.0f * rot.w * rot.z;
		matrix[1][1] = 1.0f - 2.0f * rot.x * rot.x - 2.0f * rot.z * rot.z;
		matrix[2][1] = 2.0f * rot.y * rot.z + 2.0f * rot.w * rot.x;

		matrix[0][2] = 2.0f * rot.x * rot.z + 2.0f * rot.w * rot.y;
		matrix[1][2] = 2.0f * rot.y * rot.z - 2.0f * rot.w * rot.x;
		matrix[2][2] = 1.0f - 2.0f * rot.x * rot.x - 2.0f * rot.y * rot.y;

		matrix[0][3] = pos.x;
		matrix[1][3] = pos.y;
		matrix[2][3] = pos.z;

		return matrix;
	}*/

	float delta_angle(float first, float second)
	{
		const float delta = first - second;
		float pi2 = PI * 2.f;
		float res = std::isfinite(delta) ? std::remainder(delta, pi2) : 0.f;

		if (first > second)
		{
			if (res >= PI)
				res -= pi2;
		}
		else
		{
			if (res <= -PI)
				res += pi2;
		}

		return res;
	};

	vector3d normalize(vector3d ang, bool fix_pitch)
	{
		while (ang.y < -180.0f)
			ang.y += 360.0f;
		while (ang.y > 180.0f)
			ang.y -= 360.0f;

		if (fix_pitch)
		{
			if (ang.x > 89.0f)
				ang.x = 89.0f;
			if (ang.x < -89.0f)
				ang.x = -89.0f;
		}

		return ang;
	}

	//float linear_animate(float current, float next, float speed)
	//{
	//	if (next == current)
	//		return current;

	//	float delta = next - current;
	//	float approach = (delta < 0) ? (-speed * ImGui::GetIO().DeltaTime) : (speed * ImGui::GetIO().DeltaTime);

	//	approach = std::clamp(approach, -abs(delta), abs(delta));

	//	return current + approach;
	//}
}