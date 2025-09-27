#pragma once

#include <chrono>
#include <vector>
#include <string>

using steady_clock = std::chrono::steady_clock;

struct Profile_Bucket
{
    int64_t start = 0;
    int64_t total = 0;
    int64_t count = 0;
};

class Profiler
{
public:
    Profiler (int size = 10);

    void Start(int index);
    void End(int index);
    void Result() const;
    void Ratio(int index_0, int index_1) const;
    int64_t GetTotal(int index) const;

private:
    std::vector<Profile_Bucket> buckets;
    std::string index_name[20];

    void IndexNameInit();
};

