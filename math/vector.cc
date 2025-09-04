#include "vector.h"


Vec2::Vec2(float x, float y) : x(x), y(y) {}

Vec2 Vec2::operator+(const Vec2& other) const
{
    return Vec2(x + other.x, y + other.y);
}

Vec2& Vec2::operator+=(const Vec2& other)
{
    x += other.x;
    y += other.y;
    return *this;
}

Vec2 Vec2::operator-(const Vec2& other) const
{
    return Vec2(x - other.x, y - other.y);
}

Vec2& Vec2::operator-=(const Vec2& other)
{
    x -= other.x;
    y -= other.y;
    return *this;
}

Vec2 Vec2::operator*(float scalar) const
{
    return Vec2(x * scalar, y * scalar);
}

Vec2& Vec2::operator*=(float scalar)
{
    x *= scalar;
    y *= scalar;
    return *this;
}

Vec2 Vec2::operator/(float scalar) const
{
    return Vec2(x / scalar, y / scalar);
}

Vec2& Vec2::operator/=(float scalar)
{
    x /= scalar;
    y /= scalar;
    return *this;
}

bool Vec2::operator==(const Vec2& other) const
{
    return x == other.x && y == other.y;
}

float Vec2::Dot(const Vec2& a, const Vec2& b)
{
    return a.x * b.x + a.y * b.y;
}

float Vec2::Cross(const Vec2& a, const Vec2& b)
{
    // Returns the z-component of the 3D cross product
    return a.x * b.y - a.y * b.x;
}

float Vec2::Length() const
{
    return std::sqrt(x * x + y * y);
}

Vec2 Vec2::Normalize() const
{
    float len = Length();
    return len != 0 ? *this / len : Vec2(0, 0);
}

Vec2 Vec2::Normalize(const Vec2& a)
{
    float len = a.Length();
    return len != 0 ? Vec2(a.x / len, a.y / len) : Vec2(0, 0);
}

Vec3::Vec3(float a) : x(a), y(a), z(a) {}

Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

Vec3::Vec3(const Vec3& other) : x(other.x), y(other.y), z(other.z) {}

Vec3& Vec3::operator=(const Vec3& other)
{
    if ( this != &other )
    {
        x = other.x;
        y = other.y;
        z = other.z;
    }
    return *this;
}

Vec3 Vec3::operator+(const Vec3& other) const
{
    return Vec3(x + other.x, y + other.y, z + other.z);
}

Vec3& Vec3::operator+=(const Vec3& other)
{
    x += other.x; y += other.y; z += other.z;
    return *this;
}

Vec3 Vec3::operator-(const Vec3& other) const
{
    return Vec3(x - other.x, y - other.y, z - other.z);
}

Vec3& Vec3::operator-=(const Vec3& other)
{
    x -= other.x; y -= other.y; z -= other.z;
    return *this;
}

Vec3 Vec3::operator-() const
{
    return Vec3(-x, -y, -z);
}

Vec3 Vec3::operator*(float scalar) const
{
    return Vec3(x * scalar, y * scalar, z * scalar);
}

Vec3& Vec3::operator*=(float scalar)
{
    x *= scalar; y *= scalar; z *= scalar;
    return *this;
}

Vec3 Vec3::operator/(float scalar) const
{
    return Vec3(x / scalar, y / scalar, z / scalar);
}

Vec3& Vec3::operator/=(float scalar)
{
    x /= scalar; y /= scalar; z /= scalar;
    return *this;
}

bool Vec3::operator==(const Vec3& other) const
{
    return x == other.x && y == other.y && z == other.z;
}

float Vec3::Dot(const Vec3& a, const Vec3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 Vec3::Cross(const Vec3& a, const Vec3& b)
{
    return Vec3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

float Vec3::Length() const
{
    return std::sqrt(x * x + y * y + z * z);
}

Vec3 Vec3::Normalize() const
{
    float len = Length();
    return (len > 1e-6f) ? *this / len : Vec3(0, 0, 0);
}

Vec3 Vec3::Normalize(const Vec3& a)
{
    float len = a.Length();
    return (len > 1e-6f) ? a / len : Vec3(0, 0, 0);
}

float Vec3::Length(const Vec3& a)
{
    return std::sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

// =====================
// Vec4
// =====================

Vec4::Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

Vec4 Vec4::operator+(const Vec4& other) const
{
    return Vec4(x + other.x, y + other.y, z + other.z, w + other.w);
}

Vec4& Vec4::operator+=(const Vec4& other)
{
    x += other.x; y += other.y; z += other.z; w += other.w;
    return *this;
}

Vec4 Vec4::operator-(const Vec4& other) const
{
    return Vec4(x - other.x, y - other.y, z - other.z, w - other.w);
}

Vec4& Vec4::operator-=(const Vec4& other)
{
    x -= other.x; y -= other.y; z -= other.z; w -= other.w;
    return *this;
}

Vec4 Vec4::operator*(float scalar) const
{
    return Vec4(x * scalar, y * scalar, z * scalar, w * scalar);
}

Vec4& Vec4::operator*=(float scalar)
{
    x *= scalar; y *= scalar; z *= scalar; w *= scalar;
    return *this;
}

Vec4 Vec4::operator/(float scalar) const
{
    return Vec4(x / scalar, y / scalar, z / scalar, w / scalar);
}

Vec4& Vec4::operator/=(float scalar)
{
    x /= scalar; y /= scalar; z /= scalar; w /= scalar;
    return *this;
}

bool Vec4::operator==(const Vec4& other) const
{
    return x == other.x && y == other.y && z == other.z && w == other.w;
}

float Vec4::Dot(const Vec4& a, const Vec4& b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

float Vec4::Length() const
{
    return std::sqrt(x * x + y * y + z * z + w * w);
}

Vec4 Vec4::Normalize() const
{
    float len = Length();
    return len == 0 ? Vec4(0, 0, 0, 0) : *this / len;
}

Vec4 Vec4::Normalize(const Vec4& a)
{
    float len = a.Length();
    return len == 0 ? Vec4(0, 0, 0, 0) : a / len;
}

Vec3 Vec4::ToVec3() const
{
    return Vec3(x, y, z);
}