#include "apps/info_app.h"
#include "core/app_manager.h"
#include "core/input_manager.h"
#include <stdio.h>

void InfoApp::onEnter(AppManager& manager) {
    lastSensorUpdateMs_ = 0;
    manager.sensor().begin();
    manager.sensor().update();
}

void InfoApp::onExit() {
}

void InfoApp::update(AppManager& manager) {
    uint32_t now = millis();
    auto& input = manager.input();

    if (now - lastSensorUpdateMs_ >= 1000) {
        manager.sensor().update();
        lastSensorUpdateMs_ = now;
    }

    if (input.pressed(ButtonId::Exit) || input.pressed(ButtonId::Left)) {
        manager.audio().click();
        manager.switchTo(AppId::MainMenu);
    }
}

void InfoApp::render(AppManager& manager) {
    auto& d = manager.display().oled();

    d.setFont(u8g2_font_6x12_tf);
    d.drawStr(2, 10, "Info");

    if (manager.sensor().isReady()) {
        char buf[32];
        snprintf(buf, sizeof(buf), "Temp: %.1f C", manager.sensor().temperature());
        d.drawStr(2, 28, buf);
    } else {
        d.drawStr(2, 28, "MPU6050 not found");
    }

    int batteryPercent = manager.battery().getPercent();
    Serial.printf("Battery percent: %d%%\n", batteryPercent);
    float batteryAdcVoltage = manager.battery().getFilteredVoltage();
    Serial.printf("Battery ADC voltage: %.2f V\n", batteryAdcVoltage);
    char buf[32];

    snprintf(buf, sizeof(buf), "Battery: %d%%", batteryPercent);
    d.drawStr(0, 52, buf);

    snprintf(buf, sizeof(buf), "ADC: %.2fV", batteryAdcVoltage);
    d.drawStr(0, 62, buf);
}
