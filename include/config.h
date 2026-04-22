#pragma once
#include <Arduino.h>

namespace Config {

// ---------- Display ----------
constexpr uint8_t SCREEN_WIDTH  = 128;
constexpr uint8_t SCREEN_HEIGHT = 64;

constexpr uint8_t PIN_I2C_SDA = 18;
constexpr uint8_t PIN_I2C_SCL = 19;

// ---------- Buzzer ----------
constexpr uint8_t PIN_BUZZER = 23;


// ---------- Buttons ----------
// INPUT_PULLUP => pressed = LOW
constexpr uint8_t PIN_BTN_UP    = 14;
constexpr uint8_t PIN_BTN_DOWN  = 25;
constexpr uint8_t PIN_BTN_LEFT  = 26;
constexpr uint8_t PIN_BTN_RIGHT = 33;
constexpr uint8_t PIN_BTN_OK    = 13;
constexpr uint8_t PIN_BTN_EXIT  = 27;


// ---------- Timings ----------
constexpr uint32_t SPLASH_DURATION_MS   = 1500;
constexpr uint16_t BUTTON_DEBOUNCE_MS   = 25;
constexpr uint16_t BUTTON_REPEAT_DELAY  = 200; // start repeat after hold
constexpr uint16_t BUTTON_REPEAT_RATE   = 80; // repeat interval while holding

// ---------- Audio ----------
constexpr uint8_t  BUZZER_CHANNEL     = 0;
constexpr uint16_t BUZZER_FREQ_UI     = 1800;
constexpr uint8_t  BUZZER_VOLUME_BITS = 10;

// ---------- UI ----------
constexpr uint8_t MENU_ITEM_HEIGHT      = 14;
constexpr uint8_t MENU_TOP_Y            = 24;
constexpr bool SHOW_BATTERY_PLACEHOLDER = true;

} // namespace Config