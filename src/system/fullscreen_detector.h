#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

class FullscreenDetector {
public:
    static bool IsForegroundFullscreen();

private:
    FullscreenDetector() = delete;
    static bool IsWindowFullscreen(HWND hwnd);
    static bool RectsEqual(const RECT& a, const RECT& b);
};
