#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

class WorkerW {
public:
    static HWND FindProgman();
    static bool TriggerWorkerW(HWND progman);
    static HWND Setup();

private:
    WorkerW() = delete;
};
