#include "thread_pool.h"
#include <algorithm>

Thread_Pool::Thread_Pool(size_t thd_count, int capacity) 
: thread_count(thd_count > 0 ? thd_count : 1),
q_capacity(capacity > 0 ? capacity : 1024),
task_queue(q_capacity)
{
    workers.reserve(thread_count);

    for (int tid = 0; tid < thread_count; ++tid)
        workers.emplace_back(&Thread_Pool::WorkerLoop, this, tid);
}

Thread_Pool::~Thread_Pool()
{
    {
        std::lock_guard<std::mutex> lock_guard(mutex);
        stopping = true;
    }
    

    not_empty.notify_all();
    //not_full.notify_all();
    done.notify_all();

    for (auto& thread : workers)
        thread.join();
}

size_t Thread_Pool::GetThreadCount()
{
    return thread_count;
}

Task Thread_Pool::GetTask()
{
    Task current_task = task_queue[q_head];
    q_head = (q_head+1) % q_capacity;
    --q_size;
    return current_task;
}

void Thread_Pool::PushTask(Job job, void* ctx)
{
    // 스퓨리어스 웨이크업 대비
    std::lock_guard<std::mutex> lock(mutex);

    if ( stopping ) return;

    task_queue[q_tail] = Task{job, ctx};
    q_tail = (q_tail + 1) % q_capacity;
    ++q_size;
    ++remain_count;
}

void Thread_Pool::Start()
{
    std::unique_lock<std::mutex> lock(mutex);
    
    started = true;

    size_t number_of_thread = std::min(q_size, thread_count);

    lock.unlock();

    for (size_t i = 0; i < number_of_thread; ++i) 
        not_empty.notify_one();
}

void Thread_Pool::Wait()
{
    // 함수 블록 끝나면 락 소멸
    std::unique_lock<std::mutex> lock(mutex);

    while ( remain_count != 0 )
        done.wait(lock);

    started = false;
}

void Thread_Pool::Stop()
{
    stopping = true;
}

void Thread_Pool::WorkerLoop(int tid)
{
    std::unique_lock<std::mutex> lock(mutex);

    while(true)
    {
        while ( (q_size == 0 || started == false) && stopping == false)
        {
            not_empty.wait(lock);
        }
            
        if (stopping == true && q_size == 0)
            break;

        Task task = GetTask();

        lock.unlock();
        //not_full.notify_one();
        
        task.job(task.ctx, tid);

        lock.lock();

        --remain_count;

        if (remain_count == 0)
            done.notify_all();
    }
}