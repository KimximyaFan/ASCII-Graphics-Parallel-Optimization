#pragma once

#include "math/vector.h"
#include "material/color.h"

struct Vertex 
{
    Vec4 position;
    Vec3 normal;
    Vec2 uv;
    Color color;
};