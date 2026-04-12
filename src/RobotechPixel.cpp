#include "RobotechPixel.h"

// =====================================================
//  Board definitions (PROGMEM)
// =====================================================

const BoardDef BOARD_DEFS[BOARD_TYPE_COUNT] PROGMEM = {
//  ledsPerRing  numRings  totalLeds
  {  48,           2,        96 },    // BOARD_NEORING
  {  60,           3,       180 },    // BOARD_NEORING_XL
  {   0,           0,         4 },    // BOARD_QUADPIXEL
};

// =====================================================
//  Constructors
// =====================================================

// Uniform chain (backward compatible)
RobotechPixel::RobotechPixel(BoardType boardType, uint8_t numBoards, uint8_t pin)
  : _strip(0, pin, NEO_GRB + NEO_KHZ800),
    _numBoards(min(numBoards, (uint8_t)RP_MAX_BOARDS))
{
  for (uint8_t i = 0; i < _numBoards; i++) _types[i] = boardType;
  _computeOffsets();
}

// Mixed chain
RobotechPixel::RobotechPixel(const BoardType* types, uint8_t numBoards, uint8_t pin)
  : _strip(0, pin, NEO_GRB + NEO_KHZ800),
    _numBoards(min(numBoards, (uint8_t)RP_MAX_BOARDS))
{
  for (uint8_t i = 0; i < _numBoards; i++) _types[i] = types[i];
  _computeOffsets();
}

void RobotechPixel::_computeOffsets() {
  _totalPixels = 0;
  for (uint8_t i = 0; i < _numBoards; i++) {
    _offsets[i] = _totalPixels;
    BoardDef def = rpGetBoardDef(_types[i]);
    _totalPixels += def.totalLeds;
  }
  _strip.updateLength(_totalPixels);
}

// --- Lifecycle ---

void RobotechPixel::begin() {
  _strip.begin();
  _strip.show();
}

void RobotechPixel::show()  { _strip.show(); }
void RobotechPixel::clear() { _strip.clear(); }

// --- Brightness ---

void    RobotechPixel::setBrightness(uint8_t b) { _strip.setBrightness(b); }
uint8_t RobotechPixel::getBrightness() const     { return _strip.getBrightness(); }

// --- Low-Level LED Access ---

void RobotechPixel::setLed(int globalIndex, uint32_t color) {
  if (globalIndex >= 0 && globalIndex < _totalPixels)
    _strip.setPixelColor(globalIndex, color);
}

void RobotechPixel::setRingLed(int pcbIndex, int ringIndex, int ledIndex, uint32_t color) {
  if (pcbIndex < 0 || pcbIndex >= _numBoards) return;
  BoardDef def = rpGetBoardDef(_types[pcbIndex]);
  if (def.numRings == 0) {
    // Non-ring board: ignore ringIndex
    setLed(_offsets[pcbIndex] + (ledIndex % def.totalLeds), color);
    return;
  }
  if (ringIndex < 0 || ringIndex >= def.numRings) return;
  int base = _offsets[pcbIndex];
  int offset = ringIndex * def.ledsPerRing + ((ledIndex % def.ledsPerRing + def.ledsPerRing) % def.ledsPerRing);
  setLed(base + offset, color);
}

void RobotechPixel::setRingLed(int ringIndex, int ledIndex, uint32_t color) {
  setRingLed(0, ringIndex, ledIndex, color);
}

void RobotechPixel::setRingLed(int ledIndex, uint32_t color) {
  setRingLed(0, 0, ledIndex, color);
}

uint32_t RobotechPixel::color(uint8_t r, uint8_t g, uint8_t b) {
  return Adafruit_NeoPixel::Color(r, g, b);
}

// --- Per-board geometry queries ---

BoardType RobotechPixel::getBoardType(uint8_t board) const {
  return (board < _numBoards) ? _types[board] : _types[0];
}

uint8_t RobotechPixel::getLedsPerRing(uint8_t board) const {
  BoardDef def = rpGetBoardDef((board < _numBoards) ? _types[board] : _types[0]);
  return def.ledsPerRing;
}

uint8_t RobotechPixel::getNumRings(uint8_t board) const {
  BoardDef def = rpGetBoardDef((board < _numBoards) ? _types[board] : _types[0]);
  return def.numRings;
}

uint16_t RobotechPixel::getLedsPerBoard(uint8_t board) const {
  BoardDef def = rpGetBoardDef((board < _numBoards) ? _types[board] : _types[0]);
  return def.totalLeds;
}

uint16_t RobotechPixel::getBoardOffset(uint8_t board) const {
  return (board < _numBoards) ? _offsets[board] : 0;
}

bool RobotechPixel::boardHasRings(uint8_t board) const {
  return getNumRings(board) > 0;
}

// --- Backward-compatible scalar queries (board 0) ---

BoardType RobotechPixel::getBoardType()   const { return _types[0]; }
uint8_t   RobotechPixel::getNumBoards()   const { return _numBoards; }
uint16_t  RobotechPixel::getTotalPixels() const { return _totalPixels; }
uint16_t  RobotechPixel::getLedsPerRing() const { return getLedsPerRing(0); }

uint16_t RobotechPixel::getLedsPerBoard() const {
  return getLedsPerBoard(0);
}

// --- Ring Mapping ---

int RobotechPixel::innerPixel(int board, int idx) const {
  if (board < 0 || board >= _numBoards) return 0;
  BoardDef def = rpGetBoardDef(_types[board]);
  return _offsets[board] + (idx % def.ledsPerRing);
}

int RobotechPixel::outerPixel(int board, int idx) const {
  if (board < 0 || board >= _numBoards) return 0;
  BoardDef def = rpGetBoardDef(_types[board]);
  return _offsets[board] + def.ledsPerRing + (idx % def.ledsPerRing);
}

void RobotechPixel::setPathPixel(int board, int pathPos, uint32_t color) {
  if (board < 0 || board >= _numBoards) return;
  BoardDef def = rpGetBoardDef(_types[board]);
  if (def.numRings == 0) return;
  uint16_t pathLen = def.ledsPerRing * def.numRings;
  pathPos = ((pathPos % pathLen) + pathLen) % pathLen;
  int ring = pathPos / def.ledsPerRing;
  int led  = pathPos % def.ledsPerRing;
  setRingLed(board, ring, led, color);
}

// --- Pixel Manipulation ---

void RobotechPixel::fadeAll(uint8_t factor) {
  for (int i = 0; i < _totalPixels; i++)
    fadePixel(i, factor);
}

void RobotechPixel::fadePixel(int index, uint8_t factor) {
  uint32_t c = _strip.getPixelColor(index);
  uint8_t r = (c >> 16) & 0xFF;
  uint8_t g = (c >> 8)  & 0xFF;
  uint8_t b =  c        & 0xFF;
  _strip.setPixelColor(index, Adafruit_NeoPixel::Color(
    (r * factor) >> 8,
    (g * factor) >> 8,
    (b * factor) >> 8
  ));
}

uint32_t RobotechPixel::getPixelColor(int index) const {
  return _strip.getPixelColor(index);
}
