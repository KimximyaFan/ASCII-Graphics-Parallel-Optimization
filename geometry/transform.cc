
#include <cmath>
#include "transform.h"

const Vec3& Transform::GetPosition() const
{
    return position;
}

const Vec3& Transform::GetRotation() const
{
    return rotation;
}

const Vec3& Transform::GetScale() const
{
    return scale;
}

void Transform::SetPosition(const Vec3& p)
{
    position = p;
    dirty = true;
}

void Transform::SetRotation(const Vec3& r)
{
    rotation = r;
    dirty = true;
}

void Transform::SetScale(const Vec3& s)
{
    scale = s;
    dirty = true;
}

// [3][0] ~ [3][2] 는 생략 가능해보임
Mat4x4 Transform::GetMatrix() const
{
    if ( dirty == true )
    {
        float cos_x = std::cos(rotation.x), sin_x = std::sin(rotation.x);
        float cos_y = std::cos(rotation.y), sin_y = std::sin(rotation.y);
        float cos_z = std::cos(rotation.z), sin_z = std::sin(rotation.z);
        float Sx = scale.x; float Sy = scale.y; float Sz = scale.z;

        cached_matrix.m[0][0] = cos_z*cos_y * Sx;
        cached_matrix.m[0][1] = (cos_z*sin_y*sin_x - sin_z*cos_x) * Sy;
        cached_matrix.m[0][2] = (cos_z*sin_y*cos_x + sin_z*sin_x) * Sz;
        cached_matrix.m[0][3] = position.x;
        cached_matrix.m[1][0] = sin_z*cos_y * Sx;
        cached_matrix.m[1][1] = (sin_z*sin_y*sin_x + cos_z*cos_x) * Sy;
        cached_matrix.m[1][2] = (sin_z*sin_y*cos_x - cos_z*sin_x) * Sz;
        cached_matrix.m[1][3] = position.y;
        cached_matrix.m[2][0] = -sin_y * Sx;
        cached_matrix.m[2][1] = cos_y*sin_x * Sy;
        cached_matrix.m[2][2] = cos_y*cos_x * Sz;
        cached_matrix.m[2][3] = position.z;
        cached_matrix.m[3][0] = 0.0f;
        cached_matrix.m[3][1] = 0.0f;
        cached_matrix.m[3][2] = 0.0f;
        cached_matrix.m[3][3] = 1.0f;

        dirty = false;
    }
    return cached_matrix;
}