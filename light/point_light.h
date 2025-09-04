#pragma once

#include "light.h"

class Point_Light : public Light
{
public:
    Vec3 position;
    float intensity;
    float constant = 1.0f;

    Point_Light(const Vec3& pos, float intensity)
    : position(pos), intensity(intensity) {}

    Light_Type GetType() const override
    {
        return Light_Type::POINT;
    }

    Vec3 GetDirection(const Vec3& P) const override
    {
        return position - P;
    }

    float GetIntensity(const Vec3& P) const override
    {
        float dist = Vec3::Length(position - P);
        float attenuation = 1.0f / (constant + dist*dist);
        return intensity*attenuation;
    }
};