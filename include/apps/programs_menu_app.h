#pragma once
#include <Arduino.h>
#include "apps/i_app.h"

class ProgramsMenuApp : public IApp {
public:
    void onEnter(AppManager& manager) override;
    void onExit() override;
    void update(AppManager& manager) override;
    void render(AppManager& manager) override;

private:
    uint8_t selected_ = 0;
};