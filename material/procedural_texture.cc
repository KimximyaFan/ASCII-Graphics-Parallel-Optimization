#include "procedural_texture.h"
#include <cmath>
#include <algorithm>

std::vector<Color> Procedural_Texture::BakeChecker(int W, int H, bool antialias)
{
    if (W <= 0 || H <= 0) return {};
    std::vector<Color> pixels(static_cast<size_t>(W) * static_cast<size_t>(H));

    // 기본값
    constexpr int   tilesX = 8;
    constexpr int   tilesY = 8;
    const Color A{0.92f, 0.92f, 0.92f, 1.0f};
    const Color B{0.01f, 0.01f, 0.01f, 1.0f};

    const float du = antialias ? 0.25f / static_cast<float>(W) : 0.0f;
    const float dv = antialias ? 0.25f / static_cast<float>(H) : 0.0f;

    auto sample = [&](float u, float v) -> Color {
        // [0,1)로 래핑
        u -= std::floor(u);
        v -= std::floor(v);
        int iu = static_cast<int>(std::floor(u * tilesX));
        int iv = static_cast<int>(std::floor(v * tilesY));
        return (((iu ^ iv) & 1) == 0) ? A : B;
    };

    for (int y = 0; y < H; ++y) {
        float v0 = (y + 0.5f) / static_cast<float>(H);
        for (int x = 0; x < W; ++x) {
            float u0 = (x + 0.5f) / static_cast<float>(W);
            Color c;
            if (!antialias) {
                c = sample(u0, v0);
            } else {
                // 2x2 슈퍼샘플
                Color c00 = sample(u0 - du, v0 - dv);
                Color c10 = sample(u0 + du, v0 - dv);
                Color c01 = sample(u0 - du, v0 + dv);
                Color c11 = sample(u0 + du, v0 + dv);
                c = {
                    (c00.r + c10.r + c01.r + c11.r) * 0.25f,
                    (c00.g + c10.g + c01.g + c11.g) * 0.25f,
                    (c00.b + c10.b + c01.b + c11.b) * 0.25f,
                    (c00.a + c10.a + c01.a + c11.a) * 0.25f
                };
            }
            pixels[static_cast<size_t>(y) * W + x] = c;
        }
    }
    return pixels;
}

std::vector<Color> Procedural_Texture::BakeBrick(int W, int H)
{
    if (W <= 0 || H <= 0) return {};
    std::vector<Color> pixels(static_cast<size_t>(W) * static_cast<size_t>(H));

    // 벽돌이 "가로로 눕도록" 셀 가로>세로가 되게 설정 (정사각 텍스처 기준)
    // => bricksY > bricksX 가 되면 셀 가로가 더 길어짐.
    constexpr int   bricksX = 1;   // 가로 칸 수 (적게)
    constexpr int   bricksY = 2;  // 세로 칸 수 (많게)
    constexpr float mortarU = 0.12f; // 가로 줄눈 두께(셀 비율, 0~0.49)
    constexpr float mortarV = 0.12f; // 세로 줄눈 두께(셀 비율, 0~0.49)

    // 밝은 벽돌 / 어두운 줄눈 (단색)
    const Color brickC  {0.8f, 0.8f, 0.8f, 1.0f}; // 밝은 적갈색
    const Color mortarC {0.01f, 0.01f, 0.01f, 1.0f}; // 어두운 콘크리트색

    //const Color mortarC  {0.7f, 0.7f, 0.7f, 1.0f}; // 밝은 적갈색
    //const Color brickC {0.01f, 0.01f, 0.01f, 1.0f}; // 어두운 콘크리트색

    const float mu = std::min(std::max(mortarU, 0.0f), 0.49f);
    const float mv = std::min(std::max(mortarV, 0.0f), 0.49f);

    for (int y = 0; y < H; ++y)
    {
        float v0 = (y + 0.5f) / static_cast<float>(H);
        float v  = v0 - std::floor(v0);          // 0..1
        float sv = v * bricksY;

        int   row   = static_cast<int>(std::floor(sv)); // 0..bricksY-1
        float fv    = sv - static_cast<float>(row);     // 0..1

        for (int x = 0; x < W; ++x)
        {
            float u0 = (x + 0.5f) / static_cast<float>(W);
            float u  = u0 - std::floor(u0);      // 0..1

            // 러닝본드: 홀수 줄 0.5칸 시프트 (타일 이음새 맞도록 fract만 사용)
            float su    = u * bricksX;
            float suOff = su + ((row & 1) ? 0.5f : 0.0f);
            float fu    = suOff - std::floor(suOff);     // 0..1

            // 줄눈/벽돌 판정: 중앙 직사각형만 벽돌, 나머지는 줄눈
            bool isBrick = (fu >= mu && fu <= 1.0f - mu &&
                            fv >= mv && fv <= 1.0f - mv);

            pixels[static_cast<size_t>(y) * W + x] = isBrick ? brickC : mortarC;
        }
    }
    return pixels;
}



