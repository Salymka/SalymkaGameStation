#pragma once

#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

class SensorManager {
public:
    bool begin();
    void end();
    void update();

    float accelX() const { return ax_; }
    float accelY() const { return ay_; }
    float accelZ() const { return az_; }

    float gyroX() const { return gx_; }
    float gyroY() const { return gy_; }
    float gyroZ() const { return gz_; }

    float temperature() const { return temp_; }

    bool isReady() const { return ready_; }

private:
    Adafruit_MPU6050 mpu_;
    bool ready_ = false;

    float ax_ = 0.0f;
    float ay_ = 0.0f;
    float az_ = 0.0f;

    float gx_ = 0.0f;
    float gy_ = 0.0f;
    float gz_ = 0.0f;

    float temp_ = 0.0f;
};