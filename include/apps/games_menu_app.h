#pragma once
#include <Arduino.h>
#include "apps/i_app.h"

class GamesMenuApp : public IApp {
public:
    void onEnter(AppManager& manager) override;
    void onExit() override;
    void update(AppManager& manager) override;
    void render(AppManager& manager) override;

private:
    static constexpr uint8_t ITEM_COUNT = 3;
    uint8_t selected_ = 0;

    void drawCard(class AppManager& manager,
                  int x, int y, int w, int h,
                  const char* label,
                  bool selected,
                  uint8_t iconType);
};
