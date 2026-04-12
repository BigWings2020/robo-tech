#include "RobotechPixel.h"
#include "RobotechPixelHelpers.h"

namespace {
  enum RoulettePhase { RP_SPINNING, RP_FALLING, RP_SETTLED, RP_RESTART };
  enum WheelPhase    { WP_SPINNING, WP_SETTLED, WP_RESTART };
}

// ============================================================
//  ROULETTE — spinning ball slows and settles into a pocket
// ============================================================

void fxRoulette(RobotechPixel& px, unsigned long now, int wait) {

  static float    ballPos     = 0.0f;
  static float    velocity    = 0.0f;
  static RoulettePhase phase  = RP_RESTART;
  static unsigned long lastUpdate  = 0;
  static unsigned long phaseStart  = 0;

  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  // Use board 0's ring size for the simulation
  const uint8_t N = px.getLedsPerRing(0);

  switch (phase) {
    case RP_RESTART:
      velocity  = 2.5f + (random(0, 150) / 100.0f);
      ballPos   = (float)random(0, N);
      phase     = RP_SPINNING;
      phaseStart = now;
      break;

    case RP_SPINNING:
      ballPos += velocity;
      while (ballPos >= (float)N) ballPos -= (float)N;
      velocity *= 0.99f;
      if (velocity < 0.02f) {
        velocity = 0.0f;
        ballPos  = (float)((int)(ballPos + 0.5f) % N);
        phase    = RP_FALLING;
        phaseStart = now;
      }
      break;

    case RP_FALLING:
      if (now - phaseStart > 400) {
        phase    = RP_SETTLED;
        phaseStart = now;
      }
      break;

    case RP_SETTLED:
      if (now - phaseStart > 3000)
        phase = RP_RESTART;
      break;
  }

  px.clear();

  for (int bd = 0; bd < px.getNumBoards(); bd++) {
    if (!px.boardHasRings(bd)) continue;
    uint8_t lpr = px.getLedsPerRing(bd);

    // Outer ring: roulette pockets
    for (int i = 0; i < lpr; i++) {
      if (i == 0) {
        px.setRingLed(bd, 1, i, RobotechPixel::color(0, 100, 0));
      } else if (i & 1) {
        px.setRingLed(bd, 1, i, RobotechPixel::color(160, 0, 0));
      }
    }

    // Inner ring: grey tick marks
    for (int i = 0; i < lpr; i += 2)
      px.setRingLed(bd, 0, i, RobotechPixel::color(5, 5, 5));

    // Ball rendering — map position proportionally
    int pocket = (int)ballPos % lpr;

    if (phase == RP_SPINNING) {
      px.setRingLed(bd, 0, pocket, RobotechPixel::color(255, 255, 255));
    } else if (phase == RP_FALLING) {
      if (now - phaseStart < 200) {
        px.setRingLed(bd, 0, pocket, RobotechPixel::color(255, 255, 255));
      } else {
        px.setRingLed(bd, 1, pocket, RobotechPixel::color(255, 255, 255));
      }
    } else if (phase == RP_SETTLED) {
      px.setRingLed(bd, 1, pocket, RobotechPixel::color(255, 255, 255));
    }
  }

  px.show();
}

// ============================================================
//  WHEEL OF FORTUNE — colored segments spin past a pointer
// ============================================================

void fxWheelOfFortune(RobotechPixel& px, unsigned long now,
                      int numCells, int wait) {

  static float    offset    = 0.0f;
  static float    velocity  = 0.0f;
  static WheelPhase phase   = WP_RESTART;
  static unsigned long lastUpdate = 0;
  static unsigned long phaseStart = 0;

  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  const uint8_t N = px.getLedsPerRing(0);
  numCells = constrain(numCells, 2, (int)N);
  float cellSize = (float)N / numCells;

  static const uint32_t cellColors[] = {
    0xFF0000, 0x00C800, 0xFFC800, 0x0000FF
  };
  const int paletteSize = sizeof(cellColors) / sizeof(cellColors[0]);

  switch (phase) {
    case WP_RESTART:
      velocity  = 3.0f + (random(0, 200) / 100.0f);
      offset    = (float)random(0, N);
      phase     = WP_SPINNING;
      phaseStart = now;
      break;

    case WP_SPINNING:
      offset += velocity;
      while (offset >= (float)N) offset -= (float)N;
      velocity *= 0.99f;
      if (velocity < 0.02f) {
        velocity = 0.0f;
        phase    = WP_SETTLED;
        phaseStart = now;
      }
      break;

    case WP_SETTLED:
      if (now - phaseStart > 3000)
        phase = WP_RESTART;
      break;
  }

  float rotZero = (float)0 - offset;
  while (rotZero < 0) rotZero += N;
  int winCell = (int)(rotZero / cellSize) % numCells;

  bool flashOn = true;
  if (phase == WP_SETTLED)
    flashOn = ((now - phaseStart) / 250) % 2 == 0;

  px.clear();

  for (int bd = 0; bd < px.getNumBoards(); bd++) {
    if (!px.boardHasRings(bd)) continue;
    uint8_t lpr = px.getLedsPerRing(bd);

    for (int i = 0; i < lpr; i++) {
      // Map this board's LED to the simulation coordinate
      float simPos = (float)i * N / lpr;
      float rotPos = simPos - offset;
      while (rotPos < 0) rotPos += N;
      int cell = (int)(rotPos / cellSize) % numCells;

      uint32_t raw = cellColors[cell % paletteSize];
      uint8_t r = (raw >> 16) & 0xFF;
      uint8_t g = (raw >>  8) & 0xFF;
      uint8_t b =  raw        & 0xFF;

      float bright = 0.5f;
      if (phase == WP_SETTLED && cell == winCell)
        bright = flashOn ? 1.0f : 0.15f;

      uint32_t c = RobotechPixel::color(
        (uint8_t)(r * bright),
        (uint8_t)(g * bright),
        (uint8_t)(b * bright));

      px.setRingLed(bd, 0, i, c);
      px.setRingLed(bd, 1, i, c);
    }

    // Pointer
    px.setRingLed(bd, 0, 0, RobotechPixel::color(255, 255, 255));
    px.setRingLed(bd, 1, 0, RobotechPixel::color(255, 255, 255));
  }

  px.show();
}
