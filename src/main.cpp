#include <Arduino.h>
#include "core/display_manager.h"
#include "core/input_manager.h"
#include "core/audio_manager.h"
#include "core/app_manager.h"
#include "core/sensor_manager.h"
#include "core/battery_manager.h"

DisplayManager display;
InputManager input;
AudioManager audio;
AppManager appManager;
SensorManager sensor;
BatteryManager battery;

void setup() {
    Serial.begin(115200);
    display.begin();
    input.begin();
    audio.begin();
    battery.begin();
    appManager.begin(display, input, audio, sensor);
}

void loop() {
    input.update();
    audio.update();
    battery.update();

    appManager.update();
    appManager.render();

}