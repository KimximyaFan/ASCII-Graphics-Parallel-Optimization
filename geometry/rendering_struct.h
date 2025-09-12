#pragma once

#include "material/color.h"
#include "material/texture.h"

struct Projected_Vertex
{
    float x, y, z, invW;
    float r_over_w, g_over_w, b_over_w, u_over_w, v_over_w;
};

struct Projected_Triangle
{
    Projected_Vertex v0, v1, v2;
    const Texture* texture;
};

struct Triangle_Reference
{
    int tid, index;
    int x0, y0, x1, y1;
};

struct Bounding_Box
{
    int x0, y0, x1, y1;
};

struct Tile
{
    int x0, y0, x1, y1;
    int id;
};