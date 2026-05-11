#include "fullscreen_detector.h"

bool FullscreenDetector::IsForegroundFullscreen() {
    HWND hwnd = GetForegroundWindow();
    if (!hwnd) return false;

    // Ignore the desktop and our own invisible windows
    if (hwnd == GetDesktopWindow()) return false;
    if (hwnd == GetShellWindow()) return false;

    return IsWindowFullscreen(hwnd);
}

bool FullscreenDetector::IsWindowFullscreen(HWND hwnd) {
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);

    if (!GetMonitorInfo(monitor, &mi)) return false;

    RECT window_rect;
    if (!GetWindowRect(hwnd, &window_rect)) return false;

    return RectsEqual(window_rect, mi.rcMonitor);
}

bool FullscreenDetector::RectsEqual(const RECT& a, const RECT& b) {
    return a.left   == b.left   &&
           a.top    == b.top    &&
           a.right  == b.right  &&
           a.bottom == b.bottom;
}
