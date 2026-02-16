#ifndef ROBOTECH_PIXEL_H
#define ROBOTECH_PIXEL_H

#include <Arduino.h>
#include <FastLED.h>

// Board geometry constants
#define RP_NEORING_LEDS_PER_RING   48
#define RP_NEORING_LEDS_PER_BOARD  (RP_NEORING_LEDS_PER_RING * 2)
#define RP_QUADPIXEL_LEDS_PER_BOARD 4

enum BoardType {
  BOARD_NEORING,
  BOARD_QUADPIXEL
};

class RobotechPixel {
public:
  // Constructor
  //   boardType:  BOARD_NEORING or BOARD_QUADPIXEL
  //   numBoards:  number of daisy-chained PCBs
  //   pin:        data pin
  RobotechPixel(BoardType boardType, uint8_t numBoards, uint8_t pin);

  // Lifecycle
  void begin();
  void show();
  void clear();

  // Brightness
  void    setBrightness(uint8_t b);
  uint8_t getBrightness() const;

  // --- Low-Level LED Access ---

  // Set any LED by raw global index across the entire daisy chain
  void setLed(int globalIndex, uint32_t color);

  // Set LED by board index, ring index (0=inner, 1=outer), LED position
  void setRingLed(int pcbIndex, int ringIndex, int ledIndex, uint32_t color);

  // Overload: assumes pcbIndex = 0
  void setRingLed(int ringIndex, int ledIndex, uint32_t color);

  // Overload: assumes pcbIndex = 0, ringIndex = 0
  void setRingLed(int ledIndex, uint32_t color);

  // Color helper
  static uint32_t color(uint8_t r, uint8_t g, uint8_t b);

  // --- Geometry queries ---

  BoardType getBoardType()   const;
  uint8_t   getNumBoards()   const;
  uint16_t  getTotalPixels() const;
  uint16_t  getLedsPerRing() const;
  uint16_t  getLedsPerBoard() const;

  // --- Ring mapping (public for effects) ---

  int  innerPixel(int board, int idx) const;
  int  outerPixel(int board, int idx) const;
  void setPathPixel(int board, int pathPos, uint32_t color);

  // --- Pixel manipulation helpers ---

  void     fadeAll(uint8_t factor);
  void     fadePixel(int index, uint8_t factor);
  uint32_t getPixelColor(int index) const;

private:
  CRGB*             _leds;
  uint8_t           _pin;
  BoardType         _boardType;
  uint8_t           _numBoards;
  uint16_t          _totalPixels;
};

// Include effects so users get everything from one header
#include "NeoRingEffects.h"

#endif // ROBOTECH_PIXEL_H
