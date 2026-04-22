#pragma once
#include <Arduino.h>

enum class ButtonId : uint8_t {
    Up = 0,
    Down,
    Left,
    Right,
    Ok,
    Exit,
    Count
};

struct ButtonState {
    uint8_t pin = 0;

    bool rawState = HIGH;
    bool stableState = HIGH;
    bool lastStableState = HIGH;

    uint32_t lastDebounceTime = 0;
    uint32_t pressStartTime = 0;
    uint32_t lastRepeatTime = 0;

    bool pressedEvent = false;
    bool releasedEvent = false;
    bool repeatEvent = false;
};

class InputManager {
public:
    void begin();
    void update();

    bool pressed(ButtonId id) const;
    bool released(ButtonId id) const;
    bool repeated(ButtonId id) const;
    bool pressedOrRepeated(ButtonId id) const;
    bool held(ButtonId id) const;

    bool menuNext() const;
    bool menuPrev() const;
    bool menuOk() const;

private:
    ButtonState buttons_[static_cast<uint8_t>(ButtonId::Count)];

    void initButton(ButtonState& btn, uint8_t pin);
    void updateButton(ButtonState& btn);
    void clearFrameEvents();
};
