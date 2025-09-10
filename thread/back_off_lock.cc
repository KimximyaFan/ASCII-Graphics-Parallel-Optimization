#include "back_off_lock.h"

#include <stdbool.h>
#include <chrono>
#include <thread>

Back_Off_Lock::Back_Off_Lock() : state(0) {}

void Back_Off_Lock::Lock()
{
    int delay = MIN_DELAY;

    while (true)
    {
        if (__sync_bool_compare_and_swap(&state, 0, 1))
            return;

        std::this_thread::sleep_for(std::chrono::microseconds(delay));

        if (delay < MAX_DELAY)
            delay *= 2;
    }
}

void Back_Off_Lock::Unlock()
{
    __sync_lock_release(&state);
}