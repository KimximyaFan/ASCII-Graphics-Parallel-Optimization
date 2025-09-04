#pragma once

#include "texture.h"
#include <vector>

class Image_Texture : public Texture
{
public:
    Image_Texture(int w, int h, std::vector<Color> vector_pixels);

    const std::vector<Color> GetPixels() const;
    Sampler_Desc GetSampler() const;
    void SetSampler(const Sampler_Desc& s);
    
    Color Sample(float u, float v) const override;

private:
    std::vector<Color> pixels;
    Sampler_Desc sampler;
    
    Color Texel(int x, int y) const;
};