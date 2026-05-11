#include "config_manager.h"
#include "json_loader.h"
#include "utils/logger.h"

#include <string>
#include <algorithm>

ConfigManager::ConfigManager() {
    data_ = nlohmann::json::object();
}

bool ConfigManager::load(const std::string& path) {
    file_path_ = path;
    auto json = json_loader::load_file(path);
    if (json.is_null() || json.empty()) {
        Logger::warn("Config file empty or not found, using defaults: " + path);
        data_ = {
            {"video", "assets/videos/wallpaper.mp4"},
            {"volume", 30},
            {"mute", false},
            {"pause_on_fullscreen", true},
            {"pause_when_hidden", true},
            {"fps_limit", 30}
        };
        return false;
    }
    data_ = json;
    Logger::info("Config loaded from: " + path);
    return true;
}

bool ConfigManager::save(const std::string& path) {
    return json_loader::save_file(path, data_);
}

std::string ConfigManager::video_path() const {
    return data_.value("video", std::string("assets/videos/wallpaper.mp4"));
}

void ConfigManager::set_video_path(const std::string& path) {
    data_["video"] = path;
}

int ConfigManager::volume() const {
    return data_.value("volume", 30);
}

void ConfigManager::set_volume(int volume) {
    data_["volume"] = std::clamp(volume, 0, 100);
}

bool ConfigManager::mute() const {
    return data_.value("mute", false);
}

void ConfigManager::set_mute(bool mute) {
    data_["mute"] = mute;
}

bool ConfigManager::pause_on_fullscreen() const {
    return data_.value("pause_on_fullscreen", true);
}

void ConfigManager::set_pause_on_fullscreen(bool pause) {
    data_["pause_on_fullscreen"] = pause;
}

bool ConfigManager::pause_when_hidden() const {
    return data_.value("pause_when_hidden", true);
}

void ConfigManager::set_pause_when_hidden(bool pause) {
    data_["pause_when_hidden"] = pause;
}

int ConfigManager::fps_limit() const {
    return data_.value("fps_limit", 30);
}

void ConfigManager::set_fps_limit(int fps) {
    data_["fps_limit"] = std::max(fps, 1);
}

const nlohmann::json& ConfigManager::raw() const {
    return data_;
}
