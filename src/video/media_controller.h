#pragma once

#include <string>

class MpvPlayer;

class MediaController {
public:
    explicit MediaController(MpvPlayer& player);

    void PlayPause();
    void Play();
    void Pause();
    void Stop();

    void Seek(float fraction);
    void SeekSeconds(float seconds);

    bool IsPlaying() const;
    float Position() const;
    float Duration() const;

    void SetLooping(bool loop);
    bool IsLooping() const;

    bool LoadFile(const std::string& path);

private:
    MpvPlayer& player_;
};
