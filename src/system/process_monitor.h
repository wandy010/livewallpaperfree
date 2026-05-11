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

struct ProcessInfo {
    DWORD pid;
    std::string name;
    std::string executable_path;
};

class ProcessMonitor {
public:
    static std::vector<ProcessInfo> EnumProcesses();
    static bool IsProcessRunning(const std::string& name);
    static std::string GetForegroundProcessName();
    static DWORD GetForegroundProcessId();

private:
    ProcessMonitor() = delete;
};
