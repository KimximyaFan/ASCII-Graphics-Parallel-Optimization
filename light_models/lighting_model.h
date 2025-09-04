#pragma once

#include "math/vector.h"
#include "light/light.h"
#include <vector>
#include <memory>
#include "material/material.h"
#include "material/color.h"

class Lighting_Model
{
public:
    virtual ~Lighting_Model() = default;

    virtual Color Shade(
        const Vec3& P,
        const Vec3& N,
        const Vec3& V,
        const Material& material,
        const std::vector<std::shared_ptr<Light>>& lights,
        const Color& vertex_color,
        const Vec3& ambient_intensity
    ) const = 0;

protected:
    Lighting_Model() = default;
};