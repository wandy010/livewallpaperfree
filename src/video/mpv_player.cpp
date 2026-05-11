#include "mpv_player.h"
#include "utils/logger.h"

#include <mpv/client.h>
#include <algorithm>
#include <iostream>

MpvPlayer::MpvPlayer() {}

MpvPlayer::~MpvPlayer() {
    Shutdown();
}

bool MpvPlayer::Initialize() {
    handle_ = mpv_create();
    if (!handle_) {
        Logger::error("mpv_create failed");
        return false;
    }

    // Initialize mpv options
    // We use "wid" to attach mpv to our window
    // We use "vo=gpu" for hardware acceleration on Windows
    mpv_set_option_string(handle_, "vo", "gpu");
    mpv_set_option_string(handle_, "hwdec", "auto");
    
    // Initialize mpv
    if (mpv_initialize(handle_) == mpv_ERROR_C_FAILED) {
        Logger::error("mpv_initialize failed");
        mpv_terminate_destroy(handle_);
        handle_ = nullptr;
        return false;
    }

    Logger::info("mpv player initialized");
    return true;
}

void MpvPlayer::Shutdown() {
    if (handle_) {
        mpv_terminate_destroy(handle_);
        handle_ = nullptr;
    }
    Logger::info("mpv player shutdown");
}

bool MpvPlayer::LoadMedia(const std::string& file_path) {
    if (!handle_) return false;

    // Command to load file: loadfile "path"
    std::string cmd = "loadfile \"" + file_path + "\"";
    if (mpv_command_string(handle_, cmd.c_str()) < 0) {
        Logger::error("mpv: failed to load media: " + file_path);
        return false;
    }

    Logger::info("Media loaded (mpv): " + file_path);
    return true;
}

void MpvPlayer::SetOutputWindow(void* hwnd) {
    if (handle_ && hwnd) {
        output_hwnd_ = (HWND)hwnd;
        // mpv expects the window ID as a string for the 'wid' option
        std::string wid_str = std::to_string((uintptr_t)hwnd);
        mpv_set_option_string(handle_, "wid", wid_str.c_str());
    }
}

bool MpvPlayer::Play() {
    if (!handle_) return false;
    mpv_command_string(handle_, "set property pause no");
    Logger::info("mpv Playback started");
    return true;
}

void MpvPlayer::Pause() {
    if (handle_) {
        mpv_command_string(handle_, "set property pause yes");
        Logger::info("mpv Playback paused");
    }
}

void MpvPlayer::Stop() {
    if (handle_) {
        mpv_command_string(handle_, "stop");
        Logger::info("mpv Playback stopped");
    }
}

bool MpvPlayer::IsPlaying() const {
    if (!handle_) return false;
    
    int pause = 0;
    if (mpv_get_property(handle_, "pause", MPV_FORMAT_INT, &pause) == mpv_ERROR_C_OK) {
        return pause == 0;
    }
    return false;
}

void MpvPlayer::SetLooping(bool loop) {
    if (!handle_) return;
    looping_ = loop;
    mpv_set_option_string(handle_, "loop-file", loop ? "inf" : "no");
    Logger::info(std::string("mpv Looping ") + (loop ? "enabled" : "disabled"));
}

bool MpvPlayer::IsLooping() const {
    return looping_;
}

float MpvPlayer::Position() const {
    if (!handle_) return 0.0f;
    double pos = 0.0;
    if (mpv_get_property(handle_, "time-pos", MPV_FORMAT_DOUBLE, &pos) == mpv_ERROR_C_OK) {
        return static_cast<float>(pos);
    }
    return 0.0f;
}

float MpvPlayer::Duration() const {
    if (!handle_) return 0.0f;
    double dur = 0.0;
    if (mpv_get_property(handle_, "duration", MPV_FORMAT_DOUBLE, &dur) == mpv_ERROR_C_OK) {
        return static_cast<float>(dur);
    }
    return 0.0f;
}

void MpvPlayer::SetOnEndReached(EventCallback callback) {
    on_end_reached_ = std::move(callback);
    // Note: To properly implement on_end_reached in mpv, we would need 
    // to set up mpv_set_wakeup_callback or poll for property changes.
    // For this structural migration, we keep the interface.
}

void MpvPlayer::SetVolume(int volume) {
    if (!handle_) return;
    volume_ = std::clamp(volume, 0, 100);
    double vol_double = volume_ / 100.0;
    mpv_set_property(handle_, "volume", MPV_FORMAT_DOUBLE, &vol_double);
}

int MpvPlayer::GetVolume() const {
    if (!handle_) return volume_;
    double vol_double = 0.0;
    if (mpv_get_property(handle_, "volume", MPV_FORMAT_DOUBLE, &vol_double) == mpv_ERROR_C_OK) {
        return static_cast<int>(vol_double * 100.0);
    }
    return volume_;
}

void MpvPlayer::SetMute(bool mute) {
    if (!handle_) return;
    muted_ = mute;
    int mute_val = mute ? 1 : 0;
    mpv_set_property(handle_, "mute", MPV_FORMAT_INT, &mute_val);
}

bool MpvPlayer::IsMuted() const {
    if (!handle_) return muted_;
    int mute_val = 0;
    if (mpv_get_property(handle_, "mute", MPV_FORMAT_INT, &mute_val) == mpv_ERROR_C_OK) {
        return mute_val != 0;
    }
    return muted_;
}
