#include "mpv_player.h"
#include "utils/logger.h"

#include <algorithm>
#include <iostream>

MpvPlayer::MpvPlayer() {}

MpvPlayer::~MpvPlayer() {
    Shutdown();
}

bool MpvPlayer::LoadFunctions() {
    const char* dll_names[] = { "libmpv-2.dll", "mpv-1.dll", "libmpv.dll" };
    
    for (const char* name : dll_names) {
        Logger::info("Attempting to load mpv DLL: " + std::string(name));
        mpv_dll_ = LoadLibraryA(name);
        if (mpv_dll_) {
            Logger::info("Successfully loaded DLL: " + std::string(name));
            break;
        }
    }

    if (!mpv_dll_) {
        DWORD error = GetLastError();
        Logger::error("Could not load any mpv DLL. Windows Error Code: " + std::to_string(error));
        return false;
    }

    struct {
        const char* name;
        void** ptr;
    } functions[] = {
        {"mpv_create", (void**)&_mpv_create},
        {"mpv_initialize", (void**)&_mpv_initialize},
        {"mpv_terminate_destroy", (void**)&_mpv_terminate_destroy},
        {"mpv_set_option_string", (void**)&_mpv_set_option_string},
        {"mpv_command_string", (void**)&_mpv_command_string},
        {"mpv_get_property", (void**)&_mpv_get_property},
        {"mpv_set_property", (void**)&_mpv_set_property}
    };

    for (const auto& func : functions) {
        void* addr = GetProcAddress(mpv_dll_, func.name);
        if (!addr) {
            Logger::error("Failed to find function " + std::string(func.name) + " in DLL. Error: " + std::to_string(GetLastError()));
            FreeLibrary(mpv_dll_);
            mpv_dll_ = nullptr;
            return false;
        }
        *func.ptr = addr;
    }

    return true;
}

bool MpvPlayer::Initialize() {
    if (!LoadFunctions()) return false;

    mpv_handle_ = _mpv_create();
    if (!mpv_handle_) {
        Logger::error("mpv_create failed");
        return false;
    }

    _mpv_set_option_string(mpv_handle_, "vo", "gpu");
    _mpv_set_option_string(mpv_handle_, "hwdec", "auto");
    
    if (_mpv_initialize(mpv_handle_) < 0) {
        Logger::error("mpv_initialize failed");
        _mpv_terminate_destroy(mpv_handle_);
        mpv_handle_ = nullptr;
        return false;
    }

    Logger::info("mpv player initialized (dynamically)");
    return true;
}

void MpvPlayer::Shutdown() {
    if (mpv_handle_) {
        _mpv_terminate_destroy(mpv_handle_);
        mpv_handle_ = nullptr;
    }
    if (mpv_dll_) {
        FreeLibrary(mpv_dll_);
        mpv_dll_ = nullptr;
    }
    Logger::info("mpv player shutdown");
}

bool MpvPlayer::LoadMedia(const std::string& file_path) {
    if (!mpv_handle_) return false;

    std::string cmd = "loadfile \"" + file_path + "\"";
    if (_mpv_command_string(mpv_handle_, cmd.c_str()) < 0) {
        Logger::error("mpv: failed to load media: " + file_path);
        return false;
    }

    Logger::info("Media loaded (mpv): " + file_path);
    return true;
}

void MpvPlayer::SetOutputWindow(void* hwnd) {
    if (mpv_handle_ && hwnd) {
        output_hwnd_ = (HWND)hwnd;
        std::string wid_str = std::to_string((uintptr_t)hwnd);
        _mpv_set_option_string(mpv_handle_, "wid", wid_str.c_str());
    }
}

bool MpvPlayer::Play() {
    if (!mpv_handle_) return false;
    _mpv_command_string(mpv_handle_, "set property pause no");
    Logger::info("mpv Playback started");
    return true;
}

void MpvPlayer::Pause() {
    if (mpv_handle_) {
        _mpv_command_string(mpv_handle_, "set property pause yes");
        Logger::info("mpv Playback paused");
    }
}

void MpvPlayer::Stop() {
    if (mpv_handle_) {
        _mpv_command_string(mpv_handle_, "stop");
        Logger::info("mpv Playback stopped");
    }
}

bool MpvPlayer::IsPlaying() const {
    if (!mpv_handle_) return false;
    
    int pause = 0;
    if (_mpv_get_property(mpv_handle_, "pause", 1, &pause) == 0) { // 1 is MPV_FORMAT_INT
        return pause == 0;
    }
    return false;
}

void MpvPlayer::SetLooping(bool loop) {
    if (!mpv_handle_) return;
    looping_ = loop;
    _mpv_set_option_string(mpv_handle_, "loop-file", loop ? "inf" : "no");
    Logger::info(std::string("mpv Looping ") + (loop ? "enabled" : "disabled"));
}

bool MpvPlayer::IsLooping() const {
    return looping_;
}

float MpvPlayer::Position() const {
    if (!mpv_handle_) return 0.0f;
    double pos = 0.0;
    if (_mpv_get_property(mpv_handle_, "time-pos", 2, &pos) == 0) { // 2 is MPV_FORMAT_DOUBLE
        return static_cast<float>(pos);
    }
    return 0.0f;
}

float MpvPlayer::Duration() const {
    if (!mpv_handle_) return 0.0f;
    double dur = 0.0;
    if (_mpv_get_property(mpv_handle_, "duration", 2, &dur) == 0) {
        return static_cast<float>(dur);
    }
    return 0.0f;
}

void MpvPlayer::SetOnEndReached(EventCallback callback) {
    on_end_reached_ = std::move(callback);
}

void MpvPlayer::SetVolume(int volume) {
    if (!mpv_handle_) return;
    volume_ = std::clamp(volume, 0, 100);
    double vol_double = volume_ / 100.0;
    _mpv_set_property(mpv_handle_, "volume", 2, &vol_double); // 2 is MPV_FORMAT_DOUBLE
}

int MpvPlayer::GetVolume() const {
    if (!mpv_handle_) return volume_;
    double vol_double = 0.0;
    if (_mpv_get_property(mpv_handle_, "volume", 2, &vol_double) == 0) {
        return static_cast<int>(vol_double * 100.0);
    }
    return volume_;
}

void MpvPlayer::SetMute(bool mute) {
    if (!mpv_handle_) return;
    muted_ = mute;
    int mute_val = mute ? 1 : 0;
    _mpv_set_property(mpv_handle_, "mute", 1, &mute_val); // 1 is MPV_FORMAT_INT
}

bool MpvPlayer::IsMuted() const {
    if (!mpv_handle_) return muted_;
    int mute_val = 0;
    if (_mpv_get_property(mpv_handle_, "mute", 1, &mute_val) == 0) {
        return mute_val != 0;
    }
    return muted_;
}
