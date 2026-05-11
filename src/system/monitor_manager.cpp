#include "monitor_manager.h"

#include <vector>

BOOL CALLBACK MonitorManager::MonitorEnumProc(
    HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam)
{
    auto* monitors = reinterpret_cast<std::vector<MonitorInfo>*>(lParam);

    MONITORINFOEXA mi = {};
    mi.cbSize = sizeof(mi);

    if (GetMonitorInfoA(hMonitor, reinterpret_cast<MONITORINFO*>(&mi))) {
        MonitorInfo info;
        info.handle = hMonitor;
        info.monitor_rect = mi.rcMonitor;
        info.work_rect = mi.rcWork;
        info.device_name = mi.szDevice;
        info.is_primary = (mi.dwFlags & MONITORINFOF_PRIMARY) != 0;
        monitors->push_back(info);
    }

    return TRUE;
}

std::vector<MonitorInfo> MonitorManager::EnumMonitors() {
    std::vector<MonitorInfo> monitors;
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc,
                        reinterpret_cast<LPARAM>(&monitors));
    return monitors;
}

MonitorInfo MonitorManager::GetPrimaryMonitor() {
    auto monitors = EnumMonitors();
    for (const auto& m : monitors) {
        if (m.is_primary) return m;
    }
    if (!monitors.empty()) return monitors[0];
    return {};
}

RECT MonitorManager::GetVirtualDesktopRect() {
    RECT rect;
    rect.left   = GetSystemMetrics(SM_XVIRTUALSCREEN);
    rect.top    = GetSystemMetrics(SM_YVIRTUALSCREEN);
    rect.right  = rect.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
    rect.bottom = rect.top  + GetSystemMetrics(SM_CYVIRTUALSCREEN);
    return rect;
}

int MonitorManager::GetMonitorCount() {
    return static_cast<int>(EnumMonitors().size());
}
