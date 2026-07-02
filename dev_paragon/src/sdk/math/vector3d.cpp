#include <cmath>
#include <limits>
#include "vector3d.h"
#include <algorithm>
#include <iostream>

vector3d::vector3d() : x(0.f), y(0.f), z(0.f) {}
vector3d::vector3d(float x, float y, float z) : x(x), y(y), z(z) {}

//vector3d::vector3d(const CMsgQAngle& other) : x(other.x()), y(other.y()), z(other.z()) {}

vector3d::vector3d(const vector3d& other) : x(other.x), y(other.y), z(other.z) {}
vector3d::vector3d(vector3d&& other) noexcept : x(other.x), y(other.y), z(other.z) {}

vector3d::vector3d(float* other) : x(other[0]), y(other[1]), z(other[2]) {}

vector3d vector3d::operator+(const vector3d& other)
{
	return { x + other.x, y + other.y, z + other.z };
}

vector3d vector3d::operator-(const vector3d& other)
{
	return { x - other.x, y - other.y, z - other.z };
}

vector3d vector3d::operator*(const vector3d& other)
{
	return { x * other.x, y * other.y, z * other.z };
}

vector3d vector3d::operator/(const vector3d& other)
{
	return { x / other.x, y / other.y, z / other.z };
}

vector3d vector3d::operator+=(const vector3d& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

vector3d vector3d::operator-=(const vector3d& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}

vector3d vector3d::operator*=(const vector3d& other)
{
	x *= other.x;
	y *= other.y;
	z *= other.z;
	return *this;
}

vector3d vector3d::operator/=(const vector3d& other)
{
	x /= other.x;
	y /= other.y;
	z /= other.z;
	return *this;
}

vector3d vector3d::operator+=(float value)
{
	x += value;
	y += value;
	z += value;
	return *this;
}

vector3d vector3d::operator-=(float value)
{
	x -= value;
	y -= value;
	z -= value;
	return *this;
}

vector3d vector3d::operator*=(float value)
{
	x *= value;
	y *= value;
	z *= value;
	return *this;
}

vector3d vector3d::operator/=(float value)
{
	x /= value;
	y /= value;
	z /= value;
	return *this;
}

vector3d vector3d::operator+(float value)
{
	x += value;
	y += value;
	z += value;
	return *this;
}

vector3d vector3d::operator-(float value)
{
	x -= value;
	y -= value;
	z -= value;
	return *this;
}

vector3d vector3d::operator*(float value)
{
	return vector3d(x * value, y * value, z * value);
}

vector3d vector3d::operator/(float value)
{
	x /= value;
	y /= value;
	z /= value;
	return *this;
}

vector3d vector3d::operator=(const vector3d& other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	return *this;
}

vector3d vector3d::operator=(vector3d&& other) noexcept
{
	x = other.x;
	y = other.y;
	z = other.z;
	return *this;
}

bool vector3d::operator==(const vector3d& other)
{
	return x == other.x
		&& y == other.y
		&& z == other.z;
}

bool vector3d::operator!=(const vector3d& other)
{
	return x != other.x
		|| y != other.y
		|| z != other.z;
}

bool vector3d::is_valid() const
{
	return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
}

vector3d vector3d::clamp()
{
	x = std::clamp(x, -89.0f, 89.0f);
	y = std::clamp(std::remainder(y, 360.0f), -180.0f, 180.0f);
	z = 0.f;

	return *this;
}

float vector3d::length_sqr()
{
	return x * x + y * y + z * z;
}

float vector3d::length()
{
	return std::sqrtf(length_sqr());
}

float vector3d::length_2d_sqr()
{
	return x * x + y * y;
}

float vector3d::length_2d()
{
	return std::sqrtf(length_2d_sqr());
}

float vector3d::dot_product(const vector3d& other, bool additional)
{
	if (additional)
		return x * other.y + y * other.x + z * other.z;

	return x * other.x + y * other.y + z * other.z;
}

float vector3d::dot_product(float* other)
{
	return x * other[0] + y * other[1] + z * other[2];
}

float vector3d::inversed_dot(const vector3d& other)
{
	return x * other.y + y * other.x + z * other.z;
}

vector3d vector3d::cross_product(const vector3d& other)
{
	return
	{
		y * other.z - z * other.y,
		z * other.x - x * other.z,
		x * other.y - y * other.x
	};
}

float vector3d::normalize()
{
	const float len = this->length();
	const float radius = 1.0f / (len + std::numeric_limits<float>::epsilon());

	this->x *= radius;
	this->y *= radius;
	this->z *= radius;

	return len;

	/*const auto len = length();
	if (len)
	{
		x /= len;
		y /= len;
		z /= len;
	}
	return len;*/
}

vector3d vector3d::normalized()
{
	vector3d out = *this;
	out.normalize();
	return out;
}

float vector3d::distance_to(const vector3d& other)
{
	return (*this - other).length();
}

std::ostream& operator<<(std::ostream& os, const vector3d& v)
{
	os << "X: " << v.x << " Y: " << v.y << " Z: " << v.z;

	return os;
}