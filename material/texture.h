#pragma once
#include <cstdint>
//#include <vector>
//#include <functional>
//#include <memory>
//#include <cassert>
#include "color.h"

enum class Texture_Handle : uint8_t
{
    CHECKER,
    BRICK,
    COUNT
};

/*
Wrap Mode
텍스처 좌표 (u, v)가 0.0 ~ 1.0 범위를 벗어날 때 어떻게 처리할지

Filter Mode
텍스처 샘플링할 때, 텍셀을 어떻게 보간(interpolation)해서 색을 정할지 결정하는 모드

Nearest Neighbor 보간의 원리
(u, v) 좌표에 가장 가까운 단일 texel 하나만 선택
즉, "주변 픽셀 중 하나"만 사용 → 보간 없음
수학적으로는 round() 또는 floor+0.5 같은 방식으로 가장 가까운 정수 좌표를 찾음

Linear 보간
텍스처 좌표 (u, v)는 보통 정규화된 실수 (0~1).
→ 실제 텍셀 인덱스 (x, y)로 바꾸면 정수가 아니고, 픽셀 사이 어딘가에 위치
→ 이때 가장 가까운 4개의 텍셀을 찾아서, (u, v)의 위치에 따라 가중 평균을 계산

*/

enum class Wrap_Mode : uint8_t
{
    REPEAT,
    CLAMP
};

enum class Filter_Mode : uint8_t
{
    NEAREST,
    LINEAR
};

enum class Texture_Type : uint8_t
{
    IMAGE,
    PROCEDURAL
};

struct Sampler_Desc
{
    Wrap_Mode wrap_u;
    Wrap_Mode wrap_v;
    Filter_Mode filter;
};

class Texture
{
public:
    virtual ~Texture() = default;

    Texture_Type GetType() const { return texture_type; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }

    virtual Color Sample(float u, float v) const = 0;
protected:
    Texture (Texture_Type type, int w, int h)
        : texture_type(type), width(w), height(h) {}

    Texture_Type texture_type;
    int width = 0;
    int height = 0;
};