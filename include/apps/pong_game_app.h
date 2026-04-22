#pragma once
#include <Arduino.h>
#include <U8g2lib.h>
#include "apps/i_app.h"
#include "core/input_manager.h"

class PongGameApp : public IApp {
public:
    void onEnter(AppManager& manager) override;
    void onExit() override;
    void update(AppManager& manager) override;
    void render(AppManager& manager) override;

private:
    static constexpr uint8_t LEVEL_COUNT = 20;

    enum class State : uint8_t {
        LevelSelect = 0,
        Playing,
        WinScreen
    };

    struct LevelSettings {
        float ballSpeedX;
        float ballSpeedY;
        uint8_t botSpeed;
        uint8_t scoreToWin;
        uint8_t errorChance;     // bigger = bot makes more mistakes
        uint16_t errorDurationMs;
    };

    State state_ = State::LevelSelect;

    uint8_t selectedLevel_ = 0;      // 0..19
    uint8_t maxUnlockedLevel_ = 0;   // runtime only
    bool solved_[LEVEL_COUNT] = {false};

    // field
    static constexpr int SCREEN_W = 128;
    static constexpr int SCREEN_H = 64;

    static constexpr int FIELD_X = 2;
    static constexpr int FIELD_Y = 2;
    static constexpr int FIELD_W = 124;
    static constexpr int FIELD_H = 50;

    static constexpr int PADDLE_W = 3;
    static constexpr int PADDLE_H = 14;

    static constexpr int PLAYER_X = 6;
    static constexpr int BOT_X = 119;

    static constexpr int BALL_SIZE = 3;

    // game objects
    int playerY_ = 25;
    int botY_ = 25;

    float ballX_ = 64.0f;
    float ballY_ = 32.0f;
    float ballVX_ = 1.5f;
    float ballVY_ = 0.8f;

    uint8_t playerScore_ = 0;
    uint8_t botScore_ = 0;

    uint32_t lastFrameMs_ = 0;
    uint32_t winScreenStartMs_ = 0;

    bool botErrorActive_ = false;
    uint32_t botErrorUntilMs_ = 0;
    int8_t botErrorDir_ = 0;
    uint16_t completedLevelsMask_ = 0;


    static constexpr const char* GAME_TITLE = "Super Pong";

    void updateLevelSelect(AppManager& manager);
    void updatePlaying(AppManager& manager);
    void updateWinScreen(AppManager& manager);

    void renderLevelSelect(AppManager& manager);
    void renderPlaying(AppManager& manager);
    void renderWinScreen(AppManager& manager);

    LevelSettings getLevelSettings(uint8_t levelIndex) const;

    void startLevel(uint8_t levelIndex);
    void resetRound(bool towardPlayer);
    void updatePlayer(InputManager& input);
    void updateBot(const LevelSettings& lvl, uint32_t now);
    void updateBall(const LevelSettings& lvl, AppManager& manager);
    void checkWin(AppManager& manager);

    void drawCentered(U8G2& d, const char* text, int y);
};