#include "apps/shooter_game_app.h"
#include "core/app_manager.h"
#include "core/input_manager.h"
#include "core/audio_manager.h"
#include <stdio.h>

void ShooterGameApp::onEnter(AppManager& manager) {
    bestScore_ = manager.storage().getShooterBestScore();
    resetGame();
    state_ = State::Start;
}

void ShooterGameApp::onExit() {
}

void ShooterGameApp::resetGame() {
    playerX_ = 58;
    score_ = 0;
    enemySpawnDelayMs_ = 700;
    lastFireMs_ = 0;
    lastSpawnMs_ = millis();
    lastStepMs_ = millis();
    gameOverAtMs_ = 0;

    for (uint8_t i = 0; i < MAX_BULLETS; ++i) bullets_[i].active = false;
    for (uint8_t i = 0; i < MAX_ENEMIES; ++i) enemies_[i].active = false;
}

void ShooterGameApp::update(AppManager& manager) {
    switch (state_) {
        case State::Start: updateStart(manager); break;
        case State::Playing: updatePlaying(manager); break;
        case State::GameOver: updateGameOver(manager); break;
    }
}

void ShooterGameApp::render(AppManager& manager) {
    auto& d = manager.display().oled();
    d.setFont(u8g2_font_6x12_tf);

    switch (state_) {
        case State::Start: {
            drawCentered(d, "Shooter", 9);
            d.drawHLine(0, 11, 128);
            char line[24];
            snprintf(line, sizeof(line), "Best:%u", bestScore_);
            d.drawStr(4, 22, line);
            d.drawStr(4, 34, "LEFT/RIGHT move");
            d.drawStr(4, 46, "OK fire/start");
            d.drawStr(4, 58, "EXIT back");
            drawPlayer(d, 100, 30, true);
            drawEnemy(d, 95, 18, 0);
            break;
        }

        case State::Playing: {
            char hud[32];
            snprintf(hud, sizeof(hud), "S:%u B:%u", score_, bestScore_);
            d.drawStr(2, 8, hud);
            d.drawHLine(0, HUD_H - 1, SCREEN_W);

            for (int x = 0; x < SCREEN_W; x += 24) {
                d.drawPixel((x + (millis() / 80)) % SCREEN_W, HUD_H + (x % 7));
            }

            for (uint8_t i = 0; i < MAX_BULLETS; ++i) {
                if (bullets_[i].active) {
                    d.drawVLine(bullets_[i].x, bullets_[i].y, 3);
                }
            }

            const uint8_t enemyFrame = (millis() / 140) % 2;
            for (uint8_t i = 0; i < MAX_ENEMIES; ++i) {
                if (enemies_[i].active) {
                    drawEnemy(d, enemies_[i].x, enemies_[i].y, enemyFrame);
                }
            }

            drawPlayer(d, playerX_, PLAYER_Y, ((millis() / 120) % 2) == 0);
            break;
        }

        case State::GameOver: {
            d.drawHLine(0, HUD_H - 1, SCREEN_W);
            for (uint8_t i = 0; i < MAX_ENEMIES; ++i) {
                if (enemies_[i].active) drawEnemy(d, enemies_[i].x, enemies_[i].y, 1);
            }
            d.drawBox(22, 18, 84, 28);
            d.setDrawColor(0);
            drawCentered(d, "GAME OVER", 30);
            char line[24];
            snprintf(line, sizeof(line), "Score:%u", score_);
            drawCentered(d, line, 42);
            d.setDrawColor(1);
            break;
        }
    }
}

void ShooterGameApp::updateStart(AppManager& manager) {
    auto& input = manager.input();

    if (input.pressed(ButtonId::Exit)) {
        manager.audio().click();
        manager.switchTo(AppId::GamesMenu);
        return;
    }

    if (input.pressed(ButtonId::Ok) || input.pressed(ButtonId::Left) || input.pressed(ButtonId::Right)) {
        manager.audio().click();
        state_ = State::Playing;
        lastStepMs_ = millis();
        lastSpawnMs_ = millis();
    }
}

void ShooterGameApp::fire(AppManager& manager) {
    const uint32_t now = millis();
    if ((uint32_t)(now - lastFireMs_) < 180) return;

    for (uint8_t i = 0; i < MAX_BULLETS; ++i) {
        if (!bullets_[i].active) {
            bullets_[i].active = true;
            bullets_[i].x = playerX_ + PLAYER_W / 2;
            bullets_[i].y = PLAYER_Y - 2;
            lastFireMs_ = now;
            manager.audio().play(1600, 20);
            return;
        }
    }
}

void ShooterGameApp::spawnEnemy() {
    for (uint8_t i = 0; i < MAX_ENEMIES; ++i) {
        if (!enemies_[i].active) {
            enemies_[i].active = true;
            enemies_[i].x = random(2, SCREEN_W - 10);
            enemies_[i].y = FIELD_Y + 1;
            enemies_[i].speed = 1 + random(0, 2);
            return;
        }
    }
}

void ShooterGameApp::triggerGameOver(AppManager& manager) {
    state_ = State::GameOver;
    gameOverAtMs_ = millis();
    manager.audio().play(220, 180);
    if (score_ > bestScore_) {
        bestScore_ = score_;
        manager.storage().setShooterBestScore(bestScore_);
    }
}

void ShooterGameApp::stepGame(AppManager& manager) {
    const uint32_t now = millis();

    if ((uint32_t)(now - lastSpawnMs_) >= enemySpawnDelayMs_) {
        lastSpawnMs_ = now;
        spawnEnemy();
        if (enemySpawnDelayMs_ > 250) enemySpawnDelayMs_ -= 8;
    }

    for (uint8_t i = 0; i < MAX_BULLETS; ++i) {
        if (!bullets_[i].active) continue;
        bullets_[i].y -= 3;
        if (bullets_[i].y < FIELD_Y) bullets_[i].active = false;
    }

    for (uint8_t i = 0; i < MAX_ENEMIES; ++i) {
        if (!enemies_[i].active) continue;
        enemies_[i].y += enemies_[i].speed;
        if (enemies_[i].y > SCREEN_H - 8) {
            triggerGameOver(manager);
            return;
        }

        if (enemies_[i].y + 6 >= PLAYER_Y && enemies_[i].y <= PLAYER_Y + PLAYER_H &&
            enemies_[i].x + 7 >= playerX_ && enemies_[i].x <= playerX_ + PLAYER_W) {
            triggerGameOver(manager);
            return;
        }
    }

    for (uint8_t b = 0; b < MAX_BULLETS; ++b) {
        if (!bullets_[b].active) continue;
        for (uint8_t e = 0; e < MAX_ENEMIES; ++e) {
            if (!enemies_[e].active) continue;
            if (bullets_[b].x >= enemies_[e].x && bullets_[b].x <= enemies_[e].x + 7 &&
                bullets_[b].y >= enemies_[e].y && bullets_[b].y <= enemies_[e].y + 6) {
                bullets_[b].active = false;
                enemies_[e].active = false;
                score_++;
                if (score_ > bestScore_) bestScore_ = score_;
                manager.audio().play(1200, 35);
                break;
            }
        }
    }
}

void ShooterGameApp::updatePlaying(AppManager& manager) {
    auto& input = manager.input();

    if (input.pressed(ButtonId::Exit)) {
        manager.audio().click();
        manager.switchTo(AppId::GamesMenu);
        return;
    }

    if (input.held(ButtonId::Left) || input.pressedOrRepeated(ButtonId::Left)) {
        playerX_ -= 3;
    }
    if (input.held(ButtonId::Right) || input.pressedOrRepeated(ButtonId::Right)) {
        playerX_ += 3;
    }
    if (playerX_ < 1) playerX_ = 1;
    if (playerX_ > SCREEN_W - PLAYER_W - 1) playerX_ = SCREEN_W - PLAYER_W - 1;

    if (input.pressed(ButtonId::Ok) || input.pressed(ButtonId::Up)) {
        fire(manager);
    }

    const uint32_t now = millis();
    if ((uint32_t)(now - lastStepMs_) >= 33) {
        lastStepMs_ = now;
        stepGame(manager);
    }
}

void ShooterGameApp::updateGameOver(AppManager& manager) {
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

    if ((int32_t)(millis() - gameOverAtMs_) > 4000) {
        resetGame();
        state_ = State::Start;
    }
}

void ShooterGameApp::drawCentered(U8G2& d, const char* text, int y) {
    int w = d.getStrWidth(text);
    d.drawStr((SCREEN_W - w) / 2, y, text);
}

void ShooterGameApp::drawPlayer(U8G2& d, int x, int y, bool thrust) {
    d.drawPixel(x + 5, y);
    d.drawLine(x + 5, y, x + 2, y + 4);
    d.drawLine(x + 5, y, x + 8, y + 4);
    d.drawLine(x + 2, y + 4, x + 8, y + 4);
    d.drawBox(x + 4, y + 4, 3, 3);
    d.drawPixel(x + 1, y + 6);
    d.drawPixel(x + 9, y + 6);
    if (thrust) {
        d.drawLine(x + 5, y + 7, x + 4, y + 9);
        d.drawLine(x + 5, y + 7, x + 6, y + 9);
    }
}

void ShooterGameApp::drawEnemy(U8G2& d, int x, int y, uint8_t frame) {
    d.drawFrame(x + 1, y + 1, 6, 4);
    d.drawPixel(x, y + 2);
    d.drawPixel(x + 7, y + 2);
    d.drawPixel(x + 3, y);
    if (frame == 0) {
        d.drawPixel(x + 1, y + 5);
        d.drawPixel(x + 6, y + 5);
    } else {
        d.drawPixel(x + 2, y + 6);
        d.drawPixel(x + 5, y + 6);
    }
}
