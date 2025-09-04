#pragma once

#include <cstdint>
#include <windows.h>
#include <array>
#include <atomic>
#include <thread>

enum class Key : uint8_t
{
    W, 
    A, 
    S, 
    D,
    Q,
    E,
    LEFTSHIFT,
    SPACE,
    ESC,
    COUNT
};

struct Mouse_Delta
{
    float dx, dy;
};

class Input_Handler
{
public:
    Input_Handler(HWND hwnd, bool lock_mouse);
    ~Input_Handler() = default;

    void Poll();

    bool IsKeyDown(Key k) const;

    Mouse_Delta GetMouseDelta() const;

    void PointerLockToggle();
private:
    HWND hwnd = nullptr;
    bool pointer_lock;
    float mouse_dx;
    float mouse_dy;
    std::array<bool, static_cast<size_t>(Key::COUNT)> key_down;

    // ==== Raw Input 관련 ====
    bool StartRawInput_();        // 초기화
    void StopRawInput_();         // 해제
    static LRESULT CALLBACK RawWndProc_(HWND h, UINT m, WPARAM w, LPARAM l);

    
    std::atomic<LONG> raw_dx{0};
    std::atomic<LONG> raw_dy{0};
    std::atomic<bool>  raw_running{false};
    std::thread        raw_thread;
    HWND               raw_hwnd = nullptr;

    // 커서 리센터 백업(폴백용)
    POINT last_pos{0,0};
    bool  has_last_pos = false;



    static int ToVK(Key k);
    void UpdateKeyboard();
    void UpdateMouse();
    void UpdateMouse2();
};
