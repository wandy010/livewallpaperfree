#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <shellapi.h>

#include <string>
#include <functional>

class TrayIcon {
public:
    using CommandCallback = std::function<void(int command_id)>;

    enum Command {
        CMD_PLAY_PAUSE  = 1001,
        CMD_TOGGLE_MUTE = 1002,
        CMD_SETTINGS    = 1003,
        CMD_EXIT        = 1004
    };

    explicit TrayIcon(HINSTANCE hInstance);
    ~TrayIcon();

    bool Create(HWND parent_hwnd);
    void Destroy();
    void ShowBalloon(const std::string& title, const std::string& message);
    void SetCallback(CommandCallback callback);
    HMENU CreateContextMenu();

    static const UINT WM_TRAY_NOTIFY = WM_APP + 1;

private:
    HINSTANCE hInstance_;
    HWND parent_hwnd_ = nullptr;
    NOTIFYICONDATAW nid_ = {};
    bool created_ = false;
    CommandCallback callback_;

    static std::wstring ToWide(const std::string& str);
};
