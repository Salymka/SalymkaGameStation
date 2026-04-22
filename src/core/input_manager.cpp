#include "core/input_manager.h"
#include "config.h"

void InputManager::begin() {
    initButton(buttons_[static_cast<uint8_t>(ButtonId::Up)],    Config::PIN_BTN_UP);
    initButton(buttons_[static_cast<uint8_t>(ButtonId::Down)],  Config::PIN_BTN_DOWN);
    initButton(buttons_[static_cast<uint8_t>(ButtonId::Left)],  Config::PIN_BTN_LEFT);
    initButton(buttons_[static_cast<uint8_t>(ButtonId::Right)], Config::PIN_BTN_RIGHT);
    initButton(buttons_[static_cast<uint8_t>(ButtonId::Ok)],    Config::PIN_BTN_OK);
    initButton(buttons_[static_cast<uint8_t>(ButtonId::Exit)],  Config::PIN_BTN_EXIT);
}

void InputManager::initButton(ButtonState& btn, uint8_t pin) {
    btn.pin = pin;
    pinMode(pin, INPUT_PULLUP);

    bool now = digitalRead(pin);
    btn.rawState = now;
    btn.stableState = now;
    btn.lastStableState = now;
    btn.lastDebounceTime = millis();
    btn.pressStartTime = 0;
    btn.lastRepeatTime = 0;
    btn.pressedEvent = false;
    btn.releasedEvent = false;
    btn.repeatEvent = false;
}

void InputManager::update() {
    clearFrameEvents();

    for (uint8_t i = 0; i < static_cast<uint8_t>(ButtonId::Count); ++i) {
        updateButton(buttons_[i]);
    }
}

void InputManager::clearFrameEvents() {
    for (uint8_t i = 0; i < static_cast<uint8_t>(ButtonId::Count); ++i) {
        buttons_[i].pressedEvent = false;
        buttons_[i].releasedEvent = false;
        buttons_[i].repeatEvent = false;
    }
}

void InputManager::updateButton(ButtonState& btn) {
    const uint32_t now = millis();
    const bool rawNow = digitalRead(btn.pin);

    if (rawNow != btn.rawState) {
        btn.rawState = rawNow;
        btn.lastDebounceTime = now;
    }

    if ((now - btn.lastDebounceTime) >= Config::BUTTON_DEBOUNCE_MS) {
        if (btn.stableState != btn.rawState) {
            btn.lastStableState = btn.stableState;
            btn.stableState = btn.rawState;

            if (btn.stableState == LOW) {
                btn.pressedEvent = true;
                btn.pressStartTime = now;
                btn.lastRepeatTime = now;
            } else {
                btn.releasedEvent = true;
            }
        }
    }

    if (btn.stableState == LOW) {
        if ((now - btn.pressStartTime) >= Config::BUTTON_REPEAT_DELAY) {
            if ((now - btn.lastRepeatTime) >= Config::BUTTON_REPEAT_RATE) {
                btn.repeatEvent = true;
                btn.lastRepeatTime = now;
            }
        }
    }
}

bool InputManager::pressed(ButtonId id) const {
    return buttons_[static_cast<uint8_t>(id)].pressedEvent;
}

bool InputManager::released(ButtonId id) const {
    return buttons_[static_cast<uint8_t>(id)].releasedEvent;
}

bool InputManager::repeated(ButtonId id) const {
    return buttons_[static_cast<uint8_t>(id)].repeatEvent;
}

bool InputManager::pressedOrRepeated(ButtonId id) const {
    const auto& btn = buttons_[static_cast<uint8_t>(id)];
    return btn.pressedEvent || btn.repeatEvent;
}

bool InputManager::held(ButtonId id) const {
    return buttons_[static_cast<uint8_t>(id)].stableState == LOW;
}

bool InputManager::menuPrev() const {
    return pressedOrRepeated(ButtonId::Left);
}

bool InputManager::menuNext() const {
    return pressedOrRepeated(ButtonId::Right);
}

bool InputManager::menuOk() const {
    return pressed(ButtonId::Ok);
}
