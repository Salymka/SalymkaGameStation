#include "core/sensor_manager.h"
#include <Wire.h>

namespace {
constexpr uint8_t MPU6050_ADDR = 0x68;
constexpr uint8_t MPU6050_REG_PWR_MGMT_1 = 0x6B;
constexpr uint8_t MPU6050_SLEEP = 0x40;
}

bool SensorManager::begin() {
    if (ready_) {
        return true;
    }

    if (!mpu_.begin(MPU6050_ADDR, &Wire)) {
        ready_ = false;
        return false;
    }

    mpu_.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu_.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu_.setFilterBandwidth(MPU6050_BAND_21_HZ);

    ready_ = true;
    return true;
}

void SensorManager::end() {
    if (!ready_) {
        return;
    }

    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(MPU6050_REG_PWR_MGMT_1);
    Wire.write(MPU6050_SLEEP);
    Wire.endTransmission();

    ready_ = false;
    ax_ = ay_ = az_ = 0.0f;
    gx_ = gy_ = gz_ = 0.0f;
    temp_ = 0.0f;
}

void SensorManager::update() {
    if (!ready_) return;

    sensors_event_t a, g, temp;
    mpu_.getEvent(&a, &g, &temp);

    ax_ = a.acceleration.x;
    ay_ = a.acceleration.y;
    az_ = a.acceleration.z;

    gx_ = g.gyro.x;
    gy_ = g.gyro.y;
    gz_ = g.gyro.z;

    temp_ = temp.temperature;
}
