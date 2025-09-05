#pragma once

#include "material/color.h"

struct Projected_Vertex
{
    float x, y, z, invW;
    float r_over_w, g_over_w, b_over_w, u_over_w, v_over_w;
};

struct Projected_Triangle
{
    Projected_Vertex v0, v1, v2;
};