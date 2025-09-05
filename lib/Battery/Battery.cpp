#include <Arduino.h>
#include "Battery.h"

// Static member initialization
unsigned long Battery::lastChargeCheck = 0;
bool Battery::chargeCompleteState = false;
bool Battery::cachedChargingState = false;
unsigned long Battery::lastChargingCheck = 0;


Battery::Battery(Adafruit_DotStar &strip) {
    stripObj = &strip;
    lastBrightness = 0;
}

void Battery::initializeChargingState() {
    // Check charging state once at startup
    uint16_t voltage = readVoltage();
    //Serial.println("Battery voltage: " + String(voltage));
    cachedChargingState = (voltage > 4200);
}

bool Battery::isCharging() {
    return cachedChargingState;
}

bool Battery::isChargeComplete() {
    // Check every 10 seconds
    if (millis() - lastChargeCheck > 10000) {
        uint16_t currentVoltage = readVoltage();

        // Use voltage-based detection: charge complete at ~4788mV
        chargeCompleteState = (currentVoltage >= 4765); // Small buffer for measurement variance

        lastChargeCheck = millis();
    }

    return chargeCompleteState;
}


void Battery::printVoltageDebug() {
    uint16_t supplyVoltage = readVoltage();
    const bool charging = isCharging();
    const bool complete = isChargeComplete();

    Serial.print("Supply: ");
    Serial.print(supplyVoltage);
    Serial.print("mV, Charging: ");
    Serial.print(charging ? "YES" : "NO");
    Serial.print(", Complete: ");
    Serial.print(complete ? "YES" : "NO");

    Serial.println();
}

void Battery::updateChargingDisplay() const {
    if (!isCharging()) {
        return; // Not charging, don't change display
    }

    if (isChargeComplete()) {
        // Single green LED when charge complete
        stripObj->setPixelColor(0, 0, 100, 0); // Dim green on first LED
    } else {
        // Single dim red LED while charging
        stripObj->setPixelColor(0, 50, 0, 0); // Dim red on first LED
    }
}

void Battery::showBatteryLevel() const {
    uint8_t savedBrightness = stripObj->getBrightness();
    stripObj->clear();
    stripObj->setBrightness(150);

    uint16_t mV = readVoltage();
    uint8_t lvl = (mV >= BATT_MAX_MV)
                      ? stripObj->numPixels()
                      : (mV <= BATT_MIN_MV)
                            ? 1
                            : // Drained
                            1 + ((mV - BATT_MIN_MV) * stripObj->numPixels() +
                                 (stripObj->numPixels() / 2)) /
                            (BATT_MAX_MV - BATT_MIN_MV +
                             1); // # LEDs lit (1-NUM_LEDS)

    for (uint8_t i = 0; i < lvl; i++) {
        uint8_t g = (i * 5 + 2) / stripObj->numPixels();
        stripObj->setPixelColor(i, 4 - g, g, 0);
        stripObj->show();
        delay(250 / stripObj->numPixels());
    }

    delay(1500);
    stripObj->clear();
    stripObj->show();
    stripObj->setBrightness(savedBrightness);
}

uint16_t Battery::readVoltage() {
    int i, prev;
    uint8_t count;
    uint16_t mV;

    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

    for (prev = 9999, count = 0; count < 4;) {
        for (ADCSRA |= _BV(ADSC); ADCSRA & _BV(ADSC););
        i = ADC;
        mV = i ? (1100L * 1023 / i) : 0;

        if (abs((int) mV - prev) <= 10) {
            count++;
        } else {
            count = 0;
        }
        prev = mV;
    }

    ADCSRA = 0;
    return mV;
}
