#include "apps/pong_game_app.h"
#include "core/app_manager.h"
#include "core/input_manager.h"
#include "core/audio_manager.h"
#include <math.h>

void PongGameApp::onEnter(AppManager& manager) {
    state_ = State::LevelSelect;

    completedLevelsMask_ = manager.storage().getPongCompletedMask();

    for (uint8_t i = 0; i < LEVEL_COUNT; i++) {
        solved_[i] = (completedLevelsMask_ & (1 << i)) != 0;
    }

    maxUnlockedLevel_ = 0;
    while (maxUnlockedLevel_ < LEVEL_COUNT - 1 && solved_[maxUnlockedLevel_]) {
        maxUnlockedLevel_++;
    }

    if (selectedLevel_ > maxUnlockedLevel_) {
        selectedLevel_ = maxUnlockedLevel_;
    }
}

void PongGameApp::onExit() {
}

void PongGameApp::update(AppManager& manager) {
    switch (state_) {
        case State::LevelSelect:
            updateLevelSelect(manager);
            break;
        case State::Playing:
            updatePlaying(manager);
            break;
        case State::WinScreen:
            updateWinScreen(manager);
            break;
    }
}

void PongGameApp::render(AppManager& manager) {
    switch (state_) {
        case State::LevelSelect:
            renderLevelSelect(manager);
            break;
        case State::Playing:
            renderPlaying(manager);
            break;
        case State::WinScreen:
            renderWinScreen(manager);
            break;
    }
}

PongGameApp::LevelSettings PongGameApp::getLevelSettings(uint8_t levelIndex) const {
    // levelIndex: 0..19 => shown as 1..20
    const uint8_t lvl = levelIndex + 1;

    LevelSettings s{};

    // Ball gets faster each level
    s.ballSpeedX = 1.30f + (0.09f * levelIndex);   // ~1.3 -> ~3.0
    s.ballSpeedY = 0.55f + (0.05f * levelIndex);   // ~0.55 -> ~1.5

    // Bot speed rises too
    s.botSpeed = 1 + (levelIndex / 3);             // 1..7

    // Score to win grows a bit
    if (lvl <= 5) s.scoreToWin = 3;
    else if (lvl <= 10) s.scoreToWin = 4;
    else if (lvl <= 15) s.scoreToWin = 5;
    else s.scoreToWin = 6;

    // Bot mistakes:
    // level 1 => many mistakes
    // level 20 => few mistakes
    s.errorChance = 28 - levelIndex;               // 28..9
    if (s.errorChance < 4) s.errorChance = 4;

    s.errorDurationMs = 240 - (levelIndex * 7);    // 240..107
    if (s.errorDurationMs < 70) s.errorDurationMs = 70;

    return s;
}

void PongGameApp::startLevel(uint8_t levelIndex) {
    selectedLevel_ = levelIndex;
    playerScore_ = 0;
    botScore_ = 0;

    playerY_ = 25;
    botY_ = 25;

    botErrorActive_ = false;
    botErrorUntilMs_ = 0;
    botErrorDir_ = 0;

    lastFrameMs_ = millis();
    resetRound(false);
    state_ = State::Playing;
}

void PongGameApp::resetRound(bool towardPlayer) {
    ballX_ = FIELD_X + FIELD_W / 2.0f;
    ballY_ = FIELD_Y + FIELD_H / 2.0f;

    LevelSettings lvl = getLevelSettings(selectedLevel_);

    ballVX_ = towardPlayer ? -lvl.ballSpeedX : lvl.ballSpeedX;
    ballVY_ = (random(0, 2) == 0 ? -lvl.ballSpeedY : lvl.ballSpeedY);
}

void PongGameApp::updateLevelSelect(AppManager& manager) {
    auto& input = manager.input();

    if (input.pressedOrRepeated(ButtonId::Up) && selectedLevel_ > 0) {
        selectedLevel_--;
        manager.audio().click();
    }

    if (input.pressedOrRepeated(ButtonId::Down) && selectedLevel_ + 1 < LEVEL_COUNT) {
        selectedLevel_++;
        manager.audio().click();
    }

    if (input.pressed(ButtonId::Exit) || input.pressed(ButtonId::Left)) {
        manager.audio().click();
        manager.switchTo(AppId::GamesMenu);
        return;
    }

    if (input.pressed(ButtonId::Ok) || input.pressed(ButtonId::Right)) {
        if (selectedLevel_ <= maxUnlockedLevel_) {
            manager.audio().click();
            startLevel(selectedLevel_);
        }
    }
}

void PongGameApp::updatePlayer(InputManager& input) {
    const int playerSpeed = 3;

    if (input.pressedOrRepeated(ButtonId::Up)) {
        playerY_ -= playerSpeed;
    }
    if (input.pressedOrRepeated(ButtonId::Down)) {
        playerY_ += playerSpeed;
    }

    if (playerY_ < FIELD_Y + 1) playerY_ = FIELD_Y + 1;
    if (playerY_ > FIELD_Y + FIELD_H - PADDLE_H - 1) {
        playerY_ = FIELD_Y + FIELD_H - PADDLE_H - 1;
    }
}

void PongGameApp::updateBot(const LevelSettings& lvl, uint32_t now) {
    const float ballCenterY = ballY_ + BALL_SIZE / 2.0f;
    const int botCenterY = botY_ + (PADDLE_H / 2);

    // Random AI mistakes
    if (!botErrorActive_) {
        // only sometimes make an error, more often on easier levels
        if (random(0, 1000) < lvl.errorChance) {
            botErrorActive_ = true;
            botErrorUntilMs_ = now + lvl.errorDurationMs;
            botErrorDir_ = (random(0, 2) == 0) ? -1 : 1;
        }
    } else if ((int32_t)(now - botErrorUntilMs_) >= 0) {
        botErrorActive_ = false;
        botErrorDir_ = 0;
    }

    if (botErrorActive_) {
        botY_ += botErrorDir_ * lvl.botSpeed;
    } else {
        if (ballCenterY < botCenterY - 2) {
            botY_ -= lvl.botSpeed;
        } else if (ballCenterY > botCenterY + 2) {
            botY_ += lvl.botSpeed;
        }
    }

    if (botY_ < FIELD_Y + 1) botY_ = FIELD_Y + 1;
    if (botY_ > FIELD_Y + FIELD_H - PADDLE_H - 1) {
        botY_ = FIELD_Y + FIELD_H - PADDLE_H - 1;
    }
}

void PongGameApp::updateBall(const LevelSettings& lvl, AppManager& manager) {
    ballX_ += ballVX_;
    ballY_ += ballVY_;

    // top/bottom walls
    if (ballY_ <= FIELD_Y + 1) {
        ballY_ = FIELD_Y + 1;
        ballVY_ = fabs(ballVY_);
        manager.audio().click();
    }

    if (ballY_ >= FIELD_Y + FIELD_H - BALL_SIZE - 1) {
        ballY_ = FIELD_Y + FIELD_H - BALL_SIZE - 1;
        ballVY_ = -fabs(ballVY_);
        manager.audio().click();
    }

    // player paddle
    if (ballVX_ < 0 &&
        ballX_ <= PLAYER_X + PADDLE_W &&
        ballX_ + BALL_SIZE >= PLAYER_X &&
        ballY_ + BALL_SIZE >= playerY_ &&
        ballY_ <= playerY_ + PADDLE_H) {

        ballX_ = PLAYER_X + PADDLE_W + 1;
        ballVX_ = fabs(lvl.ballSpeedX);

        float hitPos = (ballY_ + BALL_SIZE / 2.0f) - (playerY_ + PADDLE_H / 2.0f);
        ballVY_ = hitPos * 0.12f;

        if (ballVY_ > 1.8f) ballVY_ = 1.8f;
        if (ballVY_ < -1.8f) ballVY_ = -1.8f;

        manager.audio().click();
    }

    // bot paddle
    if (ballVX_ > 0 &&
        ballX_ + BALL_SIZE >= BOT_X &&
        ballX_ <= BOT_X + PADDLE_W &&
        ballY_ + BALL_SIZE >= botY_ &&
        ballY_ <= botY_ + PADDLE_H) {

        ballX_ = BOT_X - BALL_SIZE - 1;
        ballVX_ = -fabs(lvl.ballSpeedX);

        float hitPos = (ballY_ + BALL_SIZE / 2.0f) - (botY_ + PADDLE_H / 2.0f);
        ballVY_ = hitPos * 0.12f;

        if (ballVY_ > 1.8f) ballVY_ = 1.8f;
        if (ballVY_ < -1.8f) ballVY_ = -1.8f;

        manager.audio().click();
    }

    // score
    if (ballX_ < FIELD_X) {
        botScore_++;
        resetRound(false);
    }

    if (ballX_ > FIELD_X + FIELD_W) {
        playerScore_++;
        resetRound(true);
    }
}

void PongGameApp::checkWin(AppManager& manager) {
    LevelSettings lvl = getLevelSettings(selectedLevel_);

    if (playerScore_ >= lvl.scoreToWin) {
        solved_[selectedLevel_] = true;

        completedLevelsMask_ |= (1 << selectedLevel_);
        manager.storage().setPongCompletedMask(completedLevelsMask_);

        if (selectedLevel_ == maxUnlockedLevel_ && maxUnlockedLevel_ + 1 < LEVEL_COUNT) {
            maxUnlockedLevel_++;
        }

        winScreenStartMs_ = millis();
        state_ = State::WinScreen;
        return;
    }

    if (botScore_ >= lvl.scoreToWin) {
        // back to level select after lose
        state_ = State::LevelSelect;
    }
}

void PongGameApp::updatePlaying(AppManager& manager) {
    auto& input = manager.input();

    if (input.pressed(ButtonId::Exit)) {
        manager.audio().click();
        state_ = State::LevelSelect;
        return;
    }

    const uint32_t now = millis();
    if (lastFrameMs_ == 0) lastFrameMs_ = now;

    // simple fixed-step feel
    if ((int32_t)(now - lastFrameMs_) < 16) {
        return;
    }
    lastFrameMs_ = now;

    LevelSettings lvl = getLevelSettings(selectedLevel_);

    updatePlayer(input);
    updateBot(lvl, now);
    updateBall(lvl, manager);
    checkWin(manager);
}

void PongGameApp::updateWinScreen(AppManager& manager) {
    auto& input = manager.input();

    if (input.pressed(ButtonId::Ok) || input.pressed(ButtonId::Exit)) {
        manager.audio().click();

        // go back to Pong menu (LevelSelect)
        state_ = State::LevelSelect;
        return;
    }
}

void PongGameApp::drawCentered(U8G2& d, const char* text, int y) {
    int w = d.getStrWidth(text);
    d.drawStr((SCREEN_W - w) / 2, y, text);
}

void PongGameApp::renderLevelSelect(AppManager& manager) {
    auto& d = manager.display().oled();
    d.setFont(u8g2_font_6x12_tf);

    drawCentered(d, GAME_TITLE, 10);
    d.drawHLine(0, 13, 128);

    // visible list area
    const uint8_t visibleRows = 4;
    uint8_t top = 0;

    if (selectedLevel_ >= visibleRows) {
        top = selectedLevel_ - (visibleRows - 1);
    }
    if (top > LEVEL_COUNT - visibleRows) {
        top = LEVEL_COUNT - visibleRows;
    }

    for (uint8_t row = 0; row < visibleRows; ++row) {
        const uint8_t idx = top + row;
        const int y = 26 + row * 11;

        char line[24];
        const bool unlocked = idx <= maxUnlockedLevel_;
        const bool solved = solved_[idx];

        char marker = ' ';
        if (idx == selectedLevel_) marker = '>';
        else if (solved) marker = '*';
        else if (!unlocked) marker = 'x';

        snprintf(line, sizeof(line), "%c Lvl %02u", marker, idx + 1);
        d.drawStr(8, y, line);

        if (solved) {
            d.drawStr(80, y, "done");
        } else if (!unlocked) {
            d.drawStr(80, y, "lock");
        }
    }
}

void PongGameApp::renderPlaying(AppManager& manager) {
    auto& d = manager.display().oled();
    d.setFont(u8g2_font_6x12_tf);

    // small scoreboard
    char topLine[24];
    snprintf(topLine, sizeof(topLine), "L%u  %u:%u", selectedLevel_ + 1, playerScore_, botScore_);
    d.drawStr(4, 10, topLine);

    // center divider
    for (int y = 16; y < 62; y += 6) {
        d.drawVLine(63, y, 3);
    }

    d.drawLine(0, 14, 128, 14);
    d.drawLine(0, 63, 128, 63);

    // border
    // d.drawFrame(FIELD_X, FIELD_Y + 12, FIELD_W, FIELD_H - 20);

    // paddles
    d.drawBox(PLAYER_X, playerY_ + 12, PADDLE_W, PADDLE_H);
    d.drawBox(BOT_X, botY_ + 12, PADDLE_W, PADDLE_H);

    // ball
    d.drawBox((int)ballX_, (int)ballY_ + 12, BALL_SIZE, BALL_SIZE);
}

void PongGameApp::renderWinScreen(AppManager& manager) {
    auto& d = manager.display().oled();
    d.setFont(u8g2_font_6x12_tf);

    drawCentered(d, "Congratulations!", 24);

    char line[24];
    snprintf(line, sizeof(line), "Level %u solved", selectedLevel_ + 1);
    drawCentered(d, line, 40);
    // delay(2000);
    // manager.switchTo(AppId::PongGame);
}