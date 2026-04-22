#pragma once
#include <U8g2lib.h>

class DisplayManager {
public:
    DisplayManager();

    void begin();
    void clear();
    void send();
    void drawBattery(U8G2& oled, int x, int y, int percent);

    U8G2& oled();

private:
    U8G2_SH1106_128X64_NONAME_F_HW_I2C display_;
};