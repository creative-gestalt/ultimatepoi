#include "LED_Effects.h"
#include "Color.h"

LED_Effects::LED_Effects(Adafruit_DotStar &strip) {
    stripObj = &strip;
}

void LED_Effects::adjustBrightness(int8_t delta) {
    uint8_t newBrightness = constrain(stripObj->getBrightness() + delta, 0, 255);

    stripObj->setBrightness(newBrightness);

    if (stripObj->getBrightness() >= 255) {
        flashIndicator(0, 255, 255, 2, 500);
    }
}

void LED_Effects::flashIndicator(uint8_t r, uint8_t g, uint8_t b, uint8_t flashTimes, int flashSpeed) {
    uint8_t ob = stripObj->getBrightness();
    uint32_t stripColor = Adafruit_DotStar::Color(r,g,b);
    for (int i = 0; i < flashTimes; i++) {
        stripObj->setBrightness(100);
        stripObj->fill(stripColor, 0, 0);  // Set LEDs to green
        stripObj->show();
        delay(flashSpeed);  // Flash duration
        stripObj->clear();      // Turn off LEDs
        stripObj->show();
        delay(flashSpeed);  // Delay between flashes
    }
    stripObj->setBrightness(ob);
}

