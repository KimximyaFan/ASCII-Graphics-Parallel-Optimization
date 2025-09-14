
#include "fps_counter.h"
#include <cmath>

static inline unsigned long long TimeStamp() noexcept
{
    using clock = std::chrono::steady_clock;
    using ms    = std::chrono::milliseconds;
    return std::chrono::duration_cast<ms>(clock::now().time_since_epoch()).count();
}

Fps_Counter::Fps_Counter()
{
    frame_count = 0;
    elapsed_time = 0;
    start_time = TimeStamp()-1;
    samples.clear();
}

int Fps_Counter::GetFpsAvg ()
{
    ++frame_count;
    elapsed_time = TimeStamp()-start_time;
    return (frame_count*1000ULL)/elapsed_time;
}

int Fps_Counter::GetFpsDiff()
{
    const Time_Point now = Clock::now();
    const Time_Point cut_off = now - window;

    samples.push_back(now);

    while( !samples.empty() && samples.front() < cut_off )
        samples.pop_front();
    
    if (samples.size() < 2) 
        return 0;

    const double span = std::chrono::duration<double>(
        samples.back() - samples.front()
    ).count();

    double avg = (samples.size() - 1) / std::max(span, 1e-6);  // 부동소수로 계산

    // 4) (옵션) EWMA로 한 번 더 스무딩
    static double smooth = 0.0;
    static Time_Point last = now;
    const double dt = std::chrono::duration<double>(now - last).count();
    last = now;
    const double half_life = 0.5;                 // 0.3~0.7s 취향대로
    const double tau = half_life / std::log(2.0);
    const double alpha = 1.0 - std::exp(-dt / tau);
    smooth += alpha * (avg - smooth);

    // HUD에 정수로만 보여주고 싶다면 여기서만 캐스팅
    return (int)(smooth + 0.5);
}