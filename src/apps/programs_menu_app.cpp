#include "apps/programs_menu_app.h"
#include "core/app_manager.h"
#include "core/input_manager.h"
#include "ui/menu.h"

void ProgramsMenuApp::onEnter(AppManager&) {
}

void ProgramsMenuApp::onExit() {
}

void ProgramsMenuApp::update(AppManager& manager) {
    auto& input = manager.input();
    if (input.pressed(ButtonId::Exit) || input.pressed(ButtonId::Left)) {
        manager.audio().click();
        manager.switchTo(AppId::MainMenu);
    }

    if (input.menuOk() || input.pressed(ButtonId::Right)) {
        manager.audio().click();
        manager.switchTo(AppId::Info);
    }
}

void ProgramsMenuApp::render(AppManager& manager) {
    static const char* items[] = {
        "Info"
    };

    MenuRenderer::draw(manager, "Programs", items, 1, selected_);
}