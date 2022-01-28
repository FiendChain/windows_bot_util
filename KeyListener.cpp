#include "KeyListener.h"

#include <unordered_map>
#include <vector>

namespace util 
{

typedef std::unordered_map<DWORD, std::vector<KeyboardCallback>> KeyboardCallbacks;
KeyboardCallbacks g_callbacks;

HHOOK g_kb_hook;

// https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms644985(v=vs.85)
// https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-kbdllhookstruct?redirectedfrom=MSDN
LRESULT WINAPI GlobalKeyboardHandler(int code, WPARAM wParam, LPARAM lParam) {
    if (code < 0) {
        return CallNextHookEx(g_kb_hook, code, wParam, lParam);
    }
    KBDLLHOOKSTRUCT* info = (KBDLLHOOKSTRUCT*)(lParam);

    DWORD virtual_key = info->vkCode;
    WPARAM key_type = wParam;
    
    if (g_callbacks.find(virtual_key) != g_callbacks.end()) {
        for (auto &callback: g_callbacks.at(virtual_key)) {
            callback(key_type);
        }
    }
    return CallNextHookEx(g_kb_hook, code, wParam, lParam);
};

void InitGlobalListener() {
    g_kb_hook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)(GlobalKeyboardHandler), NULL, 0);
}

void AttachKeyboardListener(DWORD key, KeyboardCallback callback) {
    g_callbacks[key].push_back(callback);
}


}
