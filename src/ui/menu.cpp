#include "ui/menu.h"
#include "core/app_manager.h"
#include "config.h"

void MenuRenderer::draw(AppManager& manager,
                        const char* title,
                        const char* const items[],
                        uint8_t count,
                        uint8_t selected) {
    auto& d = manager.display().oled();
    d.setFont(u8g2_font_6x12_tf);

    d.drawStr(2, 10, title);
    d.drawHLine(0, 13, 128);

    int batteryPercent = manager.battery().getPercent();
    manager.display().drawBattery(d, 112, 3, batteryPercent);

    for (uint8_t i = 0; i < count; ++i) {
        int y = Config::MENU_TOP_Y + i * Config::MENU_ITEM_HEIGHT;

        if (i == selected) {
            d.drawStr(2, y, ">");
        }

        d.drawStr(12, y, items[i]);
    }
}