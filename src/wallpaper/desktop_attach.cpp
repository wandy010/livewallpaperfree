#include "desktop_attach.h"
#include "workerw.h"
#include "utils/logger.h"

HWND DesktopAttach::SetupHost() {
    Logger::info("Preparing desktop wallpaper host...");
    HWND host = WorkerW::Setup();
    if (!host) {
        Logger::error("Could not obtain wallpaper host window");
        return nullptr;
    }
    Logger::info("Desktop wallpaper host ready");
    return host;
}

bool DesktopAttach::Attach(HWND wallpaper_hwnd, HWND host_hwnd) {
    if (!wallpaper_hwnd || !host_hwnd) {
        Logger::error("DesktopAttach::Attach — invalid handles");
        return false;
    }

    SetParent(wallpaper_hwnd, host_hwnd);

    Logger::info("Wallpaper attached to host — icons should now be on top");
    return true;
}

bool DesktopAttach::Detach(HWND wallpaper_hwnd) {
    if (!wallpaper_hwnd) return false;

    HWND desktop = GetDesktopWindow();
    SetParent(wallpaper_hwnd, desktop);

    Logger::info("Wallpaper detached from host");
    return true;
}
