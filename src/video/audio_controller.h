#pragma once

class MFPlayer;

class AudioController {
public:
    explicit AudioController(MFPlayer& player);

    void SetVolume(int volume);
    int GetVolume() const;

    void SetMute(bool mute);
    bool IsMuted() const;
    void ToggleMute();

private:
    MFPlayer& player_;
    int last_volume_ = 30;
};
