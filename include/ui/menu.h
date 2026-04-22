#pragma once
#include <Arduino.h>

class AppManager;

class MenuRenderer {
public:
    static void draw(AppManager& manager,
                     const char* title,
                     const char* const items[],
                     uint8_t count,
                     uint8_t selected);
};