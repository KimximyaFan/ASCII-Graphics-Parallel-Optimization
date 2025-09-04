#pragma once

#include "light.h"

class Directional_Light : public Light
{
public:
    Vec3 position;
    float intensity;

    Directional_Light (const Vec3& pos, const float intensity)
        : position(Vec3::Normalize(pos)), intensity(intensity) {}

    Light_Type GetType () const override
    {
        return Light_Type::DIRECTIONAL;
    }

    Vec3 GetDirection (const Vec3& ) const override
    {
        return position;
    }

    float GetIntensity (const Vec3& ) const override
    {
        return intensity;
    }
};