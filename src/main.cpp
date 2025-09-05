#include <Arduino.h>
#include <Adafruit_DotStar.h>
#include <IRremote.hpp>
#include <SPI.h>
#include <Wire.h>

// Private
#include <Battery.h>
#include <Images.h>
#include <LED_Effects.h>
#include <Remote.h>
#include <avr/power.h>

#define NUM_LEDS 22
//#define DATA_PIN 5
//#define CLOCK_PIN 6
#define IR_RECEIVE_PIN 3
#define MODE_BUTTON_PIN 4

#define LONG_PRESS_TIME 500L
#define DEBOUNCE_WAIT 0

#define CYCLE_TIME 10  // Time, in seconds, between auto-cycle images

//Adafruit_DotStar strip(NUM_LEDS, DATA_PIN, CLOCK_PIN, DOTSTAR_BGR);
Adafruit_DotStar strip(NUM_LEDS, DOTSTAR_BGR, &SPI);

LED_Effects effects(strip);
Images images(strip);
Battery battery(strip);

// GLOBAL -----
uint8_t debounce = 0;
int currentMode = 0;
bool autoCycle = false;

void setup() {
    // Initialize Serial communication
    //Serial.begin(9600);
#if defined(__AVR_ATmega328P__) && (F_CPU == 16000000L)
    clock_prescale_set(clock_div_1); // Enable 16 MHz on Metro Mini
#endif

    // Initialize the DotStar strip
    strip.begin();

    battery.showBatteryLevel();
    Battery::initializeChargingState();
    images.init();

    // Initialize IR receiver
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

    // Initialize mode button pin
    pinMode(MODE_BUTTON_PIN, INPUT_PULLUP);
    strip.setBrightness(20);
}

void toggleAutoCycle() {
    autoCycle = !autoCycle;

    if (autoCycle) {
        effects.flashIndicator(0, 255, 0, 3, 200);
    } else {
        effects.flashIndicator(255, 0, 0, 3, 200);
    }
}

void nextMode(uint8_t mode = 10) {
    currentMode = mode < MODE_COUNT ? mode : (currentMode + 1) % MODE_COUNT;
    images.setImages(currentMode);
    images.restartImage();
}

unsigned long sMillisOfFirstReceive;
bool sLongPressJustDetected;

bool detectLongPress(uint16_t aLongPressDurationMillis) {
    if (!sLongPressJustDetected && (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)) {
        /*
             * Here the repeat flag is set (which implies, that command is the same as the previous one)
             */
        if (millis() - aLongPressDurationMillis > sMillisOfFirstReceive) {
            sLongPressJustDetected = true; // Long press here
        }
    } else {
        // No repeat here
        sMillisOfFirstReceive = millis();
        sLongPressJustDetected = false;
    }
    return sLongPressJustDetected; // No long press here
}

void processModePress() {
    uint32_t time = millis();

    // Change images with mode btn
    if (digitalRead(MODE_BUTTON_PIN)) {
        // Image select?
        debounce = 0; // Not pressed -- reset counter
    } else {
        // Pressed...
        if (++debounce >= DEBOUNCE_WAIT) {
            // Debounce input
            images.nextImage(); // Switch to next image
            while (!digitalRead(MODE_BUTTON_PIN)); // Wait for release
            // If held 1+ sec, toggle auto-cycle mode on/off
            if ((millis() - time) >= LONG_PRESS_TIME) {
                // toggleAutoCycle();
                nextMode(currentMode + 1);
            }
            debounce = 0;
        }
    }
}

void processIRCommands(uint16_t command) {
    if (command != Remote::BTN_NONE && IrReceiver.decodedIRData.protocol != UNKNOWN) {
        switch (command) {
            case Remote::BTN_BRIGHT_UP:
                effects.adjustBrightness(15);
                break;
            case Remote::BTN_BRIGHT_DOWN:
                effects.adjustBrightness(-15);
                break;
            case Remote::BTN_FASTER:
                if (images.microDelay == 500) {
                    effects.flashIndicator(0, 255, 255, 4, 100);
                } else {
                    images.adjustLineInterval(1);
                }
                break;
            case Remote::BTN_SLOWER:
                if (images.microDelay == 1500) {
                    effects.flashIndicator(255, 0, 255, 4, 100);
                } else {
                    images.adjustLineInterval(2);
                }
                break;
            case Remote::BTN_RESTART:
                images.restartImage();
                break;
            case Remote::BTN_BATTERY:
                battery.showBatteryLevel();
                break;
            case Remote::BTN_OFF:
                strip.setBrightness(0);
                break;
            case Remote::BTN_PATTERN_PREV:
                images.prevImage();
                break;
            case Remote::BTN_PATTERN_NEXT:
                images.nextImage();
                break;
            case Remote::BTN_AUTOPLAY:
                toggleAutoCycle();
                break;
            case Remote::BTN_MODE:
                nextMode();
                break;
            case Remote::BTN_ONE:
                nextMode(0);
                break;
            case Remote::BTN_TWO:
                nextMode(1);
                break;
            case Remote::BTN_THREE:
                nextMode(2);
                break;
            case Remote::BTN_FOUR:
                nextMode(3);
                break;
                //      case Remote::BTN_FIVE:
                //        nextMode(4);
                //        break;
                //      case Remote::BTN_SIX:
                //        nextMode(5);
                //        break;
        }
        IrReceiver.decodedIRData.command = Remote::BTN_NONE;
    }
}

void IRInterrupt() {
    if (IrReceiver.decode()) {
        processIRCommands(IrReceiver.decodedIRData.command);
        IrReceiver.resume();

        if (detectLongPress(250)) {
            processIRCommands(IrReceiver.decodedIRData.command);
        }
    }
}

void loop() {
    uint32_t currentTime = millis(); // Current time, milliseconds

    //static unsigned long lastDebugPrint = 0;
    //if (currentTime - lastDebugPrint > 5000) {
    //    battery.printVoltageDebug();
    //    lastDebugPrint = currentTime;
    //}

    // Handle charging display - overrides normal operation when charging
    if (Battery::isCharging()) {
        battery.updateChargingDisplay();
    } else {
        // Normal operation when not charging
        if (autoCycle && (currentTime - images.lastImageTime) >= (CYCLE_TIME * 1000L)) {
            images.nextImage();
        }
        images.transferScanline();
    }

    processModePress();
    IRInterrupt();

    strip.show(); // Refresh LEDs
    delayMicroseconds(images.microDelay);
}
