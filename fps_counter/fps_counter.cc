
#include "fps_counter.h"

void Fps_Counter::Start()
{
    frame_count = 0;
    elapsed_time = 0;
    start_time = TimeStamp() - 1;
}

unsigned long long Fps_Counter::Get_Fps()
{
    ++frame_count;

    elapsed_time = TimeStamp()-start_time;

    return (frame_count*1000ULL)/elapsed_time;
}

unsigned long long Fps_Counter::TimeStamp()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}