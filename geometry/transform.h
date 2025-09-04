#pragma once

#include "math/vector.h"
#include "math/matrix.h"

struct Transform 
{
    Vec3 position = { 0, 0, 0 };
    Vec3 rotation = { 0, 0, 0 };
    Vec3 scale = { 1, 1, 1 };

    mutable Mat4x4 cached_matrix;
    mutable bool dirty = true;

    void SetPosition(const Vec3& p);
    void SetRotation(const Vec3& r);
    void SetScale(const Vec3& s);
    const Vec3& GetPosition() const;
    const Vec3& GetRotation() const;
    const Vec3& GetScale() const;

    Mat4x4 GetMatrix() const;
};