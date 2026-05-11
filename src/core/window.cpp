#include "window.h"
#include "utils/logger.h"

Window::Window(HINSTANCE hInstance)
    : hInstance_(hInstance)
    , class_name_("WallpaperEngineClone_Window") {}

Window::~Window() {
    Destroy();
}

bool Window::RegisterClass() {
    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance_;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = class_name_.c_str();

    if (!RegisterClassExA(&wc)) {
        Logger::error("Failed to register window class");
        return false;
    }
    return true;
}

bool Window::Create(const std::string& title, int width, int height, bool visible) {
    if (!RegisterClass()) return false;

    hwnd_ = CreateWindowExA(
        0,
        class_name_.c_str(),
        title.c_str(),
        visible ? WS_OVERLAPPEDWINDOW : WS_POPUP,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        nullptr,
        nullptr,
        hInstance_,
        this
    );

    if (!hwnd_) {
        Logger::error("Failed to create window: " + title);
        return false;
    }

    Logger::info("Window created: " + title);
    return true;
}

void Window::Destroy() {
    if (hwnd_) {
        DestroyWindow(hwnd_);
        hwnd_ = nullptr;
    }
}

HWND Window::Handle() const {
    return hwnd_;
}

void Window::Show(int nCmdShow) {
    if (hwnd_) ShowWindow(hwnd_, nCmdShow);
}

void Window::Hide() {
    if (hwnd_) ShowWindow(hwnd_, SW_HIDE);
}

void Window::SetMessageHandler(MessageHandler handler) {
    user_handler_ = std::move(handler);
}

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    Window* window = nullptr;

    if (msg == WM_CREATE) {
        auto* cs = reinterpret_cast<CREATESTRUCTA*>(lParam);
        window = static_cast<Window*>(cs->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
    } else {
        window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (window && window->user_handler_) {
        return window->user_handler_(hwnd, msg, wParam, lParam);
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}
