#include "volume_slider.h"
#include "utils/logger.h"

#include <algorithm>

VolumeSlider::VolumeSlider() = default;

VolumeSlider::~VolumeSlider() {
    Destroy();
}

bool VolumeSlider::Create(HWND parent_hwnd, int x, int y, int w, int h, int id) {
    hwnd_ = CreateWindowExA(
        0,
        TRACKBAR_CLASSA,
        "Volume",
        WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_HORZ,
        x, y, w, h,
        parent_hwnd,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
        GetModuleHandle(nullptr),
        nullptr
    );

    if (!hwnd_) {
        Logger::error("Failed to create volume slider");
        return false;
    }

    SendMessage(hwnd_, TBM_SETRANGE, TRUE, MAKELPARAM(0, 100));
    SendMessage(hwnd_, TBM_SETTICFREQ, 10, 0);
    SendMessage(hwnd_, TBM_SETPOS, TRUE, current_volume_);

    Logger::info("Volume slider created");
    return true;
}

void VolumeSlider::Destroy() {
    if (hwnd_) {
        DestroyWindow(hwnd_);
        hwnd_ = nullptr;
    }
}

HWND VolumeSlider::Handle() const {
    return hwnd_;
}

void VolumeSlider::SetVolume(int volume) {
    current_volume_ = std::clamp(volume, 0, 100);
    if (hwnd_) {
        SendMessage(hwnd_, TBM_SETPOS, TRUE, current_volume_);
    }
}

int VolumeSlider::GetVolume() const {
    if (hwnd_) {
        return static_cast<int>(SendMessage(hwnd_, TBM_GETPOS, 0, 0));
    }
    return current_volume_;
}

void VolumeSlider::SetCallback(ChangeCallback callback) {
    callback_ = std::move(callback);
}
