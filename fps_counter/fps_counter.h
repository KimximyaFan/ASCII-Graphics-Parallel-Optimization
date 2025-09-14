#pragma once

#include <chrono>
#include <deque>

class Fps_Counter
{
public:
    Fps_Counter();
    void Start();
    int GetFpsAvg ();
    int GetFpsDiff ();
private:
    using Clock = std::chrono::steady_clock;
    using Time_Point = Clock::time_point;

    unsigned long long frame_count;
    unsigned long long start_time;
    unsigned long long elapsed_time;

    double window_second = 1.0;

    const Clock::duration window = std::chrono::duration_cast<Clock::duration>(
        std::chrono::duration<double>(window_second)
    );

    std::deque<Time_Point> samples;
};