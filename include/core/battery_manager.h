#pragma once

#include <Arduino.h>

class BatteryManager {
public:
    void begin();
    void update();

    float readAdcVoltage();
    float getFilteredVoltage() const;
    int getPercent() const;
    bool isLow() const;

private:
    static constexpr int BATTERY_ADC_PIN = 35; // change if needed

    // Your measured range at ADC pin
    static constexpr float ADC_MIN_V = 2.28f;
    static constexpr float ADC_MAX_V = 3.2f;

    float filteredVoltage_ = ADC_MIN_V;
    bool initialized_ = false;

    static constexpr uint8_t SAMPLE_BATCH = 16;
    static constexpr uint32_t SAMPLE_INTERVAL_MS = 200;

    uint32_t sampleSum_ = 0;
    uint8_t sampleCount_ = 0;
    uint32_t lastSampleMs_ = 0;

    int mapPercent(float v) const;
};