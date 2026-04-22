#pragma once
#include "apps/i_app.h"
#include <Arduino.h>

class SnakeGameApp : public IApp {
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

    enum class Direction : uint8_t {
        Up = 0,
        Down,
        Left,
        Right
    };

    struct Cell {
        int8_t x = 0;
        int8_t y = 0;
    };

    static constexpr uint8_t SCREEN_W = 128;
    static constexpr uint8_t SCREEN_H = 64;

    static constexpr uint8_t HUD_H = 10;
    static constexpr uint8_t CELL_SIZE = 6;
    static constexpr uint8_t GRID_W = 20;
    static constexpr uint8_t GRID_H = 9;
    static constexpr uint8_t FIELD_W = GRID_W * CELL_SIZE;
    static constexpr uint8_t FIELD_H = GRID_H * CELL_SIZE;
    static constexpr uint8_t FIELD_X = (SCREEN_W - FIELD_W) / 2;
    static constexpr uint8_t FIELD_Y = HUD_H;

    static constexpr uint8_t MAX_SNAKE_LEN = GRID_W * GRID_H;

    State state_ = State::Start;
    Direction dir_ = Direction::Right;
    Direction nextDir_ = Direction::Right;

    Cell snake_[MAX_SNAKE_LEN]{};
    uint8_t snakeLen_ = 0;
    Cell food_{};


    uint16_t score_ = 0;
    uint16_t bestScore_ = 0;

    uint32_t lastStepMs_ = 0;
    uint16_t stepDelayMs_ = 220;
    uint32_t gameOverAtMs_ = 0;

    void resetGame();
    void spawnFood();
    bool isSnakeAt(int8_t x, int8_t y) const;
    bool isOpposite(Direction a, Direction b) const;

    void updateStart(AppManager& manager);
    void updatePlaying(AppManager& manager);
    void updateGameOver(AppManager& manager);

    void applyInput(AppManager& manager);
    void stepSnake(AppManager& manager);

    void renderStart(AppManager& manager);
    void renderPlaying(AppManager& manager);
    void renderGameOver(AppManager& manager);
    void renderField(AppManager& manager, bool blinkFood);
    void drawCentered(class U8G2& d, const char* text, int y);
};
