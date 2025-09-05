#ifndef BATTERY_H
#define BATTERY_H

#include <Arduino.h>
#include <Adafruit_DotStar.h>

// Empty and full thresholds (millivolts) used for battery level display:
#define BATT_MIN_MV 3200  // Some headroom over battery cutoff near 2.9V
#define BATT_MAX_MV 4200  // And little below fresh-charged battery near 4.1V
#define VBUS_PIN A5

class Battery {

public:
    explicit Battery(Adafruit_DotStar &strip);

    uint8_t lastBrightness;

    static void initializeChargingState();
    void showBatteryLevel() const;
    void updateChargingDisplay() const;
    static bool isCharging();
    static bool isChargeComplete();
    static void printVoltageDebug() ;

private:
    Adafruit_DotStar *stripObj;

    static uint16_t readVoltage();

    // Charging state tracking
    static unsigned long lastChargeCheck;
    static bool chargeCompleteState;
    static bool cachedChargingState;
    static unsigned long lastChargingCheck;
};

#endif  // BATTERY_H