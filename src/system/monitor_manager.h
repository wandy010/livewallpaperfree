#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <string>
#include <vector>

struct MonitorInfo {
    HMONITOR handle;
    RECT monitor_rect;
    RECT work_rect;
    std::string device_name;
    bool is_primary;
};

class MonitorManager {
public:
    static std::vector<MonitorInfo> EnumMonitors();
    static MonitorInfo GetPrimaryMonitor();
    static RECT GetVirtualDesktopRect();
    static int GetMonitorCount();

private:
    MonitorManager() = delete;

    static BOOL CALLBACK MonitorEnumProc(
        HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam);
};
