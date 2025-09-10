#pragma once

class Back_Off_Lock
{
public:
    Back_Off_Lock();

    void Lock();
    void Unlock();

private:
    static const int MIN_DELAY = 1;
    static const int MAX_DELAY = 1024;

    volatile int state;
};