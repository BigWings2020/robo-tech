#include "RobotechPixel.h"
#include "RobotechPixelHelpers.h"

// ============================================================
//  11: BREATHING — sinusoidal breathing
// ============================================================
void fxBreathing(RobotechPixel& px, unsigned long now,
                 uint8_t r, uint8_t g, uint8_t b, int wait) {
  static uint8_t phase = 0;
  static unsigned long lastUpdate = 0;

  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  uint8_t val = rpTriWave(phase);
  uint16_t total = px.getTotalPixels();
  for (int i = 0; i < total; i++)
    px.setLed(i, RobotechPixel::color((r * val) >> 8, (g * val) >> 8, (b * val) >> 8));
  px.show();
  phase++;
}

// ============================================================
//  12: COLOR WIPE — sequential color fill with rotation
// ============================================================
void fxColorWipe(RobotechPixel& px, unsigned long now,
                 uint32_t color, int wait) {
  static int pos = 0;
  static unsigned long lastUpdate = 0;

  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  for (int b = 0; b < px.getNumBoards(); b++) {
    if (!px.boardHasRings(b)) continue;
    uint8_t lpr = px.getLedsPerRing(b);
    px.setRingLed(b, 0, pos % lpr, color);
    px.setRingLed(b, 1, pos % lpr, color);
  }
  px.show();
  pos++;
  if (pos >= px.getLedsPerRing(0)) {
    pos = 0;
    px.clear();
  }
}

// ============================================================
//  13: WAVE — gamma-corrected wave pattern
// ============================================================
void fxWave(RobotechPixel& px, unsigned long now,
            uint32_t innerColor, uint32_t outerColor,
            int speed, int wait) {
  static uint16_t offset = 0;
  static unsigned long lastUpdate = 0;

  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  uint8_t ir = (innerColor >> 16) & 0xFF;
  uint8_t ig = (innerColor >> 8)  & 0xFF;
  uint8_t ib =  innerColor        & 0xFF;
  uint8_t or_ = (outerColor >> 16) & 0xFF;
  uint8_t og  = (outerColor >> 8)  & 0xFF;
  uint8_t ob  =  outerColor        & 0xFF;

  for (int b = 0; b < px.getNumBoards(); b++) {
    if (!px.boardHasRings(b)) continue;
    uint8_t lpr = px.getLedsPerRing(b);
    for (int i = 0; i < lpr; i++) {
      uint8_t raw = rpTriWave((uint8_t)((i * 256 / lpr) + offset));
      uint8_t val = rpGamma8(raw);
      px.setRingLed(b, 0, i, RobotechPixel::color(
        (ir * val) >> 8, (ig * val) >> 8, (ib * val) >> 8));
      px.setRingLed(b, 1, i, RobotechPixel::color(
        (or_ * val) >> 8, (og * val) >> 8, (ob * val) >> 8));
    }
  }
  px.show();
  offset += speed;
}

// ============================================================
//  14: CYLON — bouncing eye
// ============================================================
void fxCylon(RobotechPixel& px, unsigned long now,
             uint32_t color, int width, int wait) {
  static int pos = 24;
  static int8_t dir = 1;
  static unsigned long lastUpdate = 0;

  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  int half = width / 2;
  uint8_t r  = (color >> 16) & 0xFF;
  uint8_t g  = (color >> 8)  & 0xFF;
  uint8_t bl =  color        & 0xFF;

  px.clear();
  for (int bd = 0; bd < px.getNumBoards(); bd++) {
    if (!px.boardHasRings(bd)) continue;
    uint8_t lpr = px.getLedsPerRing(bd);
    int p = pos % lpr;
    for (int j = -half; j <= half; j++) {
      int idx = p + j;
      if (idx < 0 || idx >= lpr) continue;
      uint8_t fade = 255 - (abs(j) * 255 / (half + 1));
      px.setRingLed(bd, 0, idx, RobotechPixel::color(
        (r * fade) >> 8, (g * fade) >> 8, (bl * fade) >> 8));
      px.setRingLed(bd, 1, idx, RobotechPixel::color(
        (r * fade) >> 8, (g * fade) >> 8, (bl * fade) >> 8));
    }
  }
  px.show();

  // Bounce using board 0's ring size
  uint8_t lpr0 = px.getLedsPerRing(0);
  pos += dir;
  if (pos >= lpr0 - half) dir = -1;
  if (pos <= half) dir = 1;
}

// ============================================================
//  15: METEOR SHOWER — multiple meteors with fade trails
// ============================================================
void fxMeteorShower(RobotechPixel& px, unsigned long now,
                    uint32_t color1, uint32_t color2, uint32_t color3,
                    uint8_t fadeFactor, int wait) {
  static int m1 = 0, m2 = 16, m3 = 32;
  static unsigned long lastUpdate = 0;

  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  uint16_t total = px.getTotalPixels();
  for (int i = 0; i < total; i++) {
    if (random(10) > 3) px.fadePixel(i, fadeFactor);
  }

  for (int b = 0; b < px.getNumBoards(); b++) {
    if (!px.boardHasRings(b)) continue;
    uint8_t lpr = px.getLedsPerRing(b);
    px.setRingLed(b, 0, m1 % lpr, color1);
    px.setRingLed(b, 1, m2 % lpr, color2);
    px.setRingLed(b, 0, m3 % lpr, color3);
  }
  m1 = (m1 + 1);
  m2 = (m2 + 2);
  m3 = (m3 + 1);
  px.show();
}

// ============================================================
//  16: COMET CROSSER — AA comet oscillating inner/outer
// ============================================================
void fxCometCrosser(RobotechPixel& px, unsigned long now,
                    uint32_t cometColor, uint32_t sparkleColor,
                    int numSparkles, int tailLen,
                    float cometSpeed, float blendSpeed, int wait) {
  static float angle = 0;
  static float blend = 0;
  static float blendDir = 0.025f;
  static unsigned long lastUpdate = 0;

  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  // Use caller's blend speed for direction magnitude
  if (blendDir > 0) blendDir = blendSpeed;
  else              blendDir = -blendSpeed;

  blend += blendDir;
  if (blend >= 1.0f || blend <= 0.0f) blendDir = -blendDir;
  blend = constrain(blend, 0.0f, 1.0f);

  uint8_t cr = (cometColor >> 16) & 0xFF;
  uint8_t cg = (cometColor >> 8)  & 0xFF;
  uint8_t cb =  cometColor        & 0xFF;

  px.clear();

  for (int b = 0; b < px.getNumBoards(); b++) {
    if (!px.boardHasRings(b)) continue;
    uint8_t lpr = px.getLedsPerRing(b);
    int hi = ((int)angle) % lpr;
    float frac = angle - (int)angle;
    float iw = 1.0f - blend;
    float ow = blend;

    // Anti-aliased head
    for (int k = 0; k < 2; k++) {
      int pxIdx = (hi + k) % lpr;
      float br = (k == 0) ? (1.0f - frac) : frac;
      px.setRingLed(b, 0, pxIdx, RobotechPixel::color(
        (uint8_t)(cr * br * iw), (uint8_t)(cg * br * iw), (uint8_t)(cb * br * iw)));
      px.setRingLed(b, 1, pxIdx, RobotechPixel::color(
        (uint8_t)(cr * br * ow), (uint8_t)(cg * br * ow), (uint8_t)(cb * br * ow)));
    }
    // Quadratic fading tail
    int tl = min(tailLen, (int)lpr);
    for (int j = 1; j < tl; j++) {
      int tp = (hi - j + lpr) % lpr;
      float fade = 1.0f - (float)j / tl;
      fade *= fade;
      px.setRingLed(b, 0, tp, RobotechPixel::color(
        (uint8_t)(cr * fade * iw), (uint8_t)(cg * fade * iw), (uint8_t)(cb * fade * iw)));
      px.setRingLed(b, 1, tp, RobotechPixel::color(
        (uint8_t)(cr * fade * ow), (uint8_t)(cg * fade * ow), (uint8_t)(cb * fade * ow)));
    }
  }

  // Sparkles
  uint8_t sr = (sparkleColor >> 16) & 0xFF;
  uint8_t sg = (sparkleColor >> 8)  & 0xFF;
  uint8_t sb =  sparkleColor        & 0xFF;
  uint16_t total = px.getTotalPixels();
  for (int s = 0; s < numSparkles; s++) {
    uint8_t t = random(100, 255);
    px.setLed(random(total), RobotechPixel::color(
      (sr * t) >> 8, (sg * t) >> 8, (sb * t) >> 8));
  }

  px.show();
  angle += cometSpeed;
  uint8_t lpr0 = px.getLedsPerRing(0);
  if (angle >= lpr0) angle -= lpr0;
  if (angle < 0) angle += lpr0;
}

// ============================================================
//  17: STROBE — rapid on/off with hue rotation
// ============================================================
void fxStrobe(RobotechPixel& px, unsigned long now,
              uint8_t hueStep, int wait) {
  static bool on = false;
  static uint8_t hue = 0;
  static unsigned long lastUpdate = 0;

  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  uint16_t total = px.getTotalPixels();
  if (on) {
    uint32_t c = rpWheel(hue);
    for (int i = 0; i < total; i++) px.setLed(i, c);
    hue += hueStep;
  } else {
    px.clear();
  }
  px.show();
  on = !on;
}

// ============================================================
//  18: CONFETTI — random sparkles with slow trail decay
// ============================================================
void fxConfetti(RobotechPixel& px, unsigned long now,
                uint8_t fadeFactor, int wait) {
  static unsigned long lastUpdate = 0;

  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  px.fadeAll(fadeFactor);
  px.setLed(random(px.getTotalPixels()), rpWheel(random(256)));
  px.show();
}

// ============================================================
//  19: CLOCK — 12-hour clock with hour/minute/second hands
// ============================================================
void fxClockEffect(RobotechPixel& px, unsigned long now,
                   uint32_t hourColor, uint32_t minuteColor,
                   uint32_t secondColor, int hourWidth, int clockOffset) {
  static unsigned long startMs = 0;
  static unsigned long lastUpdate = 0;

  if (now - lastUpdate < 16) return;  // ~60fps cap
  lastUpdate = now;

  if (startMs == 0) startMs = now;
  unsigned long elapsed = now - startMs;

  int mins = (elapsed / 60000) % 60;
  int hrs  = (elapsed / 3600000) % 12;

  // Smooth second position — clockwise (increasing LED index = CW)
  float secPos = (elapsed % 60000) / 60000.0f * RP_MAX_LEDS_PER_RING + clockOffset;

  uint8_t sr = (secondColor >> 16) & 0xFF;
  uint8_t sg = (secondColor >> 8)  & 0xFF;
  uint8_t sb =  secondColor        & 0xFF;

  int halfHour = hourWidth / 2;

  px.clear();
  for (int b = 0; b < px.getNumBoards(); b++) {
    if (!px.boardHasRings(b)) continue;
    uint8_t lpr = px.getLedsPerRing(b);

    // Scale positions to this board's ring size
    float bSecPos = (elapsed % 60000) / 60000.0f * lpr + clockOffset;
    while (bSecPos >= lpr) bSecPos -= lpr;
    int secInt = (int)bSecPos;
    uint8_t secFrac = (uint8_t)((bSecPos - secInt) * 255);

    int minPos  = (mins * lpr / 60 + clockOffset) % lpr;
    int hourPos = (hrs * lpr / 12 + mins * lpr / 720 + clockOffset) % lpr;

    // Hour hand
    for (int j = -halfHour; j <= halfHour; j++) {
      int idx = (hourPos + j + lpr) % lpr;
      px.setRingLed(b, 0, idx, hourColor);
      px.setRingLed(b, 1, idx, hourColor);
    }
    // Minute hand
    px.setRingLed(b, 0, minPos, minuteColor);
    px.setRingLed(b, 1, minPos, minuteColor);
    // Second hand (anti-aliased)
    int s0 = secInt % lpr;
    int s1 = (secInt + 1) % lpr;
    uint8_t bright0 = 255 - secFrac;
    uint8_t bright1 = secFrac;
    px.setRingLed(b, 0, s0, RobotechPixel::color(
      (sr * bright0) >> 8, (sg * bright0) >> 8, (sb * bright0) >> 8));
    px.setRingLed(b, 1, s0, RobotechPixel::color(
      (sr * bright0) >> 8, (sg * bright0) >> 8, (sb * bright0) >> 8));
    px.setRingLed(b, 0, s1, RobotechPixel::color(
      (sr * bright1) >> 8, (sg * bright1) >> 8, (sb * bright1) >> 8));
    px.setRingLed(b, 1, s1, RobotechPixel::color(
      (sr * bright1) >> 8, (sg * bright1) >> 8, (sb * bright1) >> 8));
  }
  px.show();
}

// ============================================================
//  20: SPARKS — gold/orange/white sparkles
// ============================================================
void fxSparks(RobotechPixel& px, unsigned long now,
               uint8_t fadeFactor, int wait) {
  static unsigned long lastUpdate = 0;

  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  px.fadeAll(fadeFactor);

  const uint32_t palette[] = {
    RobotechPixel::color(255, 180, 0),   // gold
    RobotechPixel::color(255, 100, 0),   // orange
    RobotechPixel::color(255, 255, 200), // warm white
  };

  int count = random(1, 3); // 1 or 2 sparkles per frame
  uint16_t total = px.getTotalPixels();
  for (int i = 0; i < count; i++)
    px.setLed(random(total), palette[random(3)]);

  px.show();
}

// ============================================================
//  21: AUTO CYCLE — cycles through all effects
// ============================================================
void fxAutoCycle(RobotechPixel& px, unsigned long now,
                 unsigned long interval) {
  static int currentEffect = 0;
  static unsigned long lastSwitch = 0;

  if (lastSwitch == 0) lastSwitch = now;
  if (now - lastSwitch >= interval) {
    lastSwitch = now;
    currentEffect = (currentEffect + 1) % 25;
    px.clear();
    px.show();
  }

  switch (currentEffect) {
    // Orbit & Trail
    case 0:  fxPixelWalk(px, now);     break;
    case 1:  fxRadarSweep(px, now);    break;
    case 2:  fxDualSpin(px, now);      break;
    case 3:  fxComets(px, now);        break;
    case 4:  fxCometCrosser(px, now);  break;
    case 5:  fxOrbit(px, now);         break;
    case 6:  fxMeteorShower(px, now);  break;
    // Scan & Chase
    case 7:  fxCylon(px, now);         break;
    case 8:  fxTheaterChase(px, now);  break;
    // Sparkle & Particle
    case 9:  fxSparkle(px, now);       break;
    case 10: fxConfetti(px, now);      break;
    case 11: fxSparks(px, now);        break;
    case 12: fxTwinkle(px, now);       break;
    // Pulse & Wave
    case 13: fxColorCycle(px, now);    break;
    case 14: fxRainbowWheel(px, now);  break;
    case 15: fxWave(px, now);          break;
    case 16: fxRipplePulse(px, now);   break;
    case 17: fxColorWipe(px, now);     break;
    case 18: fxBreathing(px, now);     break;
    case 19: fxHeartbeat(px, now);     break;
    // Simulation & Cartesian
    case 20: fxFireRing(px, now);      break;
    case 21: fxStarfield(px, now);     break;
    // Signal & Flash
    case 22: fxStrobe(px, now);        break;
    case 23: fxPolice(px, now);        break;
    // Dial
    case 24: fxClockEffect(px, now);   break;
  }
}
