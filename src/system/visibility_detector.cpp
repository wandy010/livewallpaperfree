#include "visibility_detector.h"

bool VisibilityDetector::IsDesktopObscured() {
    HWND foreground = GetForegroundWindow();
    if (!foreground) return false;

    // Desktop itself or shell
    if (foreground == GetDesktopWindow()) return false;
    if (foreground == GetShellWindow())   return false;

    // Check if the foreground window is a normal maximized window
    // that covers the entire desktop
    if (!IsWindowVisible(foreground)) return false;

    LONG style = GetWindowLong(foreground, GWL_STYLE);
    if (!(style & WS_CAPTION) && !(style & WS_POPUP)) return false;

    // If the foreground window is maximized, it covers the desktop
    if (IsZoomed(foreground)) return true;

    // If the screen is locked (Win+L), the desktop isn't visible
    // (handled via fullscreen detection of the lock screen)

    return false;
}
