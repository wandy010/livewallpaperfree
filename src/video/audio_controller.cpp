#include "audio_controller.h"
#include "mpv_player.h"
#include "utils/logger.h"

#include <algorithm>

AudioController::AudioController(MpvPlayer& player)
    : player_(player) {}

void AudioController::SetVolume(int volume) {
    volume = std::clamp(volume, 0, 100);
    last_volume_ = volume;

    player_.SetVolume(volume);
    Logger::info("Volume set to " + std::to_string(volume));
}

int AudioController::GetVolume() const {
    return player_.GetVolume();
    return last_volume_;
}

void AudioController::SetMute(bool mute) {
    if (mute) {
        last_volume_ = GetVolume();
    }

    player_.SetMute(mute);
    Logger::info(std::string("Mute ") + (mute ? "ON" : "OFF"));
}

bool AudioController::IsMuted() const {
    return player_.IsMuted();
    return false;
}

void AudioController::ToggleMute() {
    SetMute(!IsMuted());
}
