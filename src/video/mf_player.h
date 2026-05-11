#pragma once

#include <string>
#include <functional>
#include <windows.h>

class MFPlayer {
public:
    using EventCallback = std::function<void()>;

    MFPlayer();
    ~MFPlayer();

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
    
    // Audio control methods to replace libvlc_audio_... calls in AudioController
    void SetVolume(int volume);
    int GetVolume() const;
    void SetMute(bool mute);
    bool IsMuted() const;

private:
    void* session_ = nullptr; // IMFMediaSession*
    void* source_reader_ = nullptr; // IMFSourceReader*
    void* topology_ = nullptr; // IMFTopology*
    
    HWND output_hwnd_ = nullptr;
    bool looping_ = false;
    int volume_ = 100;
    bool muted_ = false;
    EventCallback on_end_reached_;

    // Helper to handle MF events
    static void OnMediaEvent(void* session, void* user_data);
};
