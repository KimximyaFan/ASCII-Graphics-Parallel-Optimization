
#include "output_handler.h"
#include "shade_mapper.h"
#include <iostream>

Output_Handler::Output_Handler(int w, int h, Print_Type type) 
: width(w), height(h), hConsole(GetStdHandle(STD_OUTPUT_HANDLE)), char_info(width*height), print_type(type)
{
    out_str.reserve((width + 1) * height + 64);

    if ( print_type == Print_Type::VT )
        EnableVT();
}

void Output_Handler::PrintBufferNormal(const std::vector<Color>& frame_buffer, int fps)
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

void Output_Handler::EnableVT()
{
    DWORD mode = 0;
    GetConsoleMode(hConsole, &mode);
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hConsole, mode);
    WriteFile(hConsole, "\x1b[?25l", 6, &written, nullptr);
}
/*
void Output_Handler::PrintBufferFast(const std::vector<Color>& frame_buffer, int fps)
{
    out_str.clear();
    out_str.append("\x1b[H");

    for (int y=0; y<height; ++y)
    {
        int row = y * width;
        
        for (int x=0; x<width; ++x)
            out_str.push_back(Shade_Mapper::FastColorToChar(frame_buffer[row + x]));

        if (y + 1 < height)
            out_str.append("\r\n");
    }

    char hud[64];
    int n = std::snprintf(hud, sizeof(hud), "\x1b[%d;%dHFPS: %d", height, 1, fps);
    out_str.append(hud, n);

    WriteFile(hConsole, out_str.data(), (DWORD)out_str.size(), &written, nullptr);
}
*/
void Output_Handler::PrintBufferVT(const std::vector<Color>& frame_buffer, int fps)
{
    out_str.clear();
    out_str.append("\x1b[H");

    const Color* fb = frame_buffer.data();
    const int N = width * height;
    int col = 0;

    for (int i = 0; i < N; ++i)
    {
        out_str.push_back(Shade_Mapper::FastColorToChar(fb[i]));

        if (++col == width)
        {
            col = 0;

            if (i + 1 < N)
                out_str.append("\r\n");
        }
    }

    char hud[64];
    int n = std::snprintf(hud, sizeof(hud), "\x1b[%d;%dHFPS: %d", height, 1, fps);
    out_str.append(hud, n);

    WriteFile(hConsole, out_str.data(), (DWORD)out_str.size(), &written, nullptr);
}


void Output_Handler::PrintBufferFast(const std::vector<Color>& frame_buffer, int fps)
{
    const WORD baseAttr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    const int N = width * height;

    for (int i=0; i<N; ++i)
    {
        CHAR_INFO& ci = char_info[i];
        ci.Char.AsciiChar = Shade_Mapper::FastColorToChar(frame_buffer[i]);
        ci.Attributes = baseAttr;
    }

    char fpsbuf[64];
    const int n = std::snprintf(fpsbuf, sizeof(fpsbuf), "FPS: %d", fps);
    const int fy = height - 1;
    int i0 = fy * width;

    for (int k = 0; k < n && k < width; ++k)
    {
        CHAR_INFO& ci = char_info[i0 + k];
        ci.Char.AsciiChar = fpsbuf[k];
        ci.Attributes = baseAttr;
    }

    COORD bufSize = {(SHORT)width, (SHORT)height};
    COORD bufCoord = {0, 0};
    SMALL_RECT rect = {0, 0, (SHORT)(width - 1), (SHORT)(height - 1)};

    WriteConsoleOutputA(hConsole, char_info.data(), bufSize, bufCoord, &rect);
}

void Output_Handler::PrintBuffer(const std::vector<Color>& frame_buffer, int fps)
{
    switch (print_type)
    {
        case Print_Type::NORMAL:
            PrintBufferNormal(frame_buffer, fps);
            break;
        case Print_Type::FAST:
            PrintBufferFast(frame_buffer, fps);
            break;
        case Print_Type::VT:
            PrintBufferVT(frame_buffer, fps);
            break;
        default:
            break;
    }
}

void Output_Handler::PrintClear()
{
    char buf[height+2][width];

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            buf[y][x] = ' ';
        }
    }

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