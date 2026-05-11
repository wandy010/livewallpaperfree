#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

class MessageLoop {
public:
    MessageLoop() = default;

    int Run();
    void Quit(int exit_code = 0);

    bool PumpOnce();
    bool IsRunning() const;

private:
    bool running_ = false;
    int exit_code_ = 0;
};
