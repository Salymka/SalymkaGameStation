#include "core/battery_manager.h"

void BatteryManager::begin() {
    analogReadResolution(12); // 0..4095
#if defined(ESP32)
    analogSetPinAttenuation(BATTERY_ADC_PIN, ADC_11db); // up to ~3.6V
#endif
    pinMode(BATTERY_ADC_PIN, INPUT);
    filteredVoltage_ = readAdcVoltage();
    initialized_ = true;
}

void BatteryManager::update() {
    uint32_t now = millis();

    if ((uint32_t)(now - lastSampleMs_) < SAMPLE_INTERVAL_MS) {
        return;
    }
    lastSampleMs_ = now;

    sampleSum_ += analogRead(BATTERY_ADC_PIN);
    sampleCount_++;

    if (sampleCount_ >= SAMPLE_BATCH) {
        float rawAvg = sampleSum_ / (float)sampleCount_;
        float voltage = (rawAvg / 4095.0f) * 3.2f;
        Serial.printf("Battery ADC average: %.2f, voltage: %.2f V\n", rawAvg, voltage);
        if (!initialized_) {
            filteredVoltage_ = voltage;
            initialized_ = true;
        } else {
            // smoothing
            filteredVoltage_ = filteredVoltage_ * 0.85f + voltage * 0.15f;
        }
        Serial.printf("Battery filtered voltage: %.2f V\n", filteredVoltage_);
        sampleSum_ = 0;
        sampleCount_ = 0;
    }
}

float BatteryManager::readAdcVoltage() {
    const int samples = 5;
    uint32_t sum = 0;

    for (int i = 0; i < samples; i++) {
        sum += analogRead(BATTERY_ADC_PIN);
    }

    float raw = sum / (float)samples;
    // 12-bit ADC, approx 3.3V reference
    // since you already measured real voltages on this divider,
    // this is enough for UI percent indicator
    float voltage = (raw / 4095.0f) * 3.2f;
    Serial.printf("Battery ADC raw: %.2f, voltage: %.2f V\n", raw, voltage);
    return voltage;
}

float BatteryManager::getFilteredVoltage() const {
    return filteredVoltage_;
}

int BatteryManager::mapPercent(float v) const {
    if (v >= 3.20f) return 100;
    if (v <= 2.28f) return 0;

    float percent;

    if (v >= 3.10f) {
        // 3.10 - 3.20 → 80% - 100%
        percent = 80.0f + (v - 3.10f) * (20.0f / 0.10f);
    } 
    else if (v >= 3.00f) {
        // 3.00 - 3.10 → 60% - 80%
        percent = 60.0f + (v - 3.00f) * (20.0f / 0.10f);
    } 
    else if (v >= 2.90f) {
        // 2.90 - 3.00 → 40% - 60%
        percent = 40.0f + (v - 2.90f) * (20.0f / 0.10f);
    } 
    else if (v >= 2.70f) {
        // 2.70 - 2.90 → 20% - 40%
        percent = 20.0f + (v - 2.70f) * (20.0f / 0.20f);
    } 
    else {
        // 2.28 - 2.70 → 0% - 20%
        percent = (v - 2.28f) * (20.0f / 0.42f);
    }

    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    return (int)(percent + 0.5f);
}

int BatteryManager::getPercent() const {
    return mapPercent(filteredVoltage_);
}

bool BatteryManager::isLow() const {
    return getPercent() <= 15;
}