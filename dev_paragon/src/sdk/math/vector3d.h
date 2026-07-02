#pragma once
#include <iostream>

#include <cstdint> 
#include <intrin.h>
class vector3d
{
public:
	float x;
	float y;
	float z;

	vector3d();
	vector3d(float x, float y, float z);
	//vector3d(const CMsgQAngle& other);
	vector3d(const vector3d& other);
	vector3d(vector3d&& other) noexcept;
	vector3d(float* other);

	vector3d operator+(const vector3d& other);
	vector3d operator-(const vector3d& other);
	vector3d operator*(const vector3d& other);
	vector3d operator/(const vector3d& other);

	vector3d operator+=(const vector3d& other);
	vector3d operator-=(const vector3d& other);
	vector3d operator*=(const vector3d& other);
	vector3d operator/=(const vector3d& other);

	vector3d operator+(float other);
	vector3d operator-(float other);
	vector3d operator*(float other);
	vector3d operator/(float other);

	vector3d operator+=(float value);
	vector3d operator-=(float value);
	vector3d operator*=(float value);
	vector3d operator/=(float value);

	vector3d operator=(const vector3d& other);
	vector3d operator=(vector3d&& other) noexcept;
	friend std::ostream& operator<<(std::ostream& os, const vector3d& v);

	bool operator==(const vector3d& other);
	bool operator!=(const vector3d& other);
	operator float* () {
		return &x;
	}

	bool is_valid() const;
	vector3d clamp();

	float length_sqr();
	float length();

	float length_2d_sqr();
	float length_2d();

	float dot_product(const vector3d& other, bool additional = false);
	float dot_product(float* other);

	float inversed_dot(const vector3d& other);
	float distance_to(const vector3d& other);

	vector3d cross_product(const vector3d& other);
	float normalize();
	vector3d normalized();
};