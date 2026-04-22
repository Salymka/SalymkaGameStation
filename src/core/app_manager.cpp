#include "core/app_manager.h"
#include "apps/main_menu_app.h"
#include "apps/games_menu_app.h"
#include "apps/programs_menu_app.h"
#include "apps/snake_game_app.h"
#include "apps/pong_game_app.h"
#include "apps/shooter_game_app.h"
#include "apps/info_app.h"
#include "core/audio_songs.h"
#include "config.h"

void AppManager::begin(DisplayManager& display, InputManager& input, AudioManager& audio, SensorManager& sensor) {
    display_ = &display;
    input_ = &input;
    audio_ = &audio;
    sensor_ = &sensor;
    battery_.begin();
    storage_.begin();
    splashStart_ = millis();
    splashActive_ = true;

    audio_->playSong(Album::hello_ta);
 

    createApps();
}

void AppManager::createApps() {
    static MainMenuApp mainMenu;
    static GamesMenuApp gamesMenu;
    static ProgramsMenuApp programsMenu;
    static SnakeGameApp snakeGame;
    static PongGameApp pongGame;
    static ShooterGameApp shooterGame;
    static InfoApp infoApp;

    mainMenuApp_ = &mainMenu;
    gamesMenuApp_ = &gamesMenu;
    programsMenuApp_ = &programsMenu;
    snakeGameApp_ = &snakeGame;
    pongGameApp_ = &pongGame;
    shooterGameApp_ = &shooterGame;
    infoApp_ = &infoApp;
}

void AppManager::update() {
    if (splashActive_) {
        if ((millis() - splashStart_) >= Config::SPLASH_DURATION_MS ||
            input_->pressed(ButtonId::Ok) ||
            input_->pressed(ButtonId::Exit)) {
            splashActive_ = false;
            switchTo(AppId::MainMenu);
        }
        return;
    }

    if (currentApp_) currentApp_->update(*this);
}

void AppManager::render() {
    if (splashActive_) {
        renderSplash();
        return;
    }

    display_->clear();
    if (currentApp_) currentApp_->render(*this);
    display_->send();
}

void AppManager::renderSplash() {
    auto& d = display_->oled();
    display_->clear();

    d.setFont(u8g2_font_6x12_tf);

    const char* line1 = "Powered by";
    const char* line2 = "Salymka";

    int w1 = d.getStrWidth(line1);
    int w2 = d.getStrWidth(line2);

    d.drawStr((128 - w1) / 2, 26, line1);
    d.drawStr((128 - w2) / 2, 42, line2);
    
    display_->send();
}

void AppManager::switchTo(AppId id) {
    IApp* previousApp = currentApp_;
    if (currentApp_) currentApp_->onExit();

    if (previousApp == infoApp_ && sensor_) {
        sensor_->end();
    }

    switch (id) {
        case AppId::MainMenu:     currentApp_ = mainMenuApp_; break;
        case AppId::GamesMenu:    currentApp_ = gamesMenuApp_; break;
        case AppId::ProgramsMenu: currentApp_ = programsMenuApp_; break;
        case AppId::Settings:     currentApp_ = infoApp_; break; // temporary
        case AppId::SnakeGame:    currentApp_ = snakeGameApp_; break;
        case AppId::PongGame:     currentApp_ = pongGameApp_; break;
        case AppId::ShooterGame:  currentApp_ = shooterGameApp_; break;
        case AppId::Info:         currentApp_ = infoApp_; break;
    }

    if (currentApp_) currentApp_->onEnter(*this);
}

DisplayManager& AppManager::display() {
    return *display_;
}

InputManager& AppManager::input() {
    return *input_;
}

AudioManager& AppManager::audio() {
    return *audio_;
}
SensorManager& AppManager::sensor() { 
    return *sensor_; 
}

bool AppManager::splashActive() const {
    return splashActive_;
}