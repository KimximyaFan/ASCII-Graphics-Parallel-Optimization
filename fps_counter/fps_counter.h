#pragma once

#include <chrono>
#include <iostream>

class Fps_Counter
{
public:
    Fps_Counter() {};

    void Start();

    unsigned long long Get_Fps ();

private:
    unsigned long long frame_count;
    unsigned long long start_time;
    unsigned long long elapsed_time;

    static unsigned long long TimeStamp();
};