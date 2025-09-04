
#include <limits>
#include "aabb.h"

AABB::AABB()
    : min(std::numeric_limits<float>::infinity()), 
      max(std::numeric_limits<float>::lowest()) 
{}

AABB::AABB(const Vec3& min_pos, const Vec3& max_pos)
    : min(min_pos),
      max(max_pos) 
{}


void AABB::Expand(const Vec3& point)
{
    min.x = std::min(min.x, point.x);
    min.y = std::min(min.y, point.y);
    min.z = std::min(min.z, point.z);
    max.x = std::max(max.x, point.x);
    max.y = std::max(max.y, point.y);
    max.z = std::max(max.z, point.z);
}

bool AABB::Contains(const Vec3& point) const
{
    return !( point.x < min.x 
        || point.x > max.x 
        || point.y < min.y
        || point.y > max.y
        || point.z < min.z
        || point.z > max.z);
}

bool AABB::Intersects(const AABB& other) const
{
    return !( max.x < other.min.x || min.x > other.max.x
            || max.y < other.min.y || min.y > other.max.y
            || max.z < other.min.z || min.z > other.max.z);
}

Vec3 AABB::Center() const
{
    return (min + max) * 0.5f;
}

Vec3 AABB::Extents() const
{
    return (max - min) * 0.5f;
}

// 복사비용 존재
// 매트릭스와 각 코너 변환시 연산 횟수 줄일 수 있음
/*
AABB AABB::MatrixConversion(const Mat4x4& M) const
{
    Vec3 corners[8] = {
        { min.x, min.y, min.z },
        { min.x, min.y, max.z },
        { min.x, max.y, min.z },
        { min.x, max.y, max.z },
        { max.x, min.y, min.z },
        { max.x, min.y, max.z },
        { max.x, max.y, min.z },
        { max.x, max.y, max.z }
    };

    AABB conversioned;

    for (auto& c : corners)
    {
        Vec4 cc = M * Vec4(c.x, c.y, c.z, 1.0f);
        conversioned.Expand(Vec3(cc.x, cc.y, cc.z));
    }

    return conversioned;
}
*/
AABB AABB::MatrixConversion(const Mat4x4& M) const
{
    Vec3 c = Center();
    Vec3 e = Extents();

    Vec3 wc = (M*Vec4(c.x, c.y, c.z, 1.0f)).ToVec3();
    Vec3 we = {std::fabs(M.m[0][0])*e.x + std::fabs(M.m[0][1])*e.y + std::fabs(M.m[0][2])*e.z
              ,std::fabs(M.m[1][0])*e.x + std::fabs(M.m[1][1])*e.y + std::fabs(M.m[1][2])*e.z
              ,std::fabs(M.m[2][0])*e.x + std::fabs(M.m[2][1])*e.y + std::fabs(M.m[2][2])*e.z};
    
    return AABB{ wc - we, wc + we };
}

AABB AABB::Union(const AABB& a, const AABB& b)
{
    return AABB(Vec3(std::min(a.min.x, b.min.x),
                     std::min(a.min.y, b.min.y),
                     std::min(a.min.z, b.min.z)), 
                Vec3(std::max(a.max.x, b.max.x),
                     std::max(a.max.y, b.max.y),
                     std::max(a.max.z, b.max.z)
                ));
}