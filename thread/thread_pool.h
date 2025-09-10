#pragma once

#include <cstddef>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>

using Job = void(*)(void*, size_t tid);

struct Task
{
    Job job; 
    void* ctx;
};

class Thread_Pool
{
public:
    explicit Thread_Pool(int thd_count, int queue_capacity);
    ~Thread_Pool();

    Task GetTask();
    
    void Start(Job job, void* ctx);
    void Wait();
private:
    int thread_count;
    std::vector<std::thread> workers;

    const size_t q_capacity = 100;
    size_t q_head;
    size_t q_tail;
    size_t q_size;
    std::vector<Task> task_queue;

    std::mutex mutex;
    std::condition_variable not_empty;
    std::condition_variable not_full;
    std::condition_variable done;
    std::atomic<bool> stopping{false};

    size_t remain_count = 0;

    void WorkerLoop(int tid);
};