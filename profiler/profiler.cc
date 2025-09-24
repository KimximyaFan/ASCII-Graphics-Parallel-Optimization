#include "profiler.h"
#include <cstdio>
#include <cstdint>

static inline int64_t Time_Stamp() noexcept {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        clock::now().time_since_epoch()
    ).count();
}

Profiler::Profiler(int size) : buckets(size) {}

void Profiler::Start(int index)
{
    buckets[index].start = Time_Stamp();
}

void Profiler::End(int index)
{
    buckets[index].total += Time_Stamp() - buckets[index].start;
    ++buckets[index].count;
}

void Profiler::Result() const
{
    int idx = -1;
    for (const auto& b : buckets)
    {
        ++idx;

        if ( b.count == 0 )
            continue;

        double total_sec = static_cast<double>(b.total) / 1'000'000'000.0;
        double avg_sec   = total_sec / static_cast<double>(b.count);

        printf("index : %d\n", idx);
        printf("avg_sec : %.4f sec\n", avg_sec);
        printf("total_sec : %.4f sec\n", total_sec);
        printf("count : %llu\n", b.count);
        printf("\n");
    }
}

void Profiler::Ratio(int a, int b) const
{
    const auto& b0 = buckets[a];
    const auto& b1 = buckets[b];

    if ( b0.count == 0 || b1.count == 0 || b1.total == 0 )
    {
        printf("zero error");
        return;
    }

    double avg_ratio = ((double)b0.total / (double)b0.count ) / ((double)b1.total / (double)b1.count ) * 100;
    double total_ratio = (double)b0.total / (double)b1.total * 100;

    printf("a : %d   b : %d\n", a, b);
    printf("avg : %.3f %%\n", avg_ratio);
    printf("total : %.3f %%\n", total_ratio);
    printf("\n");
}