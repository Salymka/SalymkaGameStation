#pragma once
#include <Arduino.h>
#include "core/display_manager.h"
#include "core/input_manager.h"
#include "core/audio_manager.h"
#include "core/sensor_manager.h"
#include "core/storage_manager.h"
#include "apps/i_app.h"
#include "core/battery_manager.h"

enum class AppId : uint8_t {
    MainMenu = 0,
    GamesMenu,
    ProgramsMenu,
    Settings,
    SnakeGame,
    PongGame,
    ShooterGame,
    Info
};

class AppManager {
public:
    void begin(DisplayManager& display, InputManager& input, AudioManager& audio, SensorManager& sensor);
    void update();
    void render();

    void switchTo(AppId id);

    DisplayManager& display();
    InputManager& input();
    AudioManager& audio();
    SensorManager& sensor();
    StorageManager& storage() { return storage_; }
    BatteryManager& battery() { return battery_; }

    bool splashActive() const;

private:
    DisplayManager* display_ = nullptr;
    InputManager* input_ = nullptr;
    AudioManager* audio_ = nullptr;
    SensorManager* sensor_ = nullptr;
    StorageManager storage_;
    BatteryManager battery_;
    

    uint32_t splashStart_ = 0;
    bool splashActive_ = true;

    IApp* currentApp_ = nullptr;

    class MainMenuApp* mainMenuApp_ = nullptr;
    class GamesMenuApp* gamesMenuApp_ = nullptr;
    class ProgramsMenuApp* programsMenuApp_ = nullptr;
    class SnakeGameApp* snakeGameApp_ = nullptr;
    class PongGameApp* pongGameApp_ = nullptr;
    class ShooterGameApp* shooterGameApp_ = nullptr;
    class InfoApp* infoApp_ = nullptr;
    

    void renderSplash();
    void createApps();
};