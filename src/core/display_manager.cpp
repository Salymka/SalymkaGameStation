#include "core/display_manager.h"
#include "config.h"
#include <Wire.h>

DisplayManager::DisplayManager()
    : display_(U8G2_R0, U8X8_PIN_NONE) {
}

void DisplayManager::begin() {
    Wire.begin(Config::PIN_I2C_SDA, Config::PIN_I2C_SCL);
    Wire.setClock(400000);
    display_.begin();
    display_.setContrast(255);
    display_.setFont(u8g2_font_6x12_tf);
}

void DisplayManager::drawBattery(U8G2& oled, int x, int y, int percent) {
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    const int w = 10;
    const int h = 5;

    // body
    oled.drawFrame(x, y, w, h);

    // terminal
    oled.drawBox(x + w, y + 1, 1, 3);

    // fill
    int fill = ((w - 2) * percent) / 100;

    if (fill > 0) {
        oled.drawBox(x + 1, y + 1, fill, h - 2);
    }
}

void DisplayManager::clear() {
    display_.clearBuffer();
}

void DisplayManager::send() {
    display_.sendBuffer();
}

U8G2& DisplayManager::oled() {
    return display_;
}