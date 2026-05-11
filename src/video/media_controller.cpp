#include "media_controller.h"
#include "mf_player.h"
#include "utils/logger.h"

MediaController::MediaController(VLCPlayer& player)
    : player_(player) {}

void MediaController::PlayPause() {
    if (player_.IsPlaying()) {
        player_.Pause();
    } else {
        player_.Play();
    }
}

void MediaController::Play() {
    player_.Play();
}

void MediaController::Pause() {
    player_.Pause();
}

void MediaController::Stop() {
    player_.Stop();
}

void MediaController::Seek(float fraction) {
    Logger::debug("Seek to " + std::to_string(fraction));
    // VLCPlayer doesn't expose seek directly yet, but it can be added
}

void MediaController::SeekSeconds(float seconds) {
    Logger::debug("Seek to " + std::to_string(seconds) + "s");
}

bool MediaController::IsPlaying() const {
    return player_.IsPlaying();
}

float MediaController::Position() const {
    return player_.Position();
}

float MediaController::Duration() const {
    return player_.Duration();
}

void MediaController::SetLooping(bool loop) {
    player_.SetLooping(loop);
}

bool MediaController::IsLooping() const {
    return player_.IsLooping();
}

bool MediaController::LoadFile(const std::string& path) {
    Logger::info("Loading media: " + path);
    return player_.LoadMedia(path);
}
