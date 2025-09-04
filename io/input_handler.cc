#include <iostream>
#include "input_handler.h"
#include <algorithm>
#include <cmath>
#include <hidusage.h>  // usUsagePage/usUsage 상수
#include <cassert>
#include <vector>

static const char* kRawWndClass = "ASCII_RAWINPUT_HIDDEN";

static POINT GetCurrentMonitorCenter(HWND ref)
{
    // 기준점: ref 창 중심이 있으면 그쪽, 없으면 현재 커서 위치
    POINT pt{};
    if (IsWindow(ref)) {
        RECT wr{};
        GetWindowRect(ref, &wr);
        pt.x = (wr.left + wr.right) / 2;
        pt.y = (wr.top  + wr.bottom) / 2;
    } else {
        GetCursorPos(&pt);
    }

    HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{ sizeof(mi) };
    if (GetMonitorInfo(mon, &mi)) {
        POINT c{
            (mi.rcMonitor.left + mi.rcMonitor.right) / 2,
            (mi.rcMonitor.top  + mi.rcMonitor.bottom) / 2
        };
        return c;
    }
    // 폴백: 지금 좌표
    return pt;
}

Input_Handler::Input_Handler(HWND hwnd, bool lock_mouse)
: hwnd(hwnd ? hwnd : GetConsoleWindow()), pointer_lock(lock_mouse)
{
    key_down.fill(false);
    StartRawInput_();
}

int Input_Handler::ToVK(Key k)
{
    switch (k)
    {
        case Key::W: return 'W';
        case Key::A: return 'A';
        case Key::S: return 'S';
        case Key::D: return 'D';
        case Key::Q: return 'Q';
        case Key::E: return 'E';
        case Key::LEFTSHIFT: return VK_SHIFT;
        case Key::SPACE: return VK_SPACE;
        case Key::ESC: return VK_ESCAPE;
        default: return 0;
    }
}

void Input_Handler::UpdateKeyboard()
{
    for (size_t i = 0; i < static_cast<size_t>(Key::COUNT); ++i)
        key_down[i] = (GetAsyncKeyState(ToVK(static_cast<Key>(i))) & 0x8000) != 0;

    /*
    for (size_t i = 0; i < static_cast<size_t>(Key::COUNT); ++i)
    {
        int vk = ToVK(static_cast<Key>(i));

        if (vk == 0)
        {
            key_down[i] = false;
            continue;
        }
        
        key_down[i] = (GetAsyncKeyState(vk) & 0x8000) != 0;
    }
    */
}

LRESULT CALLBACK Input_Handler::RawWndProc_(HWND h, UINT m, WPARAM w, LPARAM l)
{
    if (m == WM_INPUT) {
        UINT size = 0;
        GetRawInputData((HRAWINPUT)l, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));
        if (size > 0) {
            std::vector<BYTE> buf(size);
            if (GetRawInputData((HRAWINPUT)l, RID_INPUT, buf.data(), &size, sizeof(RAWINPUTHEADER)) == size) {
                RAWINPUT* ri = reinterpret_cast<RAWINPUT*>(buf.data());
                if (ri->header.dwType == RIM_TYPEMOUSE) {
                    const RAWMOUSE& rm = ri->data.mouse;
                    if ((rm.usFlags & MOUSE_MOVE_ABSOLUTE) == 0) {
                        auto* self = reinterpret_cast<Input_Handler*>(GetWindowLongPtr(h, GWLP_USERDATA));
                        if (self) {
                            // raw_dx/raw_dy는 std::atomic<LONG> 권장 (C++17)
                            self->raw_dx.fetch_add(rm.lLastX, std::memory_order_relaxed);
                            self->raw_dy.fetch_add(rm.lLastY, std::memory_order_relaxed);
                        }
                    }
                }
            }
        }
        return 0;
    }
    return DefWindowProc(h, m, w, l);
}

bool Input_Handler::StartRawInput_()
{
    if (raw_running.load()) return true;

    raw_running = true;
    raw_thread = std::thread([this]{
        HINSTANCE inst = GetModuleHandle(nullptr);

        // ▼▼▼ 여기부터 "등록/생성" 블럭 통째로 ▼▼▼
        WNDCLASSEXA wc{};
        wc.cbSize        = sizeof(WNDCLASSEXA);
        wc.lpfnWndProc   = &Input_Handler::RawWndProc_; // static 멤버
        wc.hInstance     = inst;
        wc.lpszClassName = kRawWndClass;

        ATOM atom = RegisterClassExA(&wc);
        if (!atom) { raw_running = false; return; }

        HWND w = CreateWindowExA(
            WS_EX_TOOLWINDOW,
            kRawWndClass,      // const char*
            "",                // ANSI 빈 문자열
            WS_OVERLAPPED,
            CW_USEDEFAULT, CW_USEDEFAULT, 100, 100,
            nullptr, nullptr, inst, nullptr
        );
        if (!w) { raw_running = false; return; }

        SetWindowLongPtr(w, GWLP_USERDATA, (LONG_PTR)this);
        raw_hwnd = w;
        // ▲▲▲ 여기까지 그대로 ▲▲▲

        // Raw Input 등록
        RAWINPUTDEVICE rid{};
        rid.usUsagePage = HID_USAGE_PAGE_GENERIC;
        rid.usUsage     = HID_USAGE_GENERIC_MOUSE;
        rid.dwFlags     = RIDEV_INPUTSINK;   // 포커스 없어도 입력 받으려면 유지
        rid.hwndTarget  = w;
        if (!RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
            DestroyWindow(w); raw_hwnd = nullptr; raw_running = false; return;
        }

        // 메시지 루프
        MSG msg;
        while (raw_running.load() && GetMessage(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (raw_hwnd) { DestroyWindow(raw_hwnd); raw_hwnd = nullptr; }
    });

    // 창 생성 대기 (간단 동기화)
    for (int i=0; i<100; ++i) {
        if (raw_hwnd) break;
        Sleep(10);
    }
    return raw_hwnd != nullptr;
}

void Input_Handler::StopRawInput_()
{
    if (!raw_running.exchange(false)) return;
    if (raw_hwnd) PostMessage(raw_hwnd, WM_CLOSE, 0, 0);
    if (raw_thread.joinable()) raw_thread.join();
    raw_hwnd = nullptr;
}


void Input_Handler::UpdateMouse2()
{
    mouse_dx = 0.f;
    mouse_dy = 0.f;

    if (!pointer_lock) {
        raw_dx.store(0, std::memory_order_relaxed);
        raw_dy.store(0, std::memory_order_relaxed);
        has_last_pos = false;
        return;
    }

    // 1) Raw Input 델타 사용
    if (raw_running.load(std::memory_order_relaxed) && raw_hwnd) {
        // ← LONG으로 읽고 0으로 리셋
        LONG dx_i = raw_dx.exchange(0, std::memory_order_relaxed);
        LONG dy_i = raw_dy.exchange(0, std::memory_order_relaxed);

        float dx = static_cast<float>(dx_i);
        float dy = static_cast<float>(dy_i);

        // 데드존 (정수 기준이면 0만 컷해도 OK)
        if (dx > -0.5f && dx < 0.5f) dx = 0.f;
        if (dy > -0.5f && dy < 0.5f) dy = 0.f;

        mouse_dx = dx;
        mouse_dy = dy;
        return;
    }

    // 2) 폴백: 커서 위치 차분
    POINT p{};
    if (!GetCursorPos(&p)) return;

    if (!has_last_pos) {
        last_pos = p; has_last_pos = true;
        return;
    }

    float dx = static_cast<float>(p.x - last_pos.x);
    float dy = static_cast<float>(p.y - last_pos.y);

    if (std::fabs(dx) <= 1.f) dx = 0.f;
    if (std::fabs(dy) <= 1.f) dy = 0.f;

    mouse_dx = dx;
    mouse_dy = dy;
    last_pos = p;
}

void Input_Handler::UpdateMouse()
{
    mouse_dx = 0.0f;
    mouse_dy = 0.0f;

    if (pointer_lock == false || !IsWindow(hwnd))
        return;
    
    RECT cr;
    GetClientRect(hwnd, &cr);

    LONG w = cr.right - cr.left;
    LONG h = cr.bottom - cr.top;

    POINT center_screen = { w/2, h/2 };
    ClientToScreen(hwnd, &center_screen);

    POINT p;
    GetCursorPos(&p);

    mouse_dx = float(p.x - center_screen.x);
    mouse_dy = float(p.y - center_screen.y);

    SetCursorPos(center_screen.x, center_screen.y);
}

void Input_Handler::Poll()
{
    UpdateKeyboard();
    UpdateMouse2();
}

bool Input_Handler::IsKeyDown(Key k) const
{
    return key_down[static_cast<size_t>(k)];
}

Mouse_Delta Input_Handler::GetMouseDelta() const
{
    return {mouse_dx, mouse_dy};
}

void Input_Handler::PointerLockToggle()
{
    pointer_lock = !pointer_lock;
}