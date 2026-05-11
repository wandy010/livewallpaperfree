#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <commctrl.h>

#include <functional>

class VolumeSlider {
public:
    using ChangeCallback = std::function<void(int volume)>;

    VolumeSlider();
    ~VolumeSlider();

    bool Create(HWND parent_hwnd, int x, int y, int width, int height, int id);
    void Destroy();
    HWND Handle() const;

    void SetVolume(int volume);
    int GetVolume() const;

    void SetCallback(ChangeCallback callback);
    static LRESULT CALLBACK TrackbarWndProc(HWND hwnd, UINT msg,
                                            WPARAM wParam, LPARAM lParam,
                                            UINT_PTR uId, DWORD_PTR dwRefData);

private:
    HWND hwnd_ = nullptr;
    ChangeCallback callback_;
    int current_volume_ = 30;
};
