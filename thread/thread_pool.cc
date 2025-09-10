#include "thread_pool.h"


Thread_Pool::Thread_Pool(int thd_count) : thread_count(thd_count)
{
    if (thread_count <= 0) 
        thread_count = 1;

    workers.reserve(thread_count);

    for (int tid = 0; tid < thread_count; ++tid)
    {
        
    }
}

Task Thread_Pool::GetTask()
{
    Task current_task = task_queue[q_head];
    q_head = (q_head+1) % q_capacity;
    --q_size;
    return current_task;
}

void Thread_Pool::WorkerLoop(int tid)
{
    std::unique_lock<std::mutex> lock(mutex);

    while(true)
    {
        while (q_size == 0 && stopping == false)
            not_empty.wait(lock);

        if (stopping == true && q_size == 0)
            break;

        Task task = GetTask();

        lock.unlock();
        not_full.notify_one();
        
        task.job(task.ctx, tid);

        lock.lock();

        remain_count--;

        if (remain_count == 0)
            done.notify_all();
    }
}