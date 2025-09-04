#pragma once

#include <vector>
#include "texture.h"

class Procedural_Texture : public Texture
{
public:
    Color Sample(float u, float v) const override { return {}; }

    static std::vector<Color> BakeChecker(int W, int H, bool antialias = false);

    static std::vector<Color> BakeBrick(int W, int H);
};