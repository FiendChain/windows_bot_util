#pragma once

#include <stdint.h>

namespace util 
{

enum MouseButton {
    LEFT, MIDDLE, RIGHT
};

// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-mouse_event
// refer to this to set events more finely (ie. dragging the mouse etc)
void SendMouseEvent(uint32_t ev, int x, int y, uint32_t data=0);

// basic api for just generating clicks (no dragging)
void Click(const int x, const int y, const MouseButton button);
bool SetCursorPosition(const int x, const int y);


}