#pragma once

#include <string>
#include <functional>
#include <windows.h>
#include <vector>

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
    
    void SetVolume(int volume);
    int GetVolume() const;
    void SetMute(bool mute);
    bool IsMuted() const;

private:
    // MPV Handle
    void* mpv_handle_ = nullptr;
    HMODULE mpv_dll_ = nullptr;

    // Function Pointers for Dynamic Loading
    typedef void* (*mpv_create_t)();
    typedef int (*mpv_initialize_t)(void*);
    typedef void (*mpv_terminate_destroy_t)(void*);
    typedef int (*mpv_set_option_string_t)(void*, const char*, const char*);
    typedef int (*mpv_command_string_t)(void*, const char*);
    typedef int (*mpv_get_property_t)(void*, const char*, int, void*);
    typedef int (*mpv_set_property_t)(void*, const char*, int, const void*);

    mpv_create_t _mpv_create = nullptr;
    mpv_initialize_t _mpv_initialize = nullptr;
    mpv_terminate_destroy_t _mpv_terminate_destroy = nullptr;
    mpv_set_option_string_t _mpv_set_option_string = nullptr;
    mpv_command_string_t _mpv_command_string = nullptr;
    mpv_get_property_t _mpv_get_property = nullptr;
    mpv_set_property_t _mpv_set_property = nullptr;

    HWND output_hwnd_ = nullptr;
    bool looping_ = false;
    int volume_ = 100;
    bool muted_ = false;
    EventCallback on_end_reached_;

    bool LoadFunctions();
};
