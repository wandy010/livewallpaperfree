#include "tray_icon.h"
#include "utils/logger.h"

#include <stringapiset.h>

TrayIcon::TrayIcon(HINSTANCE hInstance)
    : hInstance_(hInstance) {}

TrayIcon::~TrayIcon() {
    Destroy();
}

std::wstring TrayIcon::ToWide(const std::string& str) {
    if (str.empty()) return {};
    int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(len - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], len);
    return wstr;
}

bool TrayIcon::Create(HWND parent_hwnd) {
    parent_hwnd_ = parent_hwnd;

    nid_.cbSize = sizeof(NOTIFYICONDATAW);
    nid_.hWnd = parent_hwnd_;
    nid_.uID = 1;
    nid_.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid_.uCallbackMessage = WM_TRAY_NOTIFY;
    nid_.hIcon = LoadIcon(nullptr, IDI_APPLICATION);

    std::wstring tip = L"WallpaperEngineClone";
    wcsncpy_s(nid_.szTip, tip.c_str(), _TRUNCATE);

    if (!Shell_NotifyIconW(NIM_ADD, &nid_)) {
        Logger::error("Failed to create tray icon");
        return false;
    }

    created_ = true;
    Logger::info("Tray icon created");
    return true;
}

void TrayIcon::Destroy() {
    if (created_) {
        Shell_NotifyIconW(NIM_DELETE, &nid_);
        created_ = false;
        Logger::info("Tray icon removed");
    }
}

void TrayIcon::ShowBalloon(const std::string& title, const std::string& message) {
    if (!created_) return;

    nid_.uFlags |= NIF_INFO;
    nid_.uTimeout = 3000;
    nid_.dwInfoFlags = NIIF_INFO;

    std::wstring wtitle = ToWide(title);
    std::wstring wmsg = ToWide(message);
    wcsncpy_s(nid_.szInfoTitle, wtitle.c_str(), _TRUNCATE);
    wcsncpy_s(nid_.szInfo, wmsg.c_str(), _TRUNCATE);

    Shell_NotifyIconW(NIM_MODIFY, &nid_);
    Logger::info("Tray balloon: " + title);
}

void TrayIcon::SetCallback(CommandCallback callback) {
    callback_ = std::move(callback);
}

HMENU TrayIcon::CreateContextMenu() {
    HMENU menu = CreatePopupMenu();
    if (!menu) return nullptr;

    AppendMenuA(menu, MF_STRING, CMD_PLAY_PAUSE,  "Play / Pause");
    AppendMenuA(menu, MF_STRING, CMD_TOGGLE_MUTE, "Toggle Mute");
    AppendMenuA(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuA(menu, MF_STRING, CMD_SETTINGS,    "Settings...");
    AppendMenuA(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuA(menu, MF_STRING, CMD_EXIT,        "Exit");

    return menu;
}
