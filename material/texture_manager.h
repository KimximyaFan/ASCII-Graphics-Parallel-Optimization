#pragma once

#include "texture.h"
#include <memory>
#include <vector>
#include <array>

class Texture_Manager
{
public:
    Texture_Manager () = default;
    ~Texture_Manager () = default;
    
    bool RegisterImage(Texture_Handle handle, const char* path, const Sampler_Desc sampler);
    bool RegisterProcedural(Texture_Handle handle, const std::vector<Color> pixels, int w, int h, const Sampler_Desc sampler);

    const Texture* GetTexture(Texture_Handle handle) const;

private:
    std::array < std::unique_ptr<Texture>, static_cast<size_t>(Texture_Handle::COUNT) > registered_textures;

    static constexpr size_t ToIndex(Texture_Handle handle) noexcept { return static_cast<size_t>(handle); }
    static bool LoadImageFile(const char* path, int& outW, int& outH, std::vector<Color>& outPixels);
    
    bool Is_Exist(Texture_Handle handle) const;
};