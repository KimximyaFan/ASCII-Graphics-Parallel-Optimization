#pragma once

#include "math/vector.h"
#include "material/color.h"

enum class Light_Type
{
    DIRECTIONAL,
    POINT
};

class Light
{
public:
    virtual ~Light() = default;

    virtual Light_Type GetType() const = 0;

    virtual Vec3 GetDirection(const Vec3& P) const = 0;

    virtual float GetIntensity(const Vec3& P) const = 0;

    int GetId() const { return id; }
protected:
    Light() : id(next_id++) {}

private:
    int id;
    static inline int next_id = 0;
};