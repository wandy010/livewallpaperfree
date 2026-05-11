#include "application.h"
#include "ui/settings_window.h"
#include "utils/logger.h"
#include "wallpaper/desktop_attach.h"
#include "system/fullscreen_detector.h"
#include "system/visibility_detector.h"

#include <commctrl.h>

Application::Application(HINSTANCE hInstance)
    : hInstance_(hInstance)
    , window_(hInstance)
    , wallpaper_window_(hInstance)
    , media_control_(mf_player_)
    , audio_control_(mf_player_)
    , tray_icon_(hInstance) {}

Application::~Application() {
    Shutdown();
}

bool Application::Init() {
    Logger::info("Initializing application...");

    INITCOMMONCONTROLSEX icc = {};
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_STANDARD_CLASSES | ICC_BAR_CLASSES;
    InitCommonControlsEx(&icc);

    if (!config_.load("assets/config/settings.json")) {
        Logger::warn("Using default configuration");
    }

    if (!window_.Create("WallpaperEngineClone", 0, 0, false)) {
        Logger::error("Failed to create main window");
        return false;
    }

    window_.SetMessageHandler([this](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        return HandleMessage(hwnd, msg, wParam, lParam);
    });

    if (!InitWallpaper()) {
        Logger::error("Failed to initialize wallpaper surface");
        return false;
    }

    if (!InitVideo()) {
        Logger::warn("Video playback not available — wallpaper will be blank");
    }

    InitTrayIcon();
    StartFullscreenTimer();

    Logger::info("Application initialized successfully");
    return true;
}

bool Application::InitWallpaper() {
    Logger::info("Setting up wallpaper...");

    HWND host = DesktopAttach::SetupHost();
    if (!host) return false;

    if (!wallpaper_window_.Create(host)) {
        Logger::error("Failed to create wallpaper window");
        return false;
    }

    Logger::info("Wallpaper surface ready");
    return true;
}

bool Application::InitVideo() {
    Logger::info("Starting video engine...");

    if (!mpv_player_.Initialize()) return false;

    mpv_player_.SetOutputWindow(wallpaper_window_.Handle());

    std::string video_path = config_.video_path();
    if (!mpv_player_.LoadMedia(video_path)) {
        Logger::warn("Video file not found: " + video_path);
        return false;
    }

    mpv_player_.SetLooping(true);

    audio_control_.SetVolume(config_.volume());
    audio_control_.SetMute(config_.mute());

    if (!mpv_player_.Play()) return false;

    Logger::info("Video playback started: " + video_path);
    return true;
}

bool Application::InitTrayIcon() {
    if (!tray_icon_.Create(window_.Handle())) {
        Logger::warn("Tray icon not available");
        return false;
    }

    tray_icon_.SetCallback([this](int cmd) { OnTrayCommand(cmd); });
    tray_icon_.ShowBalloon("WallpaperEngineClone",
                           "Running — right-click for options");

    Logger::info("Tray icon active");
    return true;
}

void Application::OnTrayCommand(int cmd) {
    switch (cmd) {
        case TrayIcon::CMD_PLAY_PAUSE:
            media_control_.PlayPause();
            break;

        case TrayIcon::CMD_TOGGLE_MUTE:
            audio_control_.ToggleMute();
            break;

        case TrayIcon::CMD_SETTINGS:
            ShowSettings();
            break;

        case TrayIcon::CMD_EXIT:
            Logger::info("Exit requested from tray menu");
            PostMessage(window_.Handle(), WM_CLOSE, 0, 0);
            break;
    }
}

void Application::ShowSettings() {
    if (settings_window_ && IsWindow(settings_window_->GetHwnd())) {
        SetForegroundWindow(settings_window_->GetHwnd());
        return;
    }

    delete settings_window_;
    settings_window_ = nullptr;

    settings_window_ = new SettingsWindow(
        hInstance_, window_.Handle(),
        config_    , mpv_player_, audio_control_);

    settings_window_->Show();
}

void Application::StartFullscreenTimer() {
    HWND hwnd = window_.Handle();
    if (hwnd) {
        timer_id_ = SetTimer(hwnd, FULLSCREEN_TIMER_ID, FULLSCREEN_CHECK_MS, nullptr);
        Logger::info("Fullscreen detection timer started (" +
                     std::to_string(FULLSCREEN_CHECK_MS) + "ms)");
    }
}

void Application::StopFullscreenTimer() {
    HWND hwnd = window_.Handle();
    if (hwnd && timer_id_) {
        KillTimer(hwnd, timer_id_);
        timer_id_ = 0;
    }
}

void Application::CheckFullscreen() {
    if (!config_.pause_on_fullscreen() && !config_.pause_when_hidden()) return;

    bool fullscreen = config_.pause_on_fullscreen() &&
                      FullscreenDetector::IsForegroundFullscreen();
    bool obscured = config_.pause_when_hidden() &&
                    VisibilityDetector::IsDesktopObscured();

    bool should_pause = fullscreen || obscured;

    if (should_pause && !was_fullscreen_) {
        was_fullscreen_ = true;
        was_obscured_ = obscured;
        Logger::info(std::string("Pausing — ") +
                     (fullscreen ? "fullscreen" : "obscured"));
        if (mpv_player_.IsPlaying()) {
            mpv_player_.Pause();
        }
    } else if (!should_pause && was_fullscreen_) {
        was_fullscreen_ = false;
        was_obscured_ = false;
        Logger::info("Resuming — desktop visible");
        if (!mpv_player_.IsPlaying()) {
            mpv_player_.Play();
        }
    }
}

int Application::Run() {
    Logger::info("Entering message loop...");
    return message_loop_.Run();
}

void Application::Shutdown() {
    Logger::info("Shutting down...");
    StopFullscreenTimer();
    tray_icon_.Destroy();
    delete settings_window_;
    settings_window_ = nullptr;
    mpv_player_.Shutdown();
    wallpaper_window_.Destroy();
    window_.Destroy();
}

HINSTANCE Application::GetInstance() const {
    return hInstance_;
}

ConfigManager& Application::GetConfig() {
    return config_;
}

WallpaperWindow& Application::GetWallpaperWindow() {
    return wallpaper_window_;
}

MpvPlayer& Application::GetPlayer() {
    return mpv_player_;
}

MediaController& Application::GetMediaControl() {
    return media_control_;
}

AudioController& Application::GetAudioControl() {
    return audio_control_;
}

LRESULT Application::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case TrayIcon::WM_TRAY_NOTIFY: {
            UINT tray_msg = LOWORD(lParam);
            if (tray_msg == WM_LBUTTONUP) {
                OnTrayCommand(TrayIcon::CMD_PLAY_PAUSE);
            } else if (tray_msg == WM_RBUTTONUP) {
                HMENU menu = tray_icon_.CreateContextMenu();
                if (menu) {
                    POINT pt;
                    GetCursorPos(&pt);
                    SetForegroundWindow(hwnd);
                    TrackPopupMenu(menu, TPM_RIGHTBUTTON,
                                   pt.x, pt.y, 0, hwnd, nullptr);
                    PostMessage(hwnd, WM_NULL, 0, 0);
                    DestroyMenu(menu);
                }
            }
            return 0;
        }

        case WM_COMMAND: {
            int id = LOWORD(wParam);
            if (id >= TrayIcon::CMD_PLAY_PAUSE &&
                id <= TrayIcon::CMD_EXIT) {
                OnTrayCommand(id);
            }
            return 0;
        }

        case WM_TIMER:
            if (wParam == FULLSCREEN_TIMER_ID) {
                CheckFullscreen();
            }
            return 0;

        case WM_DESTROY:
            Logger::info("Window destroyed");
            message_loop_.Quit(0);
            return 0;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        default:
            return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
}
