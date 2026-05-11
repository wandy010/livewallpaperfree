#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

class DesktopAttach {
public:
    static HWND SetupHost();
    static bool Attach(HWND wallpaper_hwnd, HWND host_hwnd);
    static bool Detach(HWND wallpaper_hwnd);

private:
    DesktopAttach() = delete;
};
