#include <cmath>
#include <limits>

#include "vector2d.h"

vector2d::vector2d() : x(0.f), y(0.f) {}
vector2d::vector2d(float x, float y) : x(x), y(y) {}

vector2d::vector2d(const vector2d& other) : x(other.x), y(other.y) {}
vector2d::vector2d(vector2d&& other) noexcept : x(other.x), y(other.y) {}

vector2d::vector2d(float* other) : x(other[0]), y(other[1]) {}

vector2d vector2d::operator+(const vector2d& other)
{
	return { x + other.x, y + other.y };
}

vector2d vector2d::operator-(const vector2d& other)
{
	return { x - other.x, y - other.y };
}

vector2d vector2d::operator*(const vector2d& other)
{
	return { x * other.x, y * other.y };
}

vector2d vector2d::operator/(const vector2d& other)
{
	return { x / other.x, y / other.y };
}

vector2d vector2d::operator+=(const vector2d& other)
{
	x += other.x;
	y += other.y;
	return *this;
}

vector2d vector2d::operator-=(const vector2d& other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}

vector2d vector2d::operator*=(const vector2d& other)
{
	x *= other.x;
	y *= other.y;
	return *this;
}

vector2d vector2d::operator/=(const vector2d& other)
{
	x /= other.x;
	y /= other.y;
	return *this;
}

vector2d vector2d::operator+=(float value)
{
	x += value;
	y += value;
	return *this;
}

vector2d vector2d::operator-=(float value)
{
	x -= value;
	y -= value;
	return *this;
}

vector2d vector2d::operator*=(float value)
{
	x *= value;
	y *= value;
	return *this;
}

vector2d vector2d::operator/=(float value)
{
	x /= value;
	y /= value;
	return *this;
}

vector2d vector2d::operator=(const vector2d& other)
{
	x = other.x;
	y = other.y;
	return *this;
}

vector2d vector2d::operator=(vector2d&& other) noexcept
{
	x = other.x;
	y = other.y;
	return *this;
}

bool vector2d::operator==(const vector2d& other)
{
	return x == other.x
		&& y == other.y;
}

bool vector2d::operator!=(const vector2d& other)
{
	return x != other.x
		|| y != other.y;
}

float vector2d::length_sqr()
{
	return x * x + y * y;
}

float vector2d::length()
{
	return std::sqrtf(length_sqr());
}

vector2d vector2d::normalized()
{
	float vector_length = length() + std::numeric_limits<float>::epsilon();

	x /= vector_length;
	y /= vector_length;

	return *this;
}