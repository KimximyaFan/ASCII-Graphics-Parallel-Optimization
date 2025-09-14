#pragma once

#include "material/color.h"
#include <vector>
#include <windows.h>

class Output_Handler
{
public:
    Output_Handler(int w, int h);
    
    void PrintBuffer(const std::vector<Color>& frame_buffer, int fps);

private:
    int width;
    int height;
    HANDLE hConsole;
    DWORD written;
};