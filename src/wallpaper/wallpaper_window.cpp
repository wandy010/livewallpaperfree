#include "wallpaper_window.h"
#include "utils/logger.h"

WallpaperWindow::WallpaperWindow(HINSTANCE hInstance)
    : hInstance_(hInstance) {}

WallpaperWindow::~WallpaperWindow() {
    Destroy();
}

bool WallpaperWindow::RegisterClass() {
    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance_;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc.lpszClassName = "WallpaperEngineClone_Wallpaper";

    if (!RegisterClassExA(&wc)) {
        Logger::error("Failed to register wallpaper window class");
        return false;
    }
    return true;
}

bool WallpaperWindow::Create(HWND parent_hwnd, int monitor_index) {
    if (!RegisterClass()) return false;

    parent_hwnd_ = parent_hwnd;

    int screen_x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int screen_y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int screen_w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int screen_h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    // Use WS_POPUP instead of WS_CHILD to avoid creation failures with Progman/WorkerW
    // We will then use SetParent to attach it to the host
    hwnd_ = CreateWindowExA(
        WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TRANSPARENT,
        "WallpaperEngineClone_Wallpaper",
        "Wallpaper",
        WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS,
        screen_x, screen_y, screen_w, screen_h,
        nullptr, // Create as top-level first
        nullptr,
        hInstance_,
        nullptr
    );

    if (!hwnd_) {
        Logger::error("Failed to create wallpaper window");
        return false;
    }

    // Now attach it to the host window
    if (parent_hwnd_) {
        SetParent(hwnd_, parent_hwnd_);
    }

    SetLayeredWindowAttributes(hwnd_, 0, 255, LWA_ALPHA);

    Logger::info("Wallpaper window created and attached to host");
    return true;
}

void WallpaperWindow::Destroy() {
    if (hwnd_) {
        DestroyWindow(hwnd_);
        hwnd_ = nullptr;
    }
}

HWND WallpaperWindow::Handle() const {
    return hwnd_;
}

void WallpaperWindow::Resize(int width, int height) {
    if (hwnd_) {
        SetWindowPos(hwnd_, nullptr, 0, 0, width, height,
                     SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
    }
}

void WallpaperWindow::Reparent(HWND new_parent) {
    if (hwnd_ && new_parent) {
        SetParent(hwnd_, new_parent);
        parent_hwnd_ = new_parent;
        Logger::info("Wallpaper window reparented");
    }
}

void WallpaperWindow::Show() {
    if (hwnd_) ShowWindow(hwnd_, SW_SHOW);
}

void WallpaperWindow::Hide() {
    if (hwnd_) ShowWindow(hwnd_, SW_HIDE);
}

LRESULT CALLBACK WallpaperWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_SIZE:
            return 0;

        default:
            return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
}
