#include "message_loop.h"

int MessageLoop::Run() {
    running_ = true;
    MSG msg = {};

    while (running_ && GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    running_ = false;
    return exit_code_;
}

void MessageLoop::Quit(int exit_code) {
    exit_code_ = exit_code;
    running_ = false;
    PostQuitMessage(exit_code);
}

bool MessageLoop::PumpOnce() {
    MSG msg = {};
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            running_ = false;
            exit_code_ = static_cast<int>(msg.wParam);
            return false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        return true;
    }
    return false;
}

bool MessageLoop::IsRunning() const {
    return running_;
}
