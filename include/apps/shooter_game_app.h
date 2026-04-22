#pragma once
#include "apps/i_app.h"
#include <Arduino.h>

class ShooterGameApp : public IApp {
public:
    void onEnter(AppManager& manager) override;
    void onExit() override;
    void update(AppManager& manager) override;
    void render(AppManager& manager) override;

private:
    enum class State : uint8_t {
        Start = 0,
        Playing,
        GameOver
    };

    struct Bullet {
        int16_t x = 0;
        int16_t y = 0;
        bool active = false;
    };

    struct Enemy {
        int16_t x = 0;
        int16_t y = 0;
        int8_t speed = 1;
        bool active = false;
    };

    static constexpr uint8_t SCREEN_W = 128;
    static constexpr uint8_t SCREEN_H = 64;
    static constexpr uint8_t HUD_H = 10;
    static constexpr uint8_t FIELD_Y = HUD_H;
    static constexpr uint8_t FIELD_H = SCREEN_H - HUD_H;

    static constexpr uint8_t PLAYER_W = 11;
    static constexpr uint8_t PLAYER_H = 8;
    static constexpr uint8_t PLAYER_Y = SCREEN_H - 10;

    static constexpr uint8_t MAX_BULLETS = 4;
    static constexpr uint8_t MAX_ENEMIES = 5;

    State state_ = State::Start;
    int16_t playerX_ = 58;
    Bullet bullets_[MAX_BULLETS]{};
    Enemy enemies_[MAX_ENEMIES]{};

    uint16_t score_ = 0;
    uint16_t bestScore_ = 0;

    uint32_t lastFireMs_ = 0;
    uint32_t lastSpawnMs_ = 0;
    uint32_t lastStepMs_ = 0;
    uint32_t gameOverAtMs_ = 0;
    uint16_t enemySpawnDelayMs_ = 700;

    void resetGame();
    void updateStart(AppManager& manager);
    void updatePlaying(AppManager& manager);
    void updateGameOver(AppManager& manager);

    void fire(AppManager& manager);
    void spawnEnemy();
    void stepGame(AppManager& manager);
    void triggerGameOver(AppManager& manager);
    void drawCentered(class U8G2& d, const char* text, int y);
    void drawPlayer(class U8G2& d, int x, int y, bool thrust);
    void drawEnemy(class U8G2& d, int x, int y, uint8_t frame);
};
