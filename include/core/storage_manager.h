#pragma once

#include <Arduino.h>
#include <Preferences.h>

class StorageManager {
public:
    void begin();
    void end();

    int getSnakeBestScore();
    void setSnakeBestScore(int score);

    uint16_t getPongCompletedMask();
    void setPongCompletedMask(uint16_t mask);

    int getShooterBestScore();
    void setShooterBestScore(int score);

    bool isPongLevelCompleted(uint8_t levelIndex);
    void setPongLevelCompleted(uint8_t levelIndex, bool completed = true);

private:
    Preferences prefs_;
    bool started_ = false;
};