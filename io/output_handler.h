#pragma once

#include "material/color.h"
#include <vector>
#include <windows.h>
#include <string>
#include <cstdio>
#include <cstdint>

enum class Print_Type : uint8_t
{
    NORMAL,
    FAST,
    VT
};

class Output_Handler
{
public:
    Output_Handler(int w, int h, Print_Type print_type);
    
    void EnableVT();
    void PrintBufferNormal(const std::vector<Color>& frame_buffer, int fps);
    void PrintBufferFast(const std::vector<Color>& frame_buffer, int fps);
    void PrintBufferVT(const std::vector<Color>& frame_buffer, int fps);
    void PrintBuffer(const std::vector<Color>& frame_buffer, int fps);
    void PrintClear();
    
private:
    int width;
    int height;
    std::string out_str;
    std::vector<CHAR_INFO> char_info;
    HANDLE hConsole;
    DWORD written;
    Print_Type print_type;
};