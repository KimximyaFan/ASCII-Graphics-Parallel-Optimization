#pragma once

#include <cmath>

class Vec2
{
public:
    float x, y;

    Vec2(float x = 0, float y = 0);

    Vec2 operator+(const Vec2& other) const;
    Vec2& operator+=(const Vec2& other);

    Vec2 operator-(const Vec2& other) const;
    Vec2& operator-=(const Vec2& other);

    Vec2 operator*(float scalar) const;
    Vec2& operator*=(float scalar);

    Vec2 operator/(float scalar) const;
    Vec2& operator/=(float scalar);

    bool operator==(const Vec2& other) const;

    float Length() const;

    Vec2 Normalize() const;

    static float Dot(const Vec2& a, const Vec2& b);

    static float Cross(const Vec2& a, const Vec2& b);

    static Vec2 Normalize(const Vec2& a);
};


class Vec3
{
public:
    float x, y, z;

    Vec3 () {};
    Vec3(float a);
    Vec3(float x, float y, float z);
    Vec3(const Vec3& v);

    Vec3& operator=(const Vec3& );

    Vec3 operator+(const Vec3& other) const;
    Vec3& operator+=(const Vec3& other);

    Vec3 operator-(const Vec3& other) const;
    Vec3& operator-=(const Vec3& other);

    Vec3 operator-() const;

    Vec3 operator*(float scalar) const;
    Vec3& operator*=(float scalar);

    Vec3 operator/(float scalar) const;
    Vec3& operator/=(float scalar);

    bool operator==(const Vec3& other) const;

    float Length() const;

    Vec3 Normalize() const;

    static float Dot(const Vec3& a, const Vec3& b);

    static Vec3 Cross(const Vec3& a, const Vec3& b);

    static Vec3 Normalize(const Vec3& a);

    static float Length(const Vec3& a);
};

class Vec4
{
public:
    float x, y, z, w;

    Vec4(float x = 0, float y = 0, float z = 0, float w = 0);

    Vec4 operator+(const Vec4& other) const;
    Vec4& operator+=(const Vec4& other);

    Vec4 operator-(const Vec4& other) const;
    Vec4& operator-=(const Vec4& other);

    Vec4 operator*(float scalar) const;
    Vec4& operator*=(float scalar);

    Vec4 operator/(float scalar) const;
    Vec4& operator/=(float scalar);

    bool operator==(const Vec4& other) const;

    float Length() const;

    Vec4 Normalize() const;

    Vec3 ToVec3() const;

    static float Dot(const Vec4& a, const Vec4& b);

    static Vec4 Normalize(const Vec4& a);
};
