#pragma once

#include "math/vector.h"
#include "math/matrix.h"

class AABB
{
public:
    Vec3 min;
    Vec3 max;

    AABB();
    AABB(const Vec3& min, const Vec3& max);

    void Expand(const Vec3& point);
    bool Contains(const Vec3& point) const;
    bool Intersects(const AABB& other) const;
    Vec3 Center() const;
    Vec3 Extents() const;
    AABB MatrixConversion(const Mat4x4& mat) const;
    static AABB Union(const AABB& a, const AABB& b);
};
