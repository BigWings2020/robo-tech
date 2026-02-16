#include "RobotechPixel.h"
#include "RobotechPixelHelpers.h"

// ============================================================
//  ROULETTE — spinning ball slows and settles into a pocket
//
//  Outer ring: alternating red / black (off) pockets + green 0
//  Inner ring: grey tick marks + white ball with trail
// ============================================================

void fxRoulette(RobotechPixel& px, unsigned long now, int wait) {
  enum Phase : uint8_t { SPINNING, FALLING, SETTLED, RESTART };

  static float    ballPos     = 0.0f;
  static float    velocity    = 0.0f;
  static Phase    phase       = RESTART;
  static unsigned long lastUpdate  = 0;
  static unsigned long phaseStart  = 0;

  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  const uint16_t N = px.getLedsPerRing();   // 48

  // ---- state machine ----
  switch (phase) {
    case RESTART:
      velocity  = 2.5f + (random(0, 150) / 100.0f);   // 2.5 – 4.0 px/frame
      ballPos   = (float)random(0, N);
      phase     = SPINNING;
      phaseStart = now;
      break;

    case SPINNING:
      ballPos += velocity;
      while (ballPos >= (float)N) ballPos -= (float)N;

      velocity *= 0.99f;                               // exponential decel

      if (velocity < 0.02f) {
        velocity = 0.0f;
        ballPos  = (float)((int)(ballPos + 0.5f) % N); // snap to nearest LED
        phase    = FALLING;
        phaseStart = now;
      }
      break;

    case FALLING:
      if (now - phaseStart > 400) {                     // brief pause then land
        phase    = SETTLED;
        phaseStart = now;
      }
      break;

    case SETTLED:
      if (now - phaseStart > 3000)                      // hold result 3 s
        phase = RESTART;
      break;
  }

  // ---- render ----
  px.clear();

  for (int bd = 0; bd < px.getNumBoards(); bd++) {

    // --- outer ring: roulette pockets ---
    for (int i = 0; i < N; i++) {
      if (i == 0) {
        px.setRingLed(bd, 1, i, RobotechPixel::color(0, 100, 0));   // green 0
      } else if (i & 1) {
        px.setRingLed(bd, 1, i, RobotechPixel::color(160, 0, 0));   // red
      }
      // even (except 0) stay off → black pockets
    }

    // --- inner ring: grey tick marks at every other LED ---
    for (int i = 0; i < N; i += 2)
      px.setRingLed(bd, 0, i, RobotechPixel::color(5, 5, 5));

    // --- ball rendering ---
    int pocket = (int)ballPos % N;

    if (phase == SPINNING) {
      // single bright pixel on inner ring
      px.setRingLed(bd, 0, pocket, RobotechPixel::color(255, 255, 255));

    } else if (phase == FALLING) {
      // ball visible on inner ring, then jumps to outer after half the pause
      if (now - phaseStart < 200) {
        px.setRingLed(bd, 0, pocket, RobotechPixel::color(255, 255, 255));
      } else {
        px.setRingLed(bd, 1, pocket, RobotechPixel::color(255, 255, 255));
      }

    } else if (phase == SETTLED) {
      // ball sits in the pocket on the outer ring
      px.setRingLed(bd, 1, pocket, RobotechPixel::color(255, 255, 255));
    }
  }

  px.show();
}

// ============================================================
//  WHEEL OF FORTUNE — colored segments spin past a pointer
//
//  Both rings: colored cells, 50% brightness
//  White pointer line at index 0 drawn on top of the colors
// ============================================================

void fxWheelOfFortune(RobotechPixel& px, unsigned long now,
                      int numCells, int wait) {
  enum Phase : uint8_t { SPINNING, SETTLED, RESTART };

  static float    offset    = 0.0f;
  static float    velocity  = 0.0f;
  static Phase    phase     = RESTART;
  static unsigned long lastUpdate = 0;
  static unsigned long phaseStart = 0;

  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  const uint16_t N = px.getLedsPerRing();          // 48
  numCells = constrain(numCells, 2, (int)N);
  float cellSize = (float)N / numCells;

  // four distinct colors, ordered for maximum neighbor contrast
  static const uint32_t cellColors[] = {
    0xFF0000,   // red
    0x00C800,   // green
    0xFFC800,   // yellow
    0x0000FF    // blue
  };
  const int paletteSize = sizeof(cellColors) / sizeof(cellColors[0]);

  // ---- state machine ----
  switch (phase) {
    case RESTART:
      velocity  = 3.0f + (random(0, 200) / 100.0f);  // 3.0 – 5.0
      offset    = (float)random(0, N);
      phase     = SPINNING;
      phaseStart = now;
      break;

    case SPINNING:
      offset += velocity;
      while (offset >= (float)N) offset -= (float)N;

      velocity *= 0.99f;

      if (velocity < 0.02f) {
        velocity = 0.0f;
        phase    = SETTLED;
        phaseStart = now;
      }
      break;

    case SETTLED:
      if (now - phaseStart > 3000)
        phase = RESTART;
      break;
  }

  // ---- determine winning cell (whichever is at index 0) ----
  float rotZero = (float)0 - offset;
  while (rotZero < 0) rotZero += N;
  int winCell = (int)(rotZero / cellSize) % numCells;

  // flash toggle for settled phase
  bool flashOn = true;
  if (phase == SETTLED)
    flashOn = ((now - phaseStart) / 250) % 2 == 0;

  // ---- render ----
  px.clear();

  for (int bd = 0; bd < px.getNumBoards(); bd++) {

    // both rings: colored wheel segments
    for (int i = 0; i < N; i++) {
      float rotPos = (float)i - offset;
      while (rotPos < 0) rotPos += N;
      int cell = (int)(rotPos / cellSize) % numCells;

      uint32_t raw = cellColors[cell % paletteSize];
      uint8_t r = (raw >> 16) & 0xFF;
      uint8_t g = (raw >>  8) & 0xFF;
      uint8_t b =  raw        & 0xFF;

      float bright = 0.5f;

      // settled: winning cell flashes between full and dark
      if (phase == SETTLED && cell == winCell)
        bright = flashOn ? 1.0f : 0.15f;

      uint32_t c = RobotechPixel::color(
        (uint8_t)(r * bright),
        (uint8_t)(g * bright),
        (uint8_t)(b * bright));

      px.setRingLed(bd, 0, i, c);   // inner ring
      px.setRingLed(bd, 1, i, c);   // outer ring
    }

    // pointer: white line at index 0, drawn on top of colors
    px.setRingLed(bd, 0, 0, RobotechPixel::color(255, 255, 255));
    px.setRingLed(bd, 1, 0, RobotechPixel::color(255, 255, 255));
  }

  px.show();
}
