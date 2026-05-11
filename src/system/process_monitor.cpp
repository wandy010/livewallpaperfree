#include "process_monitor.h"

#include <tlhelp32.h>
#include <psapi.h>
#undef EnumProcesses

std::vector<ProcessInfo> ProcessMonitor::EnumProcesses() {
    std::vector<ProcessInfo> processes;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot == INVALID_HANDLE_VALUE) return processes;

    PROCESSENTRY32 pe = {};
    pe.dwSize = sizeof(pe);

    if (Process32First(snapshot, &pe)) {
        do {
            ProcessInfo info;
            info.pid  = pe.th32ProcessID;
            info.name = pe.szExeFile;

            HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,
                                       FALSE, pe.th32ProcessID);
            if (hProc) {
                CHAR path[MAX_PATH] = {};
                DWORD size = MAX_PATH;
                if (QueryFullProcessImageNameA(hProc, 0, path, &size)) {
                    info.executable_path = path;
                }
                CloseHandle(hProc);
            }

            processes.push_back(info);
        } while (Process32Next(snapshot, &pe));
    }

    CloseHandle(snapshot);
    return processes;
}

bool ProcessMonitor::IsProcessRunning(const std::string& name) {
    auto processes = EnumProcesses();
    for (const auto& p : processes) {
        if (_stricmp(p.name.c_str(), name.c_str()) == 0) {
            return true;
        }
    }
    return false;
}

std::string ProcessMonitor::GetForegroundProcessName() {
    DWORD pid = GetForegroundProcessId();
    if (pid == 0) return {};

    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION |
                               PROCESS_VM_READ, FALSE, pid);
    if (!hProc) return {};

    CHAR name[MAX_PATH] = {};
    DWORD size = MAX_PATH;
    if (GetModuleBaseNameA(hProc, nullptr, name, size) == 0) {
        // Fallback: try QueryFullProcessImageNameA
        size = MAX_PATH;
        QueryFullProcessImageNameA(hProc, 0, name, &size);
        // Extract just the filename from the path
        std::string full(name);
        auto pos = full.find_last_of("/\\");
        if (pos != std::string::npos) {
            full = full.substr(pos + 1);
        }
        CloseHandle(hProc);
        return full;
    }

    CloseHandle(hProc);
    return std::string(name);
}

DWORD ProcessMonitor::GetForegroundProcessId() {
    HWND fg = GetForegroundWindow();
    if (!fg) return 0;
    DWORD pid = 0;
    GetWindowThreadProcessId(fg, &pid);
    return pid;
}
