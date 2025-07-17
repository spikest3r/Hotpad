#include "hook.h"
#include "actions.h"
#include "window.h"
#include "globals.h"

#include <QApplication>
#include <windows.h>

// WINDOWS API SPECIFIC STUFF

HHOOK g_keyboardHook = nullptr;
bool g_processHotkeys = false;

Window *window_ref;

// execute action based on key pressed
void ReportScancode(int scanCode) {
    if(keyMap.find(scanCode) != keyMap.end()) { // does keycode exist in our map
        ActionData action = hotkeyActions[keyMap[scanCode]];
        switch(action.type) {
        case ActionType::NoAction:
            break;
        case ActionType::ActionShellExecute:
            if(action.path.empty()) break;
            ShellExecute(nullptr,L"open",action.path.c_str(),nullptr,nullptr,SW_SHOW);
            break;
        case ActionType::ActionEnterText: {
            std::vector<INPUT> inputs;
            for(size_t i = 0; i < action.path.length(); i++) {
                SHORT vk = VkKeyScan(action.path[i]);
                if(vk == -1) continue;
                BYTE vkCode = LOBYTE(vk);
                BYTE shiftState = HIBYTE(vk);

                if ((shiftState & 1)) {
                    INPUT shiftDown = {};
                    shiftDown.type = INPUT_KEYBOARD;
                    shiftDown.ki.wVk = VK_SHIFT;
                    inputs.push_back(shiftDown);
                }

                INPUT keyDown = {};
                keyDown.type = INPUT_KEYBOARD;
                keyDown.ki.wVk = vkCode;
                inputs.push_back(keyDown);

                INPUT keyUp = keyDown;
                keyUp.ki.dwFlags = KEYEVENTF_KEYUP;
                inputs.push_back(keyUp);

                if ((shiftState & 1)) {
                    INPUT shiftUp = {};
                    shiftUp.type = INPUT_KEYBOARD;
                    shiftUp.ki.wVk = VK_SHIFT;
                    shiftUp.ki.dwFlags = KEYEVENTF_KEYUP;
                    inputs.push_back(shiftUp);
                }
            }
            SendInput(inputs.size(), inputs.data(), sizeof(INPUT));
            break;
        }
        case ActionType::ActionKeyCombo: {
            std::vector<INPUT> inputs;
            int keys[3] = {VK_CONTROL, VK_MENU, VK_SHIFT};
            for(int i = 0; i < 3; i++) {
                if(!action.comboKeys[i]) continue;
                INPUT keyDown = {};
                keyDown.type = INPUT_KEYBOARD;
                keyDown.ki.wVk = keys[i];
                inputs.push_back(keyDown);
            }
            if(!action.letter.isEmpty()) {
                INPUT keyDown = {};
                keyDown.type = INPUT_KEYBOARD;

                QString key = action.letter.toLower();
                if(key == "tab") {
                    keyDown.ki.wVk = VK_TAB;
                } else if(key.startsWith("ins")) {
                    keyDown.ki.wVk = VK_INSERT;
                } else if(key.startsWith("del")) {
                    keyDown.ki.wVk = VK_DELETE;
                } else {
                    if(key.length() == 2 && key.startsWith('f')) {
                        int fKey = QString(key[1]).toInt();
                        int baseKey = VK_F1; // 0x70
                        if(fKey > 0 && fKey < 25) { // valid between f1-f24 in winapi
                            fKey--;
                            baseKey += fKey;
                            keyDown.ki.wVk = baseKey;
                        }
                    } else {
                        // treat as regular letter key
                        SHORT vk = VkKeyScan(action.letter[0].toLatin1());
                        keyDown.ki.wVk = LOBYTE(vk);
                    }
                }

                inputs.push_back(keyDown);

                INPUT keyUp = keyDown;
                keyUp.ki.dwFlags = KEYEVENTF_KEYUP;
                inputs.push_back(keyUp);
            }
            for(int i = 0; i < 3; i++) {
                if(!action.comboKeys[i]) continue;
                INPUT keyUp = {};
                keyUp.type = INPUT_KEYBOARD;
                keyUp.ki.wVk = keys[i];
                keyUp.ki.dwFlags = KEYEVENTF_KEYUP;
                inputs.push_back(keyUp);
            }
            SendInput(inputs.size(), inputs.data(), sizeof(INPUT));
            break;
        }
        default:
            qDebug() << "Not implemented";
            break;
        }
    }
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if(nCode == HC_ACTION) {
        auto* p = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        DWORD scanCode = p->scanCode;
        if((GetKeyState(VK_NUMLOCK) & 0x0001) != 0) return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
        // check for extended flag because 0x35 without extended flag is slash
        if(isKeyDown && scanCode == 0x35 && (p->flags & LLKHF_EXTENDED)) {// Numpad Divide
            g_processHotkeys = !g_processHotkeys;
            window_ref->setTrayState(g_processHotkeys);
            return 1;
        }
        if(isKeyDown && scanCode == 0x37 && !(p->flags & LLKHF_EXTENDED)) {
            window_ref->showWindow();
            return 1;
        }
        if(isKeyDown && !(p->flags & LLKHF_EXTENDED) && g_processHotkeys) {
            switch (scanCode) {
            case 0x52: // Numpad 0
            case 0x4F: // Numpad 1
            case 0x50: // Numpad 2
            case 0x51: // Numpad 3
            case 0x4B: // Numpad 4
            case 0x4C: // Numpad 5
            case 0x4D: // Numpad 6
            case 0x47: // Numpad 7
            case 0x48: // Numpad 8
            case 0x49: // Numpad 9
            case 0x53: // Numpad .
                ReportScancode(scanCode);
                return 1;
            default:
                // Ignore
                break;
            }
        }
    }
    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

void InstallHook(Window *w) {
    g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, nullptr, 0);
    window_ref = w;
    if(!g_keyboardHook) {
        qDebug() << "Hook failed:" << GetLastError();
    }
}

void RemoveHook() {
    if(g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }
}
