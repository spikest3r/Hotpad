#include <windows.h>
#include "window.h"

#ifndef HOOK_H
#define HOOK_H

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
void RemoveHook();
void InstallHook(Window *w);

#endif // HOOK_H
