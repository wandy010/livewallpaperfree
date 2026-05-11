#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <string>

class ConfigManager;
class MpvPlayer;
class AudioController;
class VolumeSlider;

class SettingsWindow {
public:
    SettingsWindow(HINSTANCE hInstance, HWND parent_hwnd,
                   ConfigManager& config, MpvPlayer& player,
                   AudioController& audio);
    ~SettingsWindow();

    void Show();
    void Close();
    HWND GetHwnd() const;

private:
    HINSTANCE hInstance_;
    HWND parent_hwnd_;
    HWND hwnd_ = nullptr;
    ConfigManager& config_;
    MpvPlayer& player_;
    AudioController& audio_;
    VolumeSlider* slider_ = nullptr;

    HWND edit_path_;
    HWND chk_mute_;
    HWND chk_pause_fs_;
    HWND chk_pause_hidden_;
    HWND edit_fps_;
    HWND btn_browse_;
    HWND btn_save_;
    HWND btn_cancel_;

    bool RegisterClass();
    void CreateControls();
    void PopulateControls();
    void SaveSettings();

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg,
                                    WPARAM wParam, LPARAM lParam);
};
