#include "workerw.h"
#include "utils/logger.h"

#include <cstring>
#include <string>
#include <vector>

// Callback for EnumWindows — collects WorkerW windows that contain SHELLDLL_DefView
struct WorkerWInfo {
    HWND workerw = nullptr;       // WorkerW that holds SHELLDLL_DefView (desktop icons)
    HWND prev_workerw = nullptr;  // WorkerW before the icons one (wallpaper host)
};

static BOOL CALLBACK EnumWorkerWCallback(HWND hwnd, LPARAM lParam) {
    auto* info = reinterpret_cast<WorkerWInfo*>(lParam);

    HWND shell_view = FindWindowExA(hwnd, nullptr, "SHELLDLL_DefView", nullptr);
    if (!shell_view) return TRUE;

    // This WorkerW has the desktop icons — find the WorkerW before it in Z-order
    info->workerw = hwnd;

    HWND candidate = GetWindow(hwnd, GW_HWNDPREV);
    if (candidate && IsWindowVisible(candidate)) {
        char class_name[128] = {};
        GetClassNameA(candidate, class_name, sizeof(class_name));
        if (std::strcmp(class_name, "WorkerW") == 0) {
            info->prev_workerw = candidate;
        }
    }

    return FALSE; // Found what we need, stop enumeration
}

HWND WorkerW::FindProgman() {
    HWND progman = FindWindowA("Progman", nullptr);
    if (!progman) {
        Logger::error("Cannot find Progman window. "
                      "Are you sure this is running on Windows?");
    }
    return progman;
}

bool WorkerW::TriggerWorkerW(HWND progman) {
    if (!progman) return false;

    DWORD_PTR result = 0;
    LRESULT ok = SendMessageTimeoutA(
        progman,
        0x052C,
        0x0000000D,
        0x00000000,
        SMTO_NORMAL,
        1000,
        &result
    );

    if (!ok) {
        Logger::warn("SendMessageTimeout to Progman failed (0x052C)");
        return false;
    }

    Logger::info("WorkerW triggered via Progman 0x052C");
    return true;
}

HWND WorkerW::Setup() {
    Logger::info("Setting up wallpaper host (WorkerW) for modern Windows...");

    // Step 1: Check if a suitable WorkerW already exists
    WorkerWInfo info = {};
    EnumWindows(EnumWorkerWCallback, reinterpret_cast<LPARAM>(&info));

    HWND host = nullptr;

    if (info.workerw && info.prev_workerw) {
        // We found both the icons WorkerW and the host WorkerW
        host = info.prev_workerw;
        Logger::info("Found existing wallpaper host WorkerW");
    } else {
        // Step 2: Trigger WorkerW creation via Progman
        HWND progman = FindProgman();
        if (!progman) return nullptr;

        TriggerWorkerW(progman);

        // Step 3: Re-scan for WorkerW windows
        // Sometimes the new WorkerW appears after a brief delay
        for (int attempt = 0; attempt < 5; attempt++) {
            WorkerWInfo retry_info = {};
            EnumWindows(EnumWorkerWCallback, reinterpret_cast<LPARAM>(&retry_info));
            if (retry_info.workerw && retry_info.prev_workerw) {
                host = retry_info.prev_workerw;
                Logger::info("Found wallpaper host WorkerW after trigger");
                break;
            }
            Sleep(200);
        }

        if (!host) {
            // Fallback: use Progman directly (older Windows)
            host = progman;
            Logger::warn("Could not find WorkerW host — falling back to Progman");
        }
    }

    if (host) {
        char class_name[128] = {};
        GetClassNameA(host, class_name, sizeof(class_name));
        Logger::info(std::string("Wallpaper host ready: ") + class_name);
    }

    return host;
}
