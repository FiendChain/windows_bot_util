#include "AutoGui.h"
#include <stdint.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <winuser.h>


namespace util {

ScreenSize GetScreenSize() {
    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);
    return {screen_width, screen_height};
}

// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-mouse_event
void SendMouseEvent(uint32_t ev, int x, int y, uint32_t data) {
    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);
    int cx = (int)((65536 * x / screen_width ) + 1);
    int cy = (int)((65536 * y / screen_height) + 1);
    mouse_event(ev, cx, cy, data, 0);
}

void Click(const int x, const int y, const MouseButton button) {
    uint32_t ev;
    switch (button) {
    // made up of bitfields (maybe this is defined in win32 somewhere?)
    case MouseButton::LEFT:     ev = 0x0002 | 0x0004; break; 
    case MouseButton::MIDDLE:   ev = 0x0020 | 0x0040; break; 
    case MouseButton::RIGHT:    ev = 0x0008 | 0x0010; break; 
    default:                    return;
    }
    SendMouseEvent(ev, x, y);
}

bool SetCursorPosition(const int x, const int y) {
    return SetCursorPos(x, y);
}



}