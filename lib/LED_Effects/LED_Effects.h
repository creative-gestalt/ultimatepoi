#ifndef LED_EFFECTS_H
#define LED_EFFECTS_H

#include <Arduino.h>
#include "Adafruit_DotStar.h"

#define PATTERN_COUNT 3

class LED_Effects {

public:
    explicit LED_Effects(Adafruit_DotStar &strip);

    void adjustBrightness(int8_t delta);

    void flashIndicator(uint8_t r, uint8_t g, uint8_t b, uint8_t flashTimes, int flashSpeed);

private:
    Adafruit_DotStar *stripObj;
};

#endif  // LED_EFFECTS_H