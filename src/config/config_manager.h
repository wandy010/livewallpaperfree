#pragma once

#include <string>
#include <nlohmann/json.hpp>

class ConfigManager {
public:
    ConfigManager();

    bool load(const std::string& path);
    bool save(const std::string& path);

    std::string video_path() const;
    void set_video_path(const std::string& path);

    int volume() const;
    void set_volume(int volume);

    bool mute() const;
    void set_mute(bool mute);

    bool pause_on_fullscreen() const;
    void set_pause_on_fullscreen(bool pause);

    bool pause_when_hidden() const;
    void set_pause_when_hidden(bool pause);

    int fps_limit() const;
    void set_fps_limit(int fps);

    const nlohmann::json& raw() const;

private:
    nlohmann::json data_;
    std::string file_path_;
};
