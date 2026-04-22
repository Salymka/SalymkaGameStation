#include "apps/main_menu_app.h"
#include "core/app_manager.h"
#include "core/input_manager.h"
#include "config.h"

namespace {
    const char* MENU_ITEMS[3] = {
        "Programs",
        "Games",
        "Settings"
    };

    enum IconType : uint8_t {
        ICON_PROGRAMS = 0,
        ICON_GAMES    = 1,
        ICON_SETTINGS = 2
    };

    const uint8_t ICONS[3] = {
        ICON_PROGRAMS,
        ICON_GAMES,
        ICON_SETTINGS
    };
}

void MainMenuApp::onEnter(AppManager&) {
}

void MainMenuApp::onExit() {
}

void MainMenuApp::update(AppManager& manager) {
    auto& input = manager.input();

    uint8_t oldSelected = selected_;

    if (input.menuPrev()) {
        selected_ = (selected_ == 0) ? (ITEM_COUNT - 1) : (selected_ - 1);
    } else if (input.menuNext()) {
        selected_ = (selected_ + 1) % ITEM_COUNT;
    }

    if (selected_ != oldSelected) {
        manager.audio().click();
    }

    if (input.menuOk() || input.pressed(ButtonId::Down)) {
        manager.audio().click();

        switch (selected_) {
            case 0: manager.switchTo(AppId::ProgramsMenu); break;
            case 1: manager.switchTo(AppId::GamesMenu);    break;
            case 2: manager.switchTo(AppId::Settings);     break;
        }
    }
}

void MainMenuApp::render(AppManager& manager) {
    auto& d = manager.display().oled();

    d.setFont(u8g2_font_6x12_tf);

    // Header
    d.drawStr(3, 9, "Main");
    int batteryPercent = manager.battery().getPercent();
    manager.display().drawBattery(d, 112, 3, batteryPercent);    

    // Carousel layout
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

    // Bottom hint dots
    for (uint8_t i = 0; i < ITEM_COUNT; ++i) {
        int x = 58 + i * 6;
        if (i == selected_) d.drawDisc(x, 61, 1);
        else d.drawCircle(x, 61, 1);
    }
}

void MainMenuApp::drawBatterySmall(AppManager& manager) {
    auto& d = manager.display().oled();

    // small icon top-right
    const int x = 114;
    const int y = 2;
    const int w = 10;
    const int h = 6;

    d.drawFrame(x, y, w, h);
    d.drawBox(x + w, y + 2, 2, 2);

    // placeholder fill for now
    d.drawBox(x + 2, y + 2, 4, 2);
}

void MainMenuApp::drawCard(AppManager& manager,
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
    int iconY = y + 8;

    // simple monochrome “image-like” icons
    switch (iconType) {
        case ICON_PROGRAMS:
            // 2x2 app grid
            d.drawFrame(cx - 10, iconY - 2, 8, 8);
            d.drawFrame(cx + 2,  iconY - 2, 8, 8);
            d.drawFrame(cx - 10, iconY + 10, 8, 8);
            d.drawFrame(cx + 2,  iconY + 10, 8, 8);
            break;

        case ICON_GAMES:
            // controller
            d.drawRFrame(cx - 12, iconY + 2, 24, 12, 3);
            d.drawHLine(cx - 8, iconY + 8, 6);
            d.drawVLine(cx - 5, iconY + 5, 6);
            d.drawDisc(cx + 6, iconY + 7, 1);
            d.drawDisc(cx + 10, iconY + 10, 1);
            break;

        case ICON_SETTINGS:
            // gear-like symbol
            d.drawCircle(cx, iconY + 8, 6);
            d.drawCircle(cx, iconY + 8, 2);
            d.drawHLine(cx - 9, iconY + 8, 3);
            d.drawHLine(cx + 7, iconY + 8, 3);
            d.drawVLine(cx, iconY - 1, 3);
            d.drawVLine(cx, iconY + 14, 3);
            d.drawLine(cx - 6, iconY + 2, cx - 8, iconY);
            d.drawLine(cx + 6, iconY + 2, cx + 8, iconY);
            d.drawLine(cx - 6, iconY + 14, cx - 8, iconY + 16);
            d.drawLine(cx + 6, iconY + 14, cx + 8, iconY + 16);
            break;
    }

    // label
    if(selected) {
        d.setFont(u8g2_font_6x12_tf);
        int textW = d.getStrWidth(label);
        int textX = x + (w - textW) / 2;
        int textY = y + h - 6;

        if (textX < x + 2) textX = x + 2;
        d.drawStr(textX, textY, label);
    }
    
}