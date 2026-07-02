#include "mat3x4.h"
#include "vector4d.h"

matrix3x4_t quaternion::to_matrix(const vector3d& origin)
{
	matrix3x4_t out;

#ifdef _DEBUG
	const float x2 = this->x + this->x, y2 = this->y + this->y, z2 = this->z + this->z;
	const float xx = this->x * x2, xy = this->x * y2, xz = this->x * z2;
	const float yy = this->y * y2, yz = this->y * z2;
	const float zz = this->z * z2;
	const float wx = this->w * x2, wy = this->w * y2, wz = this->w * z2;

	out[0][0] = 1.0f - (yy + zz);
	out[1][0] = xy + wz;
	out[2][0] = xz - wy;

	out[0][1] = xy - wz;
	out[1][1] = 1.0f - (xx + zz);
	out[2][1] = yz + wx;

	out[0][2] = xz + wy;
	out[1][2] = yz - wx;
	out[2][2] = 1.0f - (xx + yy);
#else
	out[0][0] = 1.0f - 2.0f * this->y * this->y - 2.0f * this->z * this->z;
	out[1][0] = 2.0f * this->x * this->y + 2.0f * this->w * this->z;
	out[2][0] = 2.0f * this->x * this->z - 2.0f * this->w * this->y;

	out[0][1] = 2.0f * this->x * this->y - 2.0f * this->w * this->z;
	out[1][1] = 1.0f - 2.0f * this->x * this->x - 2.0f * this->z * this->z;
	out[2][1] = 2.0f * this->y * this->z + 2.0f * this->w * this->x;

	out[0][2] = 2.0f * this->x * this->z + 2.0f * this->w * this->y;
	out[1][2] = 2.0f * this->y * this->z - 2.0f * this->w * this->x;
	out[2][2] = 1.0f - 2.0f * this->x * this->x - 2.0f * this->y * this->y;
#endif

	out[0][3] = origin.x;
	out[1][3] = origin.y;
	out[2][3] = origin.z;
	return out;
}