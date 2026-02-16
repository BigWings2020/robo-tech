#include "RobotechPixel.h"

RobotechPixel::RobotechPixel(BoardType boardType, uint8_t numBoards, uint8_t pin)
  : _leds(nullptr),
    _pin(pin),
    _boardType(boardType),
    _numBoards(numBoards)
{
  uint16_t ledsPerBoard = (boardType == BOARD_NEORING)
    ? RP_NEORING_LEDS_PER_BOARD
    : RP_QUADPIXEL_LEDS_PER_BOARD;
  _totalPixels = numBoards * ledsPerBoard;
}

// --- Lifecycle ---

void RobotechPixel::begin() {
  _leds = new CRGB[_totalPixels];
  memset(_leds, 0, sizeof(CRGB) * _totalPixels);

  switch (_pin) {
    case 2:  FastLED.addLeds<WS2812B, 2,  GRB>(_leds, _totalPixels); break;
    case 3:  FastLED.addLeds<WS2812B, 3,  GRB>(_leds, _totalPixels); break;
    case 4:  FastLED.addLeds<WS2812B, 4,  GRB>(_leds, _totalPixels); break;
    case 5:  FastLED.addLeds<WS2812B, 5,  GRB>(_leds, _totalPixels); break;
    case 6:  FastLED.addLeds<WS2812B, 6,  GRB>(_leds, _totalPixels); break;
    case 7:  FastLED.addLeds<WS2812B, 7,  GRB>(_leds, _totalPixels); break;
    case 8:  FastLED.addLeds<WS2812B, 8,  GRB>(_leds, _totalPixels); break;
    case 9:  FastLED.addLeds<WS2812B, 9,  GRB>(_leds, _totalPixels); break;
    case 10: FastLED.addLeds<WS2812B, 10, GRB>(_leds, _totalPixels); break;
    case 11: FastLED.addLeds<WS2812B, 11, GRB>(_leds, _totalPixels); break;
    case 12: FastLED.addLeds<WS2812B, 12, GRB>(_leds, _totalPixels); break;
    case 13: FastLED.addLeds<WS2812B, 13, GRB>(_leds, _totalPixels); break;
  }

  FastLED.show();
}

void RobotechPixel::show()  { FastLED.show(); }
void RobotechPixel::clear() { fill_solid(_leds, _totalPixels, CRGB::Black); }

// --- Brightness ---

void    RobotechPixel::setBrightness(uint8_t b) { FastLED.setBrightness(b); }
uint8_t RobotechPixel::getBrightness() const     { return FastLED.getBrightness(); }

// --- Low-Level LED Access ---

void RobotechPixel::setLed(int globalIndex, uint32_t color) {
  if (globalIndex >= 0 && globalIndex < _totalPixels) {
    _leds[globalIndex] = CRGB(
      (color >> 16) & 0xFF,
      (color >> 8)  & 0xFF,
       color        & 0xFF
    );
  }
}

void RobotechPixel::setRingLed(int pcbIndex, int ringIndex, int ledIndex, uint32_t color) {
  if (_boardType == BOARD_NEORING) {
    int base = pcbIndex * RP_NEORING_LEDS_PER_BOARD;
    int offset = (ringIndex == 0)
      ? (ledIndex % RP_NEORING_LEDS_PER_RING)
      : RP_NEORING_LEDS_PER_RING + (ledIndex % RP_NEORING_LEDS_PER_RING);
    setLed(base + offset, color);
  } else {
    // QuadPixel: ringIndex ignored, just board + led
    setLed(pcbIndex * RP_QUADPIXEL_LEDS_PER_BOARD + ledIndex, color);
  }
}

void RobotechPixel::setRingLed(int ringIndex, int ledIndex, uint32_t color) {
  setRingLed(0, ringIndex, ledIndex, color);
}

void RobotechPixel::setRingLed(int ledIndex, uint32_t color) {
  setRingLed(0, 0, ledIndex, color);
}

uint32_t RobotechPixel::color(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

// --- Geometry ---

BoardType RobotechPixel::getBoardType()   const { return _boardType; }
uint8_t   RobotechPixel::getNumBoards()   const { return _numBoards; }
uint16_t  RobotechPixel::getTotalPixels() const { return _totalPixels; }
uint16_t  RobotechPixel::getLedsPerRing() const { return RP_NEORING_LEDS_PER_RING; }

uint16_t RobotechPixel::getLedsPerBoard() const {
  return (_boardType == BOARD_NEORING) ? RP_NEORING_LEDS_PER_BOARD : RP_QUADPIXEL_LEDS_PER_BOARD;
}

// --- Ring Mapping ---

int RobotechPixel::innerPixel(int board, int idx) const {
  return (board * RP_NEORING_LEDS_PER_BOARD) + (idx % RP_NEORING_LEDS_PER_RING);
}

int RobotechPixel::outerPixel(int board, int idx) const {
  return (board * RP_NEORING_LEDS_PER_BOARD) + RP_NEORING_LEDS_PER_RING
         + (idx % RP_NEORING_LEDS_PER_RING);
}

void RobotechPixel::setPathPixel(int board, int pathPos, uint32_t color) {
  pathPos = pathPos % (RP_NEORING_LEDS_PER_RING * 2);
  if (pathPos < RP_NEORING_LEDS_PER_RING)
    setLed(innerPixel(board, pathPos), color);
  else
    setLed(outerPixel(board, pathPos - RP_NEORING_LEDS_PER_RING), color);
}

// --- Pixel Manipulation ---

void RobotechPixel::fadeAll(uint8_t factor) {
  for (int i = 0; i < _totalPixels; i++)
    _leds[i].nscale8(factor);
}

void RobotechPixel::fadePixel(int index, uint8_t factor) {
  if (index >= 0 && index < _totalPixels)
    _leds[index].nscale8(factor);
}

uint32_t RobotechPixel::getPixelColor(int index) const {
  if (index >= 0 && index < _totalPixels) {
    return ((uint32_t)_leds[index].r << 16) |
           ((uint32_t)_leds[index].g << 8)  |
            (uint32_t)_leds[index].b;
  }
  return 0;
}
