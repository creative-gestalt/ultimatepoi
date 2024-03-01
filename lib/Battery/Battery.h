#ifndef BATTERY_H
#define BATTERY_H

#include <Arduino.h>
#include <Adafruit_DotStar.h>

// Empty and full thresholds (millivolts) used for battery level display:
#define BATT_MIN_MV 3350  // Some headroom over battery cutoff near 2.9V
#define BATT_MAX_MV 4000  // And little below fresh-charged battery near 4.1V

class Battery {

public:
    explicit Battery(Adafruit_DotStar &strip);

    uint8_t lastBrightness;

    void showBatteryLevel();

private:
    Adafruit_DotStar *stripObj;

    static uint16_t readVoltage();
};

#endif  // BATTERY_H