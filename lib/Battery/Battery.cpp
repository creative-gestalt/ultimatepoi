#include <Arduino.h>
#include "Battery.h"

Battery::Battery(Adafruit_DotStar &strip) {
  stripObj = &strip;
  lastBrightness = 0;
}

void Battery::showBatteryLevel(void) {
  uint8_t lastBrightness = stripObj->getBrightness();

  stripObj->clear();
  stripObj->setBrightness(150);

  uint16_t mV = readVoltage();
  uint8_t lvl = (mV >= BATT_MAX_MV) ? stripObj->numPixels() : (mV <= BATT_MIN_MV) ? 1
                                                                                  :                                                                                              // Drained
                                                              1 + ((mV - BATT_MIN_MV) * stripObj->numPixels() +
                                                                   (stripObj->numPixels() / 2)) /
                                                                  (BATT_MAX_MV - BATT_MIN_MV +
                                                                   1);  // # LEDs lit (1-NUM_LEDS)

  for (uint8_t i = 0; i < lvl; i++) {
    uint8_t g = (i * 5 + 2) / stripObj->numPixels();
    stripObj->setPixelColor(i, 4 - g, g, 0);
    stripObj->show();
    delay(250 / stripObj->numPixels());
  }

  delay(1500);
  stripObj->clear();
  stripObj->show();
  stripObj->setBrightness(lastBrightness);
}

uint16_t Battery::readVoltage() {
  int i, prev;
  uint8_t count;
  uint16_t mV;

  // Select AVcc voltage reference + Bandgap (1.8V) input
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  ADCSRA = _BV(ADEN) |                            // Enable ADC
           _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);  // 1/128 prescaler (125 KHz)
  // Datasheet notes that the first bandgap reading is usually garbage as
  // voltages are stabilizing.  It practices, it seems to take a bit longer
  // than that.  Tried various delays, but still inconsistent and kludgey.
  // Instead, repeated readings are taken until four concurrent readings
  // stabilize within 10 mV.
  for (prev = 9999, count = 0; count < 4;) {
    for (ADCSRA |= _BV(ADSC); ADCSRA & _BV(ADSC););                               // Start, await ADC conv.
    i = ADC;                          // Result
    mV = i ? (1100L * 1023 / i) : 0;  // Scale to millivolts

    if (abs((int) mV - prev) <= 10) {
      count++;  // +1 stable reading
    } else {
      count = 0;  // too much change, start over
    }

    prev = mV;
  }
  ADCSRA = 0;  // ADC off
  return mV;
}
