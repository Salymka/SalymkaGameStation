#pragma once

class AppManager;

class IApp {
public:
    virtual ~IApp() = default;

    virtual void onEnter(AppManager& manager) = 0;
    virtual void onExit() = 0;
    virtual void update(AppManager& manager) = 0;
    virtual void render(AppManager& manager) = 0;
};