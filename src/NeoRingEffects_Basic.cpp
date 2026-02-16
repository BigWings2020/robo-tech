#include "RobotechPixel.h"
#include "RobotechPixelHelpers.h"

// ============================================================
//  1: COLOR CYCLE — cycle through 8 solid colors
// ============================================================
void fxColorCycle(RobotechPixel& px, unsigned long now, int interval) {
  static uint8_t colorIdx = 255;
  static unsigned long lastUpdate = 0;

  if (now - lastUpdate < (unsigned long)interval && colorIdx != 255) return;
  lastUpdate = now;

  const uint32_t colors[] = {
    RobotechPixel::color(0, 0, 255),     RobotechPixel::color(0, 255, 0),
    RobotechPixel::color(255, 255, 0),   RobotechPixel::color(0, 255, 255),
    RobotechPixel::color(255, 80, 0),    RobotechPixel::color(255, 0, 255),
    RobotechPixel::color(255, 0, 0),     RobotechPixel::color(255, 255, 255),
  };

  colorIdx = (colorIdx == 255) ? 0 : (colorIdx + 1) % 8;
  uint16_t total = px.getTotalPixels();
  for (int i = 0; i < total; i++) px.setLed(i, colors[colorIdx]);
  px.show();
}

// ============================================================
//  2: PIXEL WALK — single LED walks around both rings
// ============================================================
void fxPixelWalk(RobotechPixel& px, unsigned long now,
                 uint32_t innerColor, uint32_t outerColor, int wait) {
  static int pos = 0;
  static unsigned long lastUpdate = 0;

  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  px.clear();
  for (int b = 0; b < px.getNumBoards(); b++) {
    px.setRingLed(b, 0, pos, innerColor);
    px.setRingLed(b, 1, pos, outerColor);
  }
  px.show();
  pos = (pos + 1) % px.getLedsPerRing();
}

// ============================================================
//  3: RADAR SWEEP — fading tail sweeps around ring
// ============================================================
void fxRadarSweep(RobotechPixel& px, unsigned long now,
                  uint32_t color, int tailLen, int wait) {
  static int head = 0;
  static unsigned long lastUpdate = 0;

  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8)  & 0xFF;
  uint8_t b =  color        & 0xFF;
  uint16_t ledsPerRing = px.getLedsPerRing();

  px.clear();
  for (int bd = 0; bd < px.getNumBoards(); bd++) {
    for (int j = 0; j < tailLen; j++) {
      int idx = (head - j + ledsPerRing) % ledsPerRing;
      uint8_t fade = 255 - (j * 255 / tailLen);
      uint32_t c = RobotechPixel::color((r * fade) >> 8, (g * fade) >> 8, (b * fade) >> 8);
      px.setRingLed(bd, 0, idx, c);
      px.setRingLed(bd, 1, idx, c);
    }
  }
  px.show();
  head = (head + 1) % ledsPerRing;
}

// ============================================================
//  4: DUAL SPIN — counter-rotating arcs on inner/outer rings
// ============================================================
void fxDualSpin(RobotechPixel& px, unsigned long now,
                uint32_t innerColor, uint32_t outerColor,
                int arcLen, int wait) {
  static int pos = 0;
  static unsigned long lastUpdate = 0;

  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  uint8_t ir = (innerColor >> 16) & 0xFF;
  uint8_t ig = (innerColor >> 8)  & 0xFF;
  uint8_t ib =  innerColor        & 0xFF;
  uint8_t or_ = (outerColor >> 16) & 0xFF;
  uint8_t og  = (outerColor >> 8)  & 0xFF;
  uint8_t ob  =  outerColor        & 0xFF;
  uint16_t ledsPerRing = px.getLedsPerRing();

  px.clear();
  for (int b = 0; b < px.getNumBoards(); b++) {
    for (int j = 0; j < arcLen; j++) {
      uint8_t fade = 255 - (j * 255 / arcLen);
      int iIdx = (pos - j + ledsPerRing) % ledsPerRing;
      px.setRingLed(b, 0, iIdx, RobotechPixel::color(
        (ir * fade) >> 8, (ig * fade) >> 8, (ib * fade) >> 8));
      int oIdx = (ledsPerRing - pos - j + ledsPerRing * 2) % ledsPerRing;
      px.setRingLed(b, 1, oIdx, RobotechPixel::color(
        (or_ * fade) >> 8, (og * fade) >> 8, (ob * fade) >> 8));
    }
  }
  px.show();
  pos = (pos + 1) % ledsPerRing;
}

// ============================================================
//  5: RIPPLE PULSE — phase-shifted breathing inner/outer
// ============================================================
void fxRipplePulse(RobotechPixel& px, unsigned long now,
                   uint32_t innerColor, uint32_t outerColor, int wait) {
  static uint8_t phase = 0;
  static unsigned long lastUpdate = 0;

  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  uint8_t ir = (innerColor >> 16) & 0xFF;
  uint8_t ig = (innerColor >> 8)  & 0xFF;
  uint8_t ib =  innerColor        & 0xFF;
  uint8_t or_ = (outerColor >> 16) & 0xFF;
  uint8_t og  = (outerColor >> 8)  & 0xFF;
  uint8_t ob  =  outerColor        & 0xFF;

  uint8_t innerVal = rpTriWave(phase);
  uint8_t outerVal = rpTriWave((uint8_t)(phase + 192));

  for (int b = 0; b < px.getNumBoards(); b++) {
    for (int i = 0; i < px.getLedsPerRing(); i++) {
      px.setRingLed(b, 0, i, RobotechPixel::color(
        (ir * innerVal) >> 8, (ig * innerVal) >> 8, (ib * innerVal) >> 8));
      px.setRingLed(b, 1, i, RobotechPixel::color(
        (or_ * outerVal) >> 8, (og * outerVal) >> 8, (ob * outerVal) >> 8));
    }
  }
  px.show();
  phase += 2;
}

// ============================================================
//  6: THEATER CHASE — classic chase pattern
// ============================================================
void fxTheaterChase(RobotechPixel& px, unsigned long now,
                    uint32_t color, int wait) {
  static int step = 0;
  static unsigned long lastUpdate = 0;

  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  px.clear();
  for (int b = 0; b < px.getNumBoards(); b++) {
    for (int i = step % 3; i < px.getLedsPerRing(); i += 3) {
      px.setRingLed(b, 0, i, color);
      px.setRingLed(b, 1, i, color);
    }
  }
  px.show();
  step = (step + 1) % 3;
}

// ============================================================
//  7: SPARKLE — random bright sparkles with fade
// ============================================================
void fxSparkle(RobotechPixel& px, unsigned long now,
               int numSparks, uint8_t fadeFactor, int wait) {
  static unsigned long lastUpdate = 0;

  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  px.fadeAll(fadeFactor);
  uint16_t total = px.getTotalPixels();
  for (int s = 0; s < numSparks; s++)
    px.setLed(random(total), rpWheel(random(256)));
  px.show();
}

// ============================================================
//  8: RAINBOW WHEEL — full HSV rainbow rotation
// ============================================================
void fxRainbowWheel(RobotechPixel& px, unsigned long now, int wait) {
  static uint16_t j = 0;
  static unsigned long lastUpdate = 0;

  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  uint16_t ledsPerRing = px.getLedsPerRing();
  for (int b = 0; b < px.getNumBoards(); b++) {
    for (int i = 0; i < ledsPerRing; i++) {
      uint32_t c = rpWheel(((i * 256 / ledsPerRing) + j) & 255);
      px.setRingLed(b, 0, i, c);
      px.setRingLed(b, 1, i, c);
    }
  }
  px.show();
  j++;
}

// ============================================================
//  9: FIRE RING — heat-based fire simulation
// ============================================================
void fxFireRing(RobotechPixel& px, unsigned long now,
                int cooling, int sparking, int wait) {
  static byte heat[RP_NEORING_LEDS_PER_RING];
  static unsigned long lastUpdate = 0;

  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  uint16_t ledsPerRing = px.getLedsPerRing();

  // Cool down
  for (int i = 0; i < ledsPerRing; i++) {
    int cooldown = random(0, cooling);
    heat[i] = (heat[i] > cooldown) ? heat[i] - cooldown : 0;
  }

  // Spark
  for (int k = 0; k < sparking; k++) {
    int pos = random(ledsPerRing);
    heat[pos] = min(255, heat[pos] + random(60, 160));
  }

  // Smooth
  byte smoothed[RP_NEORING_LEDS_PER_RING];
  for (int i = 0; i < ledsPerRing; i++) {
    int prev = (i - 1 + ledsPerRing) % ledsPerRing;
    int next = (i + 1) % ledsPerRing;
    smoothed[i] = ((int)heat[prev] + (int)heat[i] * 2 + (int)heat[next]) / 4;
  }
  memcpy(heat, smoothed, ledsPerRing);

  // Render
  for (int b = 0; b < px.getNumBoards(); b++) {
    for (int i = 0; i < ledsPerRing; i++) {
      uint32_t c = rpHeatColor(heat[i]);
      px.setRingLed(b, 0, i, c);
      px.setRingLed(b, 1, i, c);
    }
  }
  px.show();
}

// ============================================================
//  10: COMETS — dual comets with quadratic fade + sparkles
// ============================================================
void fxComets(RobotechPixel& px, unsigned long now,
             uint32_t cometColor, int tailLen,
             uint32_t sparkleColor, int numSparkles, int wait) {
  static int head = 0;
  static unsigned long lastUpdate = 0;

  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  uint8_t cr = (cometColor >> 16) & 0xFF;
  uint8_t cg = (cometColor >> 8)  & 0xFF;
  uint8_t cb =  cometColor        & 0xFF;
  uint16_t ledsPerRing = px.getLedsPerRing();

  px.clear();
  for (int bd = 0; bd < px.getNumBoards(); bd++) {
    for (int j = 0; j < tailLen; j++) {
      uint16_t fade = 255 - (j * 255 / tailLen);
      fade = (fade * fade) >> 8;
      uint8_t fr = (cr * fade) >> 8;
      uint8_t fg = (cg * fade) >> 8;
      uint8_t fb = (cb * fade) >> 8;
      int i1 = (head - j + ledsPerRing) % ledsPerRing;
      int i2 = (head + ledsPerRing / 2 - j + ledsPerRing) % ledsPerRing;
      px.setRingLed(bd, 0, i1, RobotechPixel::color(fr, fg, fb));
      px.setRingLed(bd, 1, i2, RobotechPixel::color(fr, fg, fb));
    }
  }

  // Optional sparkles
  uint8_t sr = (sparkleColor >> 16) & 0xFF;
  uint8_t sg = (sparkleColor >> 8)  & 0xFF;
  uint8_t sb =  sparkleColor        & 0xFF;
  uint16_t total = px.getTotalPixels();
  for (int s = 0; s < numSparkles; s++) {
    uint8_t bright = random(100, 255);
    px.setLed(random(total), RobotechPixel::color(
      (sr * bright) >> 8, (sg * bright) >> 8, (sb * bright) >> 8));
  }

  px.show();
  head = (head + 1) % ledsPerRing;
}
