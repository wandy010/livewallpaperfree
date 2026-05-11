#pragma once

class MpvPlayer;

class AudioController {
public:
    explicit AudioController(MpvPlayer& player);

    void SetVolume(int volume);
    int GetVolume() const;

    void SetMute(bool mute);
    bool IsMuted() const;
    void ToggleMute();

private:
    MpvPlayer& player_;
    int last_volume_ = 30;
};
