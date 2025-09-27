#pragma once

#include "material/color.h"

class Shade_Mapper
{
public:

    static inline constexpr float Inv3 = 1.0f / 3.0f;

    static inline constexpr char char_map[6] = { ' ', '\'', '*', '+', '#', '@' };

    static inline char FastColorToChar(const Color& color)
    {
        float value = (color.r + color.g + color.b) * Inv3;
        unsigned int index = 0;
        index += (value >= 0.05f);
        index += (value >= 0.20f);
        index += (value >= 0.40f);
        index += (value >= 0.60f);
        index += (value >= 0.80f);
        return char_map[index];
    }


    static char ColorToChar(const Color& color)
    {
        float value = (color.r + color.g + color.b) / 3.0f;

        if ( value < 0.05f )
            return ' ';
        else if ( value < 0.2f )
            return '\'';
        else if ( value < 0.4f )
            return '*';
        else if ( value < 0.6f )
            return '+';
        else if ( value < 0.8f )
            return '#';
        else
            return '@';
    }
};