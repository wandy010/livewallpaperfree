#pragma once

#include <string>
#include <functional>
#include <windows.h>

// Forward declaration of the mpv handle to avoid including the header everywhere
typedef struct mpv_handle_t* mpv_handle;

class MpvPlayer {
public:
    using EventCallback = std::function<void()>;

    MpvPlayer();
    ~MpvPlayer();

    bool Initialize();
    void Shutdown();

    bool LoadMedia(const std::string& file_path);
    void SetOutputWindow(void* hwnd);

    bool Play();
    void Pause();
    void Stop();
    bool IsPlaying() const;

    void SetLooping(bool loop);
    bool IsLooping() const;

    float Position() const;
    float Duration() const;

    void SetOnEndReached(EventCallback callback);
    
    // Audio control methods
    void SetVolume(int volume);
    int GetVolume() const;
    void SetMute(bool mute);
    bool IsMuted() const;

private:
    mpv_handle handle_ = nullptr;
    HWND output_hwnd_ = nullptr;
    bool looping_ = false;
    int volume_ = 100;
    bool muted_ = false;
    EventCallback on_end_reached_;

    void ProcessEvents();
};
