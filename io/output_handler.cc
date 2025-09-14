
#include "output_handler.h"
#include "shade_mapper.h"
#include <iostream>

Output_Handler::Output_Handler(int w, int h) 
: width(w), height(h), hConsole(GetStdHandle(STD_OUTPUT_HANDLE)) {}

void Output_Handler::PrintBuffer(const std::vector<Color>& frame_buffer, int fps)
{
    char buf[height+2][width];

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            //printf("r=%.2f g=%.2f b=%.2f ", frame_buffer[y * width + x].r, frame_buffer[y * width + x].g, frame_buffer[y * width + x].b );
            buf[y][x] = Shade_Mapper::ColorToChar(frame_buffer[y * width + x]);
        }
        /*
        WriteConsoleOutputCharacterA(
            hConsole,
            buf.data(),
            width,
            {0, (SHORT)y},
            &written
        );
        */
    }
    snprintf(buf[28], width, "FPS: %d", fps);

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    for (int y = 0; y < height; ++y) {
        DWORD written;
        WriteConsoleOutputCharacterA(
            hConsole,
            buf[y],
            width,
            {0, (SHORT)y},
            &written
        );
    }
}