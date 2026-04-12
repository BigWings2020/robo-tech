#ifndef ROBOTECH_PIXEL_H
#define ROBOTECH_PIXEL_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// =====================================================
//  Board definition system — add new boards here
// =====================================================

#define RP_MAX_BOARDS        8
#define RP_MAX_LEDS_PER_RING 60

// Compact geometry descriptor (lives in PROGMEM)
struct BoardDef {
  uint8_t  ledsPerRing;   // 0 for non-ring boards
  uint8_t  numRings;      // 0 = flat pixel board
  uint16_t totalLeds;     // ledsPerRing * numRings, or raw count
};

enum BoardType : uint8_t {
  BOARD_NEORING,       // 2 rings x 48 LEDs = 96
  BOARD_NEORING_XL,    // 3 rings x 60 LEDs = 180
  BOARD_QUADPIXEL,     // 4 LEDs, no rings
  BOARD_TYPE_COUNT
};

extern const BoardDef BOARD_DEFS[BOARD_TYPE_COUNT] PROGMEM;

// Read a BoardDef from PROGMEM
static inline BoardDef rpGetBoardDef(BoardType t) {
  BoardDef d;
  memcpy_P(&d, &BOARD_DEFS[t], sizeof(BoardDef));
  return d;
}

// Legacy constants (kept for backward compatibility)
#define RP_NEORING_LEDS_PER_RING   48
#define RP_NEORING_LEDS_PER_BOARD  (RP_NEORING_LEDS_PER_RING * 2)
#define RP_QUADPIXEL_LEDS_PER_BOARD 4

// =====================================================
//  RobotechPixel class
// =====================================================

class RobotechPixel {
public:
  // Uniform chain — all boards same type (backward compatible)
  RobotechPixel(BoardType boardType, uint8_t numBoards, uint8_t pin);

  // Mixed chain — each board can be a different type
  RobotechPixel(const BoardType* types, uint8_t numBoards, uint8_t pin);

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

  // Set LED by board index, ring index, LED position
  void setRingLed(int pcbIndex, int ringIndex, int ledIndex, uint32_t color);

  // Overload: assumes pcbIndex = 0
  void setRingLed(int ringIndex, int ledIndex, uint32_t color);

  // Overload: assumes pcbIndex = 0, ringIndex = 0
  void setRingLed(int ledIndex, uint32_t color);

  // Color helper
  static uint32_t color(uint8_t r, uint8_t g, uint8_t b);

  // --- Per-board geometry queries ---

  BoardType getBoardType(uint8_t board) const;
  uint8_t   getLedsPerRing(uint8_t board) const;
  uint8_t   getNumRings(uint8_t board) const;
  uint16_t  getLedsPerBoard(uint8_t board) const;
  uint16_t  getBoardOffset(uint8_t board) const;
  bool      boardHasRings(uint8_t board) const;

  // --- Backward-compatible scalar queries (return board 0 values) ---

  BoardType getBoardType()   const;
  uint8_t   getNumBoards()   const;
  uint16_t  getTotalPixels() const;
  uint16_t  getLedsPerRing() const;
  uint16_t  getLedsPerBoard() const;

  // --- Ring mapping ---

  int  innerPixel(int board, int idx) const;
  int  outerPixel(int board, int idx) const;
  void setPathPixel(int board, int pathPos, uint32_t color);

  // --- Pixel manipulation helpers ---

  void     fadeAll(uint8_t factor);
  void     fadePixel(int index, uint8_t factor);
  uint32_t getPixelColor(int index) const;

private:
  Adafruit_NeoPixel _strip;
  uint8_t           _numBoards;
  uint16_t          _totalPixels;
  BoardType         _types[RP_MAX_BOARDS];
  uint16_t          _offsets[RP_MAX_BOARDS];

  void _computeOffsets();
};

// Include effects so users get everything from one header
#include "NeoRingEffects.h"

#endif // ROBOTECH_PIXEL_H
