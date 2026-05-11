#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

class VisibilityDetector {
public:
    static bool IsDesktopObscured();

private:
    VisibilityDetector() = delete;
};
