#include "apps/games_menu_app.h"
#include "core/app_manager.h"
#include "core/input_manager.h"

namespace {
    const char* MENU_ITEMS[3] = {
        "Snake",
        "Pong",
        "Shooter"
    };

    enum IconType : uint8_t {
        ICON_SNAKE = 0,
        ICON_PONG  = 1,
        ICON_SHOOTER = 2
    };

    const uint8_t ICONS[3] = {
        ICON_SNAKE,
        ICON_PONG,
        ICON_SHOOTER
    };
}

void GamesMenuApp::onEnter(AppManager&) {
}

void GamesMenuApp::onExit() {
}

void GamesMenuApp::update(AppManager& manager) {
    auto& input = manager.input();

    uint8_t oldSelected = selected_;

    if (input.pressed(ButtonId::Exit)) {
        manager.audio().click();
        manager.switchTo(AppId::MainMenu);
        return;
    }

    if (input.menuPrev() || input.pressedOrRepeated(ButtonId::Up)) {
        selected_ = (selected_ == 0) ? (ITEM_COUNT - 1) : (selected_ - 1);
    } else if (input.menuNext() || input.pressedOrRepeated(ButtonId::Down)) {
        selected_ = (selected_ + 1) % ITEM_COUNT;
    }

    if (selected_ != oldSelected) {
        manager.audio().click();
    }

    if (input.menuOk()) {
        manager.audio().click();

        if (selected_ == 0) manager.switchTo(AppId::SnakeGame);
        if (selected_ == 1) manager.switchTo(AppId::PongGame);
        if (selected_ == 2) manager.switchTo(AppId::ShooterGame);
    }
}

void GamesMenuApp::render(AppManager& manager) {
    auto& d = manager.display().oled();

    d.setFont(u8g2_font_6x12_tf);
    d.drawStr(3, 9, "Games");

    int batteryPercent = manager.battery().getPercent();
    manager.display().drawBattery(d, 112, 3, batteryPercent);

    const int centerX = 64;
    const int cardY   = 16;

    const int bigW = 50;
    const int bigH = 42;

    const int sideW = 32;
    const int sideH = 30;

    const int gap = 4;

    uint8_t leftIndex  = (selected_ == 0) ? (ITEM_COUNT - 1) : (selected_ - 1);
    uint8_t rightIndex = (selected_ + 1) % ITEM_COUNT;

    int bigX = centerX - bigW / 2;
    int leftX = bigX - gap - sideW;
    int rightX = bigX + bigW + gap;

    drawCard(manager, leftX,  cardY + 6, sideW, sideH, MENU_ITEMS[leftIndex],  false, ICONS[leftIndex]);
    drawCard(manager, bigX,   cardY,     bigW,  bigH,  MENU_ITEMS[selected_],   true,  ICONS[selected_]);
    drawCard(manager, rightX, cardY + 6, sideW, sideH, MENU_ITEMS[rightIndex], false, ICONS[rightIndex]);

    for (uint8_t i = 0; i < ITEM_COUNT; ++i) {
        int x = 58 + i * 6;
        if (i == selected_) d.drawDisc(x, 61, 1);
        else d.drawCircle(x, 61, 1);
    }
}

void GamesMenuApp::drawCard(AppManager& manager,
                            int x, int y, int w, int h,
                            const char* label,
                            bool selected,
                            uint8_t iconType) {
    auto& d = manager.display().oled();

    if (selected) {
        d.drawRFrame(x, y, w, h, 4);
    } else {
        d.drawFrame(x, y, w, h);
    }

    int cx = x + w / 2;
    int iconY = y + 9;

    switch (iconType) {
        case ICON_SNAKE:
            d.drawCircle(cx - 6, iconY + 2, 2);
            d.drawDisc(cx - 5, iconY + 1, 1);
            d.drawLine(cx - 4, iconY + 4, cx + 2, iconY + 8);
            d.drawLine(cx + 2, iconY + 8, cx - 2, iconY + 12);
            d.drawLine(cx - 2, iconY + 12, cx + 4, iconY + 16);
            d.drawLine(cx + 4, iconY + 16, cx, iconY + 20);
            break;

        case ICON_PONG:
            d.drawFrame(cx - 11, iconY + 1, 22, 18);
            d.drawBox(cx - 8, iconY + 5, 2, 8);
            d.drawBox(cx + 6, iconY + 7, 2, 8);
            d.drawDisc(cx, iconY + 9, 1);
            break;

        case ICON_SHOOTER:
            d.drawLine(cx, iconY + 1, cx - 4, iconY + 6);
            d.drawLine(cx, iconY + 1, cx + 4, iconY + 6);
            d.drawLine(cx - 4, iconY + 6, cx + 4, iconY + 6);
            d.drawBox(cx - 1, iconY + 6, 3, 5);
            d.drawPixel(cx - 6, iconY + 4);
            d.drawPixel(cx + 6, iconY + 4);
            d.drawVLine(cx, iconY - 4, 4);
            d.drawFrame(cx - 10, iconY + 12, 8, 5);
            d.drawPixel(cx - 11, iconY + 14);
            d.drawPixel(cx - 2, iconY + 14);
            break;
    }

    if (selected) {
        d.setFont(u8g2_font_6x12_tf);
        int textW = d.getStrWidth(label);
        int textX = x + (w - textW) / 2;
        int textY = y + h - 6;

        if (textX < x + 2) textX = x + 2;
        d.drawStr(textX, textY, label);
    }
}
