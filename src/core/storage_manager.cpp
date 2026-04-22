#include "core/storage_manager.h"

void StorageManager::begin() {
    if (!started_) {
        prefs_.begin("SalymkaApp", false); // RW mode
        started_ = true;
    }
}

void StorageManager::end() {
    if (started_) {
        prefs_.end();
        started_ = false;
    }
}

int StorageManager::getSnakeBestScore() {
    begin();
    return prefs_.getInt("snake_best", 0);
}

void StorageManager::setSnakeBestScore(int score) {
    begin();
    int oldScore = prefs_.getInt("snake_best", 0);
    if (score > oldScore) {
        prefs_.putInt("snake_best", score);
    }
}

uint16_t StorageManager::getPongCompletedMask() {
    begin();
    return prefs_.getUShort("pong_done", 0);
}

void StorageManager::setPongCompletedMask(uint16_t mask) {
    begin();
    prefs_.putUShort("pong_done", mask);
}


int StorageManager::getShooterBestScore() {
    begin();
    return prefs_.getInt("shoot_best", 0);
}

void StorageManager::setShooterBestScore(int score) {
    begin();
    int oldScore = prefs_.getInt("shoot_best", 0);
    if (score > oldScore) {
        prefs_.putInt("shoot_best", score);
    }
}

bool StorageManager::isPongLevelCompleted(uint8_t levelIndex) {
    if (levelIndex >= 16) return false;

    uint16_t mask = getPongCompletedMask();
    return (mask & (1 << levelIndex)) != 0;
}

void StorageManager::setPongLevelCompleted(uint8_t levelIndex, bool completed) {
    if (levelIndex >= 16) return;

    uint16_t mask = getPongCompletedMask();

    if (completed) {
        mask |= (1 << levelIndex);
    } else {
        mask &= ~(1 << levelIndex);
    }

    setPongCompletedMask(mask);
}