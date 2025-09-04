#pragma once

#include "texture.h"

struct Material {
    float  ambient_coef;    // Ka
    float  diffuse_coef;    // Kd
    float  specular_coef;   // Ks
    float  shininess;  // α

    Texture_Handle texture_handle;
};