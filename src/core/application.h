#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include "window.h"
#include "message_loop.h"
#include "config/config_manager.h"
#include "wallpaper/wallpaper_window.h"
#include "video/mf_player.h"
#include "video/media_controller.h"
#include "video/audio_controller.h"
#include "ui/tray_icon.h"

class SettingsWindow;

#define FULLSCREEN_TIMER_ID  1001
#define FULLSCREEN_CHECK_MS  400

class Application {
public:
    explicit Application(HINSTANCE hInstance);
    ~Application();

    bool Init();
    int Run();
    void Shutdown();

    HINSTANCE GetInstance() const;
    ConfigManager& GetConfig();
    WallpaperWindow& GetWallpaperWindow();
    MFPlayer& GetPlayer();
    MediaController& GetMediaControl();
    AudioController& GetAudioControl();

private:
    HINSTANCE hInstance_;
    Window window_;
    MessageLoop message_loop_;
    ConfigManager config_;
    WallpaperWindow wallpaper_window_;

    MFPlayer mf_player_;
    MediaController media_control_;
    AudioController audio_control_;

    TrayIcon tray_icon_;
    SettingsWindow* settings_window_ = nullptr;

    UINT_PTR timer_id_ = 0;
    bool was_fullscreen_ = false;
    bool was_obscured_ = false;

    bool InitWallpaper();
    bool InitVideo();
    bool InitTrayIcon();
    void StartFullscreenTimer();
    void StopFullscreenTimer();
    void CheckFullscreen();
    void OnTrayCommand(int cmd);
    void ShowSettings();
    LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
