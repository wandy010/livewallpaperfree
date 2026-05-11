#include "settings_window.h"
#include "volume_slider.h"
#include "config/config_manager.h"
#include "video/mf_player.h"
#include "video/audio_controller.h"
#include "utils/logger.h"

#include <commdlg.h>
#include <cstdlib>
#include <algorithm>

SettingsWindow::SettingsWindow(HINSTANCE hInstance, HWND parent_hwnd,
                                ConfigManager& config, MFPlayer& player,
                                AudioController& audio)
    : hInstance_(hInstance)
    , parent_hwnd_(parent_hwnd)
    , config_(config)
    , player_(player)
    , audio_(audio) {}

SettingsWindow::~SettingsWindow() {
    Close();
}

bool SettingsWindow::RegisterClass() {
    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance_;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = "WallpaperEngineClone_Settings";

    return RegisterClassExA(&wc) != 0;
}

void SettingsWindow::CreateControls() {
    int y = 20;

    // ── Video path ───────────────────────────────────────────────────────
    CreateWindowExA(0, "STATIC", "Video file:", WS_CHILD | WS_VISIBLE,
                    20, y, 80, 20, hwnd_, nullptr, hInstance_, nullptr);

    edit_path_ = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT",
                                 config_.video_path().c_str(),
                                 WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                                 110, y, 200, 22, hwnd_,
                                 nullptr, hInstance_, nullptr);

    btn_browse_ = CreateWindowExA(0, "BUTTON", "...",
                                  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                  320, y, 30, 22, hwnd_,
                                  reinterpret_cast<HMENU>(2001),
                                  hInstance_, nullptr);
    y += 35;

    // ── Volume ───────────────────────────────────────────────────────────
    CreateWindowExA(0, "STATIC", "Volume:", WS_CHILD | WS_VISIBLE,
                    20, y, 80, 20, hwnd_, nullptr, hInstance_, nullptr);

    slider_ = new VolumeSlider();
    slider_->Create(hwnd_, 110, y, 200, 30, 2002);
    slider_->SetVolume(config_.volume());
    y += 40;

    // ── Mute ─────────────────────────────────────────────────────────────
    chk_mute_ = CreateWindowExA(0, "BUTTON", "Mute",
                                WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                                110, y, 100, 22, hwnd_,
                                nullptr, hInstance_, nullptr);
    SendMessage(chk_mute_, BM_SETCHECK,
                config_.mute() ? BST_CHECKED : BST_UNCHECKED, 0);
    y += 30;

    // ── Pause on fullscreen ──────────────────────────────────────────────
    chk_pause_fs_ = CreateWindowExA(0, "BUTTON", "Pause on fullscreen",
                                    WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                                    110, y, 200, 22, hwnd_,
                                    nullptr, hInstance_, nullptr);
    SendMessage(chk_pause_fs_, BM_SETCHECK,
                config_.pause_on_fullscreen() ? BST_CHECKED : BST_UNCHECKED, 0);
    y += 30;

    // ── Pause when hidden ────────────────────────────────────────────────
    chk_pause_hidden_ = CreateWindowExA(0, "BUTTON", "Pause when hidden",
                                        WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                                        110, y, 200, 22, hwnd_,
                                        nullptr, hInstance_, nullptr);
    SendMessage(chk_pause_hidden_, BM_SETCHECK,
                config_.pause_when_hidden() ? BST_CHECKED : BST_UNCHECKED, 0);
    y += 35;

    // ── FPS limit ────────────────────────────────────────────────────────
    CreateWindowExA(0, "STATIC", "FPS limit:", WS_CHILD | WS_VISIBLE,
                    20, y, 80, 20, hwnd_, nullptr, hInstance_, nullptr);

    edit_fps_ = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT",
                                std::to_string(config_.fps_limit()).c_str(),
                                WS_CHILD | WS_VISIBLE | ES_NUMBER,
                                110, y, 50, 22, hwnd_,
                                nullptr, hInstance_, nullptr);
    y += 45;

    // ── Buttons ──────────────────────────────────────────────────────────
    btn_save_ = CreateWindowExA(0, "BUTTON", "Save",
                                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                140, y, 80, 28, hwnd_,
                                reinterpret_cast<HMENU>(IDOK),
                                hInstance_, nullptr);

    btn_cancel_ = CreateWindowExA(0, "BUTTON", "Cancel",
                                  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                  240, y, 80, 28, hwnd_,
                                  reinterpret_cast<HMENU>(IDCANCEL),
                                  hInstance_, nullptr);
}

void SettingsWindow::PopulateControls() {
    SetWindowTextA(edit_path_, config_.video_path().c_str());
    slider_->SetVolume(config_.volume());
    SendMessage(chk_mute_, BM_SETCHECK,
                config_.mute() ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(chk_pause_fs_, BM_SETCHECK,
                config_.pause_on_fullscreen() ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(chk_pause_hidden_, BM_SETCHECK,
                config_.pause_when_hidden() ? BST_CHECKED : BST_UNCHECKED, 0);
    SetWindowTextA(edit_fps_, std::to_string(config_.fps_limit()).c_str());
}

void SettingsWindow::SaveSettings() {
    // Video path
    CHAR path[MAX_PATH] = {};
    GetWindowTextA(edit_path_, path, MAX_PATH);
    config_.set_video_path(path);

    // Volume from slider
    if (slider_) {
        int vol = slider_->GetVolume();
        config_.set_volume(vol);
        audio_.SetVolume(vol);
    }

    // Mute
    bool mute = SendMessage(chk_mute_, BM_GETCHECK, 0, 0) == BST_CHECKED;
    config_.set_mute(mute);
    audio_.SetMute(mute);

    // Pause on fullscreen
    config_.set_pause_on_fullscreen(
        SendMessage(chk_pause_fs_, BM_GETCHECK, 0, 0) == BST_CHECKED);

    // Pause when hidden
    config_.set_pause_when_hidden(
        SendMessage(chk_pause_hidden_, BM_GETCHECK, 0, 0) == BST_CHECKED);

    // FPS limit
    CHAR fps_str[16] = {};
    GetWindowTextA(edit_fps_, fps_str, 16);
    int fps = std::max(1, atoi(fps_str));
    config_.set_fps_limit(fps);

    // Save to file
    config_.save("assets/config/settings.json");

    Logger::info("Settings saved");
}

void SettingsWindow::Show() {
    if (hwnd_) {
        ShowWindow(hwnd_, SW_SHOW);
        SetForegroundWindow(hwnd_);
        return;
    }

    if (!RegisterClass()) return;

    int w = 380, h = 300;

    hwnd_ = CreateWindowExA(
        0,
        "WallpaperEngineClone_Settings",
        "WallpaperEngineClone — Settings",
        WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, w, h,
        parent_hwnd_,
        nullptr,
        hInstance_,
        this
    );

    if (!hwnd_) {
        Logger::error("Failed to create settings window");
        return;
    }

    CreateControls();
    Logger::info("Settings window opened");
}

HWND SettingsWindow::GetHwnd() const {
    return hwnd_;
}

void SettingsWindow::Close() {
    if (hwnd_) {
        DestroyWindow(hwnd_);
        hwnd_ = nullptr;
    }
    delete slider_;
    slider_ = nullptr;
}

LRESULT CALLBACK SettingsWindow::WndProc(HWND hwnd, UINT msg,
                                         WPARAM wParam, LPARAM lParam)
{
    SettingsWindow* self = nullptr;

    if (msg == WM_CREATE) {
        auto* cs = reinterpret_cast<CREATESTRUCTA*>(lParam);
        self = static_cast<SettingsWindow*>(cs->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA,
                         reinterpret_cast<LONG_PTR>(self));
    } else {
        self = reinterpret_cast<SettingsWindow*>(
            GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (!self) return DefWindowProcA(hwnd, msg, wParam, lParam);

    switch (msg) {
        case WM_COMMAND: {
            int id = LOWORD(wParam);
            if (id == IDOK) {
                self->SaveSettings();
                DestroyWindow(hwnd);
            } else if (id == IDCANCEL) {
                DestroyWindow(hwnd);
            } else if (id == 2001) {
                // Browse button
                OPENFILENAMEA ofn = {};
                CHAR file[MAX_PATH] = {};
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hwnd;
                ofn.lpstrFilter = "Video Files\0*.mp4;*.avi;*.mkv;*.mov;*.webm\0All Files\0*.*\0";
                ofn.lpstrFile = file;
                ofn.nMaxFile = MAX_PATH;
                ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

                if (GetOpenFileNameA(&ofn)) {
                    SetWindowTextA(self->edit_path_, file);
                }
            }
            return 0;
        }

        case WM_HSCROLL: {
            if (self->slider_ &&
                reinterpret_cast<HWND>(lParam) == self->slider_->Handle())
            {
                int vol = self->slider_->GetVolume();
                if (self->slider_) {
                    self->config_.set_volume(vol);
                    self->audio_.SetVolume(vol);
                }
            }
            return 0;
        }

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            self->hwnd_ = nullptr;
            return 0;

        default:
            return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
}
