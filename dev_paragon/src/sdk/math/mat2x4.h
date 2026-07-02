#pragma once

#include "vector3d.h"

class matrix2x4_t
{
public:
	vector3d get_origin();
	void set_origin(vector3d vec);

	union
	{
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
		};
	};
};