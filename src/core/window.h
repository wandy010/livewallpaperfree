#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <string>
#include <functional>

class Window {
public:
    using MessageHandler = std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>;

    explicit Window(HINSTANCE hInstance);
    ~Window();

    bool Create(const std::string& title, int width, int height, bool visible = false);
    void Destroy();
    HWND Handle() const;
    void Show(int nCmdShow = SW_SHOW);
    void Hide();

    void SetMessageHandler(MessageHandler handler);

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    HINSTANCE hInstance_;
    HWND hwnd_ = nullptr;
    std::string class_name_;
    MessageHandler user_handler_;

    bool RegisterClass();
};
