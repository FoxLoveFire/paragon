#include "mat2x4.h"

void matrix2x4_t::set_origin(vector3d vec)
{
	_11 = vec.x;
	_12 = vec.y;
	_13 = vec.z;
}

vector3d matrix2x4_t::get_origin()
{
	return vector3d(_11, _12, _13);
}