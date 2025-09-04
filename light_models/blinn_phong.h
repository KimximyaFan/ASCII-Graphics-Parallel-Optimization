#pragma once

#include "lighting_model.h"

class Blinn_Phong : public Lighting_Model
{
public:
    Color Shade(
        const Vec3& P,
        const Vec3& N,
        const Vec3& V,
        const Material& material,
        const std::vector<std::shared_ptr<Light>>& lights,
        const Color& vertex_color,
        const Vec3& ambient_intensity
    ) const override;
};