#pragma once

// a = 0.0 완전투명, a = 1.0 완전 불투명
struct Color
{
    float r, g, b, a;

    inline constexpr Color& operator+=(const Color& o) noexcept {
        r += o.r; g += o.g; b += o.b; a += o.a; return *this;
    }
    inline constexpr Color& operator-=(const Color& o) noexcept {
        r -= o.r; g -= o.g; b -= o.b; a -= o.a; return *this;
    }
    inline constexpr Color& operator*=(float s) noexcept {
        r *= s; g *= s; b *= s; a *= s; return *this;
    }
    inline constexpr Color& operator/=(float s) noexcept {
        const float inv = 1.0f / s;
        r *= inv; g *= inv; b *= inv; a *= inv; return *this;
    }

    inline constexpr Color operator+(const Color& o) const noexcept {
        return { r + o.r, g + o.g, b + o.b, a + o.a };
    }
    inline constexpr Color operator-(const Color& o) const noexcept {
        return { r - o.r, g - o.g, b - o.b, a - o.a };
    }
    inline constexpr Color operator*(float s) const noexcept {
        return { r * s, g * s, b * s, a * s };
    }
    inline constexpr Color operator/(float s) const noexcept {
        const float inv = 1.0f / s;
        return { r * inv, g * inv, b * inv, a * inv };
    }
    inline constexpr Color operator*(const Color& o) const noexcept {
        return { r*o.r, g*o.g, b*o.b, a*o.a };
    }

    inline constexpr Color operator-() const noexcept {
        return { -r, -g, -b, -a };
    }
};