#pragma once
#include "core/audio_songs.h"
#include <Arduino.h>

class AudioManager {
public:
    void begin();
    void update();

    void click();
    void play(uint16_t freq, uint16_t durationMs);
    void playSong(const Album::Song& song);
    void stop();

    bool isPlaying() const { return playing_; }
    bool isSongPlaying() const { return currentSong_ != nullptr; }

private:
    void playCurrentSongNote();

    bool playing_ = false;
    uint32_t toneEndMs_ = 0;

    const Album::Song* currentSong_ = nullptr;
    uint16_t currentNoteIndex_ = 0;
};