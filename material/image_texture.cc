#include "image_texture.h"
#include <algorithm>
#include <cmath>

Image_Texture::Image_Texture(int w, int h, std::vector<Color> vector_pixels)
    : Texture(Texture_Type::IMAGE, w, h), pixels(std::move(vector_pixels)) {}

const std::vector<Color> Image_Texture::GetPixels() const
{
    return pixels;
}

Color Image_Texture::Texel(int x, int y) const
{
    return pixels[width*y + x];
}

Sampler_Desc Image_Texture::GetSampler() const
{
    return sampler;
}

void Image_Texture::SetSampler(const Sampler_Desc& s)
{
    sampler = s;
}

Color Image_Texture::Sample(float u, float v) const
{
    auto wrap = [](float t, Wrap_Mode m)
    { 
        return m == Wrap_Mode::REPEAT ? t - std::floor(t) : std::clamp(t, 0.0f, 1.0f);
    };

    u = wrap(u, sampler.wrap_u);
    v = wrap(v, sampler.wrap_v);
    
    float fx = u * (width-1);
    float fy = v * (height-1);

    if ( sampler.filter == Filter_Mode::NEAREST )
        return Texel((int)std::round(fx), (int)std::round(fy));
    
    int x0 = (int)fx;
    int y0 = (int)fy;
    int x1 = x0 + (x0 < width-1);
    int y1 = y0 + (y0 < height-1);

    float tx = fx - (float)x0;
    float ty = fy - (float)y0;

    const Color c00 = Texel(x0, y0);
    const Color c10 = Texel(x1, y0);
    const Color c01 = Texel(x0, y1);
    const Color c11 = Texel(x1, y1);

    // c0 = c00 + (c10 - c00) * tx
    // c1 = c01 + (c11 - c01) * tx
    // out = c0 + (c1 - c0) * ty

    //Color out;
    /*
    out.r = (c00.r + (c10.r - c00.r) * tx) + ((c01.r + (c11.r - c01.r) * tx) - (c00.r + (c10.r - c00.r) * tx)) * ty;
    out.g = (c00.g + (c10.g - c00.g) * tx) + ((c01.g + (c11.g - c01.g) * tx) - (c00.g + (c10.g - c00.g) * tx)) * ty;
    out.b = (c00.b + (c10.b - c00.b) * tx) + ((c01.b + (c11.b - c01.b) * tx) - (c00.b + (c10.b - c00.b) * tx)) * ty;
    out.a = (c00.a + (c10.a - c00.a) * tx) + ((c01.a + (c11.a - c01.a) * tx) - (c00.a + (c10.a - c00.a) * tx)) * ty;
    */

    auto lerp = [](float a, float b, float t) { return a + (b - a) * t; };

    Color c0 { lerp(c00.r, c10.r, tx), lerp(c00.g, c10.g, tx),
            lerp(c00.b, c10.b, tx), lerp(c00.a, c10.a, tx) };
    Color c1 { lerp(c01.r, c11.r, tx), lerp(c01.g, c11.g, tx),
            lerp(c01.b, c11.b, tx), lerp(c01.a, c11.a, tx) };

    Color out { lerp(c0.r, c1.r, ty), lerp(c0.g, c1.g, ty),
                lerp(c0.b, c1.b, ty), lerp(c0.a, c1.a, ty) };
    return out;
}