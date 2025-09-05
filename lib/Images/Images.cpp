#include <Arduino.h>
#include "Images.h"

Images::Images(Adafruit_DotStar &strip) {
  stripObj = &strip;
  lastLineTime = 0L;
  lastImageTime = 0L;
  imageNumber = 0;
  microDelay = 900;
  currentImages = aImages;
  currentImageCount = A_NUM_IMAGES;
}

void Images::init() {
  // Initialize global image state for current imageNumber
  const Image PROGMEM *imageData = &currentImages[imageNumber];

  imageType = pgm_read_byte(&imageData->type);
  imageLines = pgm_read_word(&imageData->lines);
  imageLine = 0;
  imagePalette = (uint8_t *) pgm_read_word(&imageData->palette);
  imagePixels = (uint8_t *) pgm_read_word(&imageData->pixels);

  if (imageType == PALETTE1 || imageType == PALETTE4) {
    memcpy_P(palette, imagePalette, 6);
    memcpy_P(palette, imagePalette, 48);
  }

  lastImageTime = millis();  // Save time of image init for next auto-cycle
}

void Images::setImages(int mode) {
  switch (mode) {
    case 0:
      currentImages = aImages;
      currentImageCount = A_NUM_IMAGES;
      break;
    case 1:
      currentImages = bImages;
      currentImageCount = B_NUM_IMAGES;
      break;
    case 2:
      currentImages = cImages;
      currentImageCount = C_NUM_IMAGES;
      break;
    case 3:
      currentImages = dImages;
      currentImageCount = D_NUM_IMAGES;
      break;
//    case 4:
//      currentImages = eImages;
//      currentImageCount = E_NUM_IMAGES;
//      break;
//    case 5:
//      currentImages = fImages;
//      currentImageCount = F_NUM_IMAGES;
//      break;
  }
}

void Images::restartImage() {
  imageNumber = 0;
  init();
}

void Images::nextImage() {
  if (++imageNumber >= currentImageCount) imageNumber = 0;
  init();
}

void Images::prevImage() {
  imageNumber = imageNumber ? imageNumber - 1 : currentImageCount - 1;
  init();
}

void Images::transferScanline() {
  switch (imageType) {
    case PALETTE1:
      transferPalette1();
      break;
    case PALETTE4:
      transferPalette4();
      break;
    case PALETTE8:
      transferPalette8();
      break;
    case TRUECOLOR:
      transferTrueColor();
      break;
  }

  if (++imageLine >= imageLines) {
    imageLine = 0;  // Next scanline, wrap around
  }
}

void Images::adjustLineInterval(uint8_t direction) {
  if (direction == 1 && microDelay > 500) {
    microDelay -= 200;
  } else if (direction == 2 && microDelay < 1500) {
    microDelay += 200;
  }
}

void Images::transferPalette1() {
  uint8_t pixelNum = 0, byteNum, bitNum, pixels, idx, *ptr = (uint8_t *) &imagePixels[imageLine *
                                                                                      stripObj->numPixels() / 8];

  for (byteNum = stripObj->numPixels() / 8; byteNum--;) {  // Always padded to next byte
    pixels = pgm_read_byte(ptr++);            // 8 pixels of data (pixel 0 = LSB)
    for (bitNum = 8; bitNum--; pixels >>= 1) {
      idx = pixels & 1;  // Color table index for pixel (0 or 1)
      stripObj->setPixelColor(pixelNum++,
                              palette[idx][0], palette[idx][1], palette[idx][2]);
    }
  }
}

void Images::transferPalette4() {
  uint8_t pixelNum, p1, p2, *ptr = (uint8_t *) &imagePixels[imageLine * stripObj->numPixels() / 2];

  for (pixelNum = 0; pixelNum < stripObj->numPixels();) {
    p2 = pgm_read_byte(ptr++);  // Data for two pixels...
    p1 = p2 >> 4;               // Shift down 4 bits for first pixel
    p2 &= 0x0F;                 // Mask out low 4 bits for second pixel
    stripObj->setPixelColor(pixelNum++,
                            palette[p1][0], palette[p1][1], palette[p1][2]);
    stripObj->setPixelColor(pixelNum++,
                            palette[p2][0], palette[p2][1], palette[p2][2]);
  }
}

void Images::transferPalette8() {
  uint16_t o;
  uint16_t pixelNum;
  uint8_t *ptr = (uint8_t *) &imagePixels[imageLine * stripObj->numPixels()];

  for (pixelNum = 0; pixelNum < stripObj->numPixels(); pixelNum++) {
    o = pgm_read_byte(ptr++) * 3;  // Offset into imagePalette
    stripObj->setPixelColor(pixelNum,
                            pgm_read_byte(&imagePalette[o]),
                            pgm_read_byte(&imagePalette[o + 1]),
                            pgm_read_byte(&imagePalette[o + 2]));
  }
}

void Images::transferTrueColor() {
  uint16_t pixelNum, r, g, b, *ptr = (uint16_t *) &imagePixels[imageLine * stripObj->numPixels() * 3];

  for (pixelNum = 0; pixelNum < stripObj->numPixels(); pixelNum++) {
    r = pgm_read_byte(ptr++);
    g = pgm_read_byte(ptr++);
    b = pgm_read_byte(ptr++);
    stripObj->setPixelColor(pixelNum, r, g, b);
  }
}
