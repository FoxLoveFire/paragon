#pragma once
#include "../../math/viewmatrix_t.h"
#include "../../math/vector3d.h"
#include "../../../../external/imgui/imgui.h"

class ViewMatrix {
public:
	bool WorldToScreen(const vector3d& position, vector3d& out) const;

	viewmatrix_t* viewMatrix;

};