#ifndef IMAGES_H
#define IMAGES_H

#include <Arduino.h>
#include "Adafruit_DotStar.h"
#include "graphics.h"

#define MODE_COUNT 5
#define MODE_PLAY 0
#define MODE_PARTY 1
#define MODE_PERF 2
#define MODE_STUTTER 3
#define MODE_CHASERS 4

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
  uint8_t lineIntervalIndex;
  uint16_t lineInterval;
  const PROGMEM Image *currentImages;
  uint8_t currentImageCount;
  uint16_t lineTable[5]{};

private:
  Adafruit_DotStar *stripObj;

  void transferPalette1();
  void transferPalette4();
  void transferPalette8();
  void transferTrueColor();
};

#endif  // IMAGES_H