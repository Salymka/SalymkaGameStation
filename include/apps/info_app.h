#pragma once
#include "apps/i_app.h"
#include <Arduino.h>

class InfoApp : public IApp {
public:
    void onEnter(AppManager& manager) override;
    void onExit() override;
    void update(AppManager& manager) override;
    void render(AppManager& manager) override;

private:
    uint32_t lastSensorUpdateMs_ = 0;
};