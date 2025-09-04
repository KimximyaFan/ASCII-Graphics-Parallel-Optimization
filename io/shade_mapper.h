#pragma once

#include "material/color.h"

class Shade_Mapper
{
public:
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