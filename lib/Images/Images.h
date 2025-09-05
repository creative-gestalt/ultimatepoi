#ifndef IMAGES_H
#define IMAGES_H

#include <Arduino.h>
#include "Adafruit_DotStar.h"
#include "graphics.h"

#define MODE_COUNT 4

class Images {

public:
    explicit Images(Adafruit_DotStar &strip);

    void init();

    void restartImage();

    void nextImage();

    void prevImage();

    void transferScanline();

    void adjustLineInterval(uint8_t delta);

    void setImages(int mode);

    uint32_t lastLineTime;
    uint32_t lastImageTime;
    uint8_t imageNumber;
    uint8_t imageType{};
    uint8_t *imagePalette{};
    uint8_t *imagePixels{};
    uint8_t palette[16][3]{};
    size_t imageLines{};
    size_t imageLine{};
    const PROGMEM Image *currentImages;
    uint8_t currentImageCount;
    int microDelay;

private:
    Adafruit_DotStar *stripObj;

    void transferPalette1();

    void transferPalette4();

    void transferPalette8();

    void transferTrueColor();
};

#endif  // IMAGES_H