#pragma once

#include <functional>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <winuser.h>

namespace util 
{

typedef std::function<void (WPARAM)> KeyboardCallback;

void InitGlobalListener();
void AttachKeyboardListener(DWORD key, KeyboardCallback callback);

}
