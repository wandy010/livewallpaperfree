#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

class WallpaperWindow {
public:
    explicit WallpaperWindow(HINSTANCE hInstance);
    ~WallpaperWindow();

    bool Create(HWND parent_hwnd, int monitor_index = 0);
    void Destroy();
    HWND Handle() const;
    void Resize(int width, int height);
    void Reparent(HWND new_parent);

    void Show();
    void Hide();

private:
    HINSTANCE hInstance_;
    HWND hwnd_ = nullptr;
    HWND parent_hwnd_ = nullptr;

    bool RegisterClass();
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
