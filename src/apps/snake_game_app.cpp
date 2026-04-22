#include "apps/snake_game_app.h"
#include "core/app_manager.h"
#include "core/input_manager.h"
#include "core/audio_manager.h"
#include <stdio.h>

void SnakeGameApp::onEnter(AppManager& app) {
    bestScore_ = app.storage().getSnakeBestScore();
    resetGame();
    state_ = State::Start;
}

void SnakeGameApp::onExit() {
}

void SnakeGameApp::update(AppManager& manager) {
    switch (state_) {
        case State::Start:
            updateStart(manager);
            break;
        case State::Playing:
            updatePlaying(manager);
            break;
        case State::GameOver:
            updateGameOver(manager);
            break;
    }
}

void SnakeGameApp::render(AppManager& manager) {
    switch (state_) {
        case State::Start:
            renderStart(manager);
            break;
        case State::Playing:
            renderPlaying(manager);
            break;
        case State::GameOver:
            renderGameOver(manager);
            break;
    }
}

void SnakeGameApp::resetGame() {
    score_ = 0;
    stepDelayMs_ = 220;
    dir_ = Direction::Right;
    nextDir_ = Direction::Right;

    snakeLen_ = 3;
    snake_[0].x = 10;
    snake_[0].y = 4;
    snake_[1].x = 9;
    snake_[1].y = 4;
    snake_[2].x = 8;
    snake_[2].y = 4;

    spawnFood();
    lastStepMs_ = millis();
}

void SnakeGameApp::spawnFood() {
    if (snakeLen_ >= MAX_SNAKE_LEN) {
        food_.x = -1;
        food_.y = -1;
        return;
    }

    while (true) {
        int8_t x = random(0, GRID_W);
        int8_t y = random(0, GRID_H);
        if (!isSnakeAt(x, y)) {
            food_.x = x;
            food_.y = y;    
            return;
        }
    }
}

bool SnakeGameApp::isSnakeAt(int8_t x, int8_t y) const {
    for (uint8_t i = 0; i < snakeLen_; ++i) {
        if (snake_[i].x == x && snake_[i].y == y) {
            return true;
        }
    }
    return false;
}

bool SnakeGameApp::isOpposite(Direction a, Direction b) const {
    return (a == Direction::Up && b == Direction::Down) ||
           (a == Direction::Down && b == Direction::Up) ||
           (a == Direction::Left && b == Direction::Right) ||
           (a == Direction::Right && b == Direction::Left);
}

void SnakeGameApp::updateStart(AppManager& manager) {
    auto& input = manager.input();

    if (input.pressed(ButtonId::Exit)) {
        manager.audio().click();
        manager.switchTo(AppId::GamesMenu);
        return;
    }

    if (input.pressed(ButtonId::Ok) || input.pressed(ButtonId::Up) ||
        input.pressed(ButtonId::Down) || input.pressed(ButtonId::Left) ||
        input.pressed(ButtonId::Right)) {
        manager.audio().click();
        state_ = State::Playing;
        lastStepMs_ = millis();
        applyInput(manager);
    }
}

void SnakeGameApp::applyInput(AppManager& manager) {
    auto& input = manager.input();

    Direction desired = nextDir_;

    if (input.pressed(ButtonId::Up)) desired = Direction::Up;
    else if (input.pressed(ButtonId::Down)) desired = Direction::Down;
    else if (input.pressed(ButtonId::Left)) desired = Direction::Left;
    else if (input.pressed(ButtonId::Right)) desired = Direction::Right;

    if (snakeLen_ <= 1 || !isOpposite(dir_, desired)) {
        nextDir_ = desired;
    }
}

void SnakeGameApp::stepSnake(AppManager& manager) {
    dir_ = nextDir_;

    Cell newHead = snake_[0];
    switch (dir_) {
        case Direction::Up:    newHead.y--; break;
        case Direction::Down:  newHead.y++; break;
        case Direction::Left:  newHead.x--; break;
        case Direction::Right: newHead.x++; break;
    }

    if (newHead.x < 0 || newHead.x >= GRID_W || newHead.y < 0 || newHead.y >= GRID_H) {
        manager.audio().play(240, 220);
        state_ = State::GameOver;
        gameOverAtMs_ = millis();

        if (score_ > bestScore_) {
            bestScore_ = score_;
            manager.storage().setSnakeBestScore(bestScore_);
        }

        return;
    }

    const bool ateFood = (newHead.x == food_.x && newHead.y == food_.y);
    const uint8_t collisionLen = ateFood ? snakeLen_ : (snakeLen_ > 0 ? snakeLen_ - 1 : 0);

    for (uint8_t i = 0; i < collisionLen; ++i) {
        if (snake_[i].x == newHead.x && snake_[i].y == newHead.y) {
            manager.audio().play(240, 220);
            state_ = State::GameOver;
            gameOverAtMs_ = millis();

            if (score_ > bestScore_) {
                bestScore_ = score_;
                manager.storage().setSnakeBestScore(bestScore_);
            }

            return;
        }
    }

    if (ateFood && snakeLen_ < MAX_SNAKE_LEN) snakeLen_++;

    for (int i = snakeLen_ - 1; i > 0; --i) {
        snake_[i] = snake_[i - 1];
    }
    snake_[0] = newHead;

    if (ateFood) {
        score_++;
        if (score_ > bestScore_) {
            bestScore_ = score_;
            manager.storage().setSnakeBestScore(bestScore_);
        }

        if (stepDelayMs_ > 90) {
            stepDelayMs_ -= 6;
        }

        manager.audio().play(1600, 40);
        if (snakeLen_ < MAX_SNAKE_LEN) {
            spawnFood();
        }
    }
}

void SnakeGameApp::updatePlaying(AppManager& manager) {
    auto& input = manager.input();

    if (input.pressed(ButtonId::Exit)) {
        manager.audio().click();
        manager.switchTo(AppId::GamesMenu);
        return;
    }

    applyInput(manager);

    const uint32_t now = millis();
    if ((uint32_t)(now - lastStepMs_) >= stepDelayMs_) {
        lastStepMs_ = now;
        stepSnake(manager);
    }
}

void SnakeGameApp::updateGameOver(AppManager& manager) {
    auto& input = manager.input();

    if (input.pressed(ButtonId::Exit)) {
        manager.audio().click();
        manager.switchTo(AppId::GamesMenu);
        return;
    }

    if (input.pressed(ButtonId::Ok)) {
        manager.audio().click();
        resetGame();
        state_ = State::Start;
        return;
    }

    if ((int32_t)(millis() - gameOverAtMs_) > 5000) {
        resetGame();
        state_ = State::Start;
    }
}

void SnakeGameApp::drawCentered(U8G2& d, const char* text, int y) {
    const int w = d.getStrWidth(text);
    d.drawStr((SCREEN_W - w) / 2, y, text);
}

void SnakeGameApp::renderField(AppManager& manager, bool blinkFood) {
    auto& d = manager.display().oled();

    d.drawFrame(FIELD_X - 1, FIELD_Y - 1, FIELD_W + 2, FIELD_H + 1);

    for (uint8_t i = 1; i < snakeLen_; ++i) {
        int px = FIELD_X + snake_[i].x * CELL_SIZE + 1;
        int py = FIELD_Y + snake_[i].y * CELL_SIZE + 1;
        d.drawBox(px, py, CELL_SIZE - 1, CELL_SIZE - 1);
    }

    int hx = FIELD_X + snake_[0].x * CELL_SIZE + 1;
    int hy = FIELD_Y + snake_[0].y * CELL_SIZE + 1;
    d.drawFrame(hx, hy, CELL_SIZE - 1, CELL_SIZE - 1);
    d.drawPixel(hx + 1, hy + 1);
    d.drawPixel(hx + CELL_SIZE - 3, hy + 1);

    if (food_.x >= 0 && food_.y >= 0 && blinkFood) {
        int fx = FIELD_X + food_.x * CELL_SIZE + 2;
        int fy = FIELD_Y + food_.y * CELL_SIZE + 2;
        d.drawBox(fx, fy, CELL_SIZE - 3, CELL_SIZE - 3);
    }
}

void SnakeGameApp::renderStart(AppManager& manager) {
    auto& d = manager.display().oled();
    d.setFont(u8g2_font_6x12_tf);

    drawCentered(d, "Snake", 9);
    d.drawHLine(0, 11, 128);

    char line[24];
    snprintf(line, sizeof(line), "Best:%u", bestScore_);
    d.drawStr(4, 22, line);
    d.drawStr(4, 34, "Arrows = move");
    d.drawStr(4, 46, "OK = start");
    d.drawStr(4, 58, "EXIT = back");

    d.drawFrame(90, 18, 26, 26);
    d.drawBox(95, 29, 5, 5);
    d.drawBox(100, 29, 5, 5);
    d.drawFrame(105, 29, 5, 5);
    d.drawBox(110, 29, 5, 5);
    d.drawBox(110, 24, 5, 5);
}

void SnakeGameApp::renderPlaying(AppManager& manager) {
    auto& d = manager.display().oled();
    d.setFont(u8g2_font_6x12_tf);

    char hud[32];
    snprintf(hud, sizeof(hud), "S:%u  B:%u", score_, bestScore_);
    d.drawStr(2, 8, hud);

    renderField(manager, ((millis() / 180) % 2) == 0);
}

void SnakeGameApp::renderGameOver(AppManager& manager) {
    auto& d = manager.display().oled();
    d.setFont(u8g2_font_6x12_tf);

    renderField(manager, true);

    d.drawBox(22, 18, 84, 28);
    d.setDrawColor(0);
    drawCentered(d, "GAME OVER", 30);

    char line[24];
    snprintf(line, sizeof(line), "Score:%u", score_);
    drawCentered(d, line, 42);
    d.setDrawColor(1);
}
