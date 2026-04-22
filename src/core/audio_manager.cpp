#include "core/audio_manager.h"
#include "config.h"

void AudioManager::begin() {
    ledcSetup(Config::BUZZER_CHANNEL, Config::BUZZER_FREQ_UI, Config::BUZZER_VOLUME_BITS);
    ledcAttachPin(Config::PIN_BUZZER, Config::BUZZER_CHANNEL);
    ledcWriteTone(Config::BUZZER_CHANNEL, 0);

    playing_ = false;
    toneEndMs_ = 0;

    currentSong_ = nullptr;
    currentNoteIndex_ = 0;
}

void AudioManager::update() {
    if (!playing_) return;

    if (millis() >= toneEndMs_) {
        if (currentSong_ != nullptr) {
            currentNoteIndex_++;

            if (currentNoteIndex_ >= currentSong_->length) {
                stop();
            } else {
                playCurrentSongNote();
            }
        } else {
            stop();
        }
    }
}

void AudioManager::click() {
    play(1800, 20);
}

void AudioManager::play(uint16_t freq, uint16_t durationMs) {
    currentSong_ = nullptr;
    currentNoteIndex_ = 0;

    if (freq == 0) {
        ledcWriteTone(Config::BUZZER_CHANNEL, 0);
    } else {
        ledcWriteTone(Config::BUZZER_CHANNEL, freq);
    }

    toneEndMs_ = millis() + durationMs;
    playing_ = true;
}

void AudioManager::playSong(const Album::Song& song) {
    if (song.notes == nullptr || song.length == 0) {
        stop();
        return;
    }

    currentSong_ = &song;
    currentNoteIndex_ = 0;
    playCurrentSongNote();
}

void AudioManager::playCurrentSongNote() {
    if (currentSong_ == nullptr || currentNoteIndex_ >= currentSong_->length) {
        stop();
        return;
    }

    const Album::Note& note = currentSong_->notes[currentNoteIndex_];

    if (note.freq == 0) {
        ledcWriteTone(Config::BUZZER_CHANNEL, 0);
    } else {
        ledcWriteTone(Config::BUZZER_CHANNEL, note.freq);
    }

    toneEndMs_ = millis() + note.durationMs;
    playing_ = true;
}

void AudioManager::stop() {
    ledcWriteTone(Config::BUZZER_CHANNEL, 0);
    playing_ = false;
    currentSong_ = nullptr;
    currentNoteIndex_ = 0;
}