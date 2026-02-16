#include "RobotechPixel.h"
#include "RobotechPixelHelpers.h"

// ============================================================
//  HEARTBEAT — double-pulse heartbeat pattern
// ============================================================
void fxHeartbeat(RobotechPixel& px, unsigned long now,
                 uint32_t color, int wait) {
  static uint16_t phase = 0;
  static unsigned long lastUpdate = 0;

  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  // Heartbeat curve over 256 steps:
  //   0-30   first beat ramp up
  //   30-60  first beat ramp down
  //   60-80  second beat ramp up (smaller)
  //   80-100 second beat ramp down
  //   100-255 rest
  uint8_t val = 0;
  uint16_t p = phase % 256;
  if      (p < 30)  val = (p * 255) / 30;
  else if (p < 60)  val = ((60 - p) * 255) / 30;
  else if (p < 80)  val = ((p - 60) * 180) / 20;
  else if (p < 100) val = ((100 - p) * 180) / 20;

  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8)  & 0xFF;
  uint8_t b =  color        & 0xFF;
  uint16_t total = px.getTotalPixels();
  for (int i = 0; i < total; i++)
    px.setLed(i, RobotechPixel::color((r * val) >> 8, (g * val) >> 8, (b * val) >> 8));
  px.show();
  phase++;
}

// ============================================================
//  STARFIELD — stars radiating outward (polar ← cartesian)
// ============================================================
void fxStarfield(RobotechPixel& px, unsigned long now,
                 int maxStars, int wait) {
  struct Star {
    float x, y, vx, vy;
    bool active;
  };
  static Star stars[12];
  static bool initialized = false;
  static unsigned long lastUpdate = 0;

  if (!initialized) {
    memset(stars, 0, sizeof(stars));
    initialized = true;
  }

  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  uint16_t ledsPerRing = px.getLedsPerRing();
  const float maxRadius = 1.5f;
  const float innerThreshold = 0.75f;

  // Count active and spawn new stars
  int activeCount = 0;
  for (int i = 0; i < 12; i++)
    if (stars[i].active) activeCount++;

  int count = min(maxStars, 12);
  if (activeCount < count && random(10) < 4) {
    for (int i = 0; i < 12; i++) {
      if (!stars[i].active) {
        float angle = random(0, 628) / 100.0f;  // 0 to ~2π
        float speed = random(2, 6) / 100.0f;
        stars[i].x  = cos(angle) * 0.05f;
        stars[i].y  = sin(angle) * 0.05f;
        stars[i].vx = cos(angle) * speed;
        stars[i].vy = sin(angle) * speed;
        stars[i].active = true;
        break;
      }
    }
  }

  px.clear();
  for (int i = 0; i < 12; i++) {
    if (!stars[i].active) continue;

    stars[i].x += stars[i].vx;
    stars[i].y += stars[i].vy;

    float radius = sqrt(stars[i].x * stars[i].x + stars[i].y * stars[i].y);
    if (radius > maxRadius) { stars[i].active = false; continue; }

    // Polar conversion: cartesian (x,y) → angle → LED index
    float angle = atan2(stars[i].y, stars[i].x);
    if (angle < 0) angle += 2.0f * PI;
    int ledIdx = (int)(angle / (2.0f * PI) * ledsPerRing) % ledsPerRing;

    // Brightness increases with radius (approaching = brighter)
    uint8_t bright = min(255, (int)(radius * 255.0f / maxRadius));
    // Inner ring when close to center, outer when further out
    int ring = (radius < innerThreshold) ? 0 : 1;

    for (int bd = 0; bd < px.getNumBoards(); bd++)
      px.setRingLed(bd, ring, ledIdx, RobotechPixel::color(
        bright, bright, (bright * 200) >> 8));
  }
  px.show();
}

// ============================================================
//  POLICE — alternating red/blue halves
// ============================================================
void fxPolice(RobotechPixel& px, unsigned long now,
              uint32_t color1, uint32_t color2, int wait) {
  static uint8_t step = 0;
  static unsigned long lastUpdate = 0;

  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  uint16_t ledsPerRing = px.getLedsPerRing();
  int half = ledsPerRing / 2;

  // 4-step pattern: left-ON, all-OFF, right-ON, all-OFF
  px.clear();
  if (step % 2 == 0) {
    bool leftSide = (step % 4) == 0;
    uint32_t c = leftSide ? color1 : color2;
    int start = leftSide ? 0 : half;
    for (int bd = 0; bd < px.getNumBoards(); bd++) {
      for (int i = start; i < start + half; i++) {
        px.setRingLed(bd, 0, i % ledsPerRing, c);
        px.setRingLed(bd, 1, i % ledsPerRing, c);
      }
    }
  }
  px.show();
  step = (step + 1) % 4;
}

// ============================================================
//  TWINKLE — gentle individually-fading stars
// ============================================================
void fxTwinkle(RobotechPixel& px, unsigned long now,
               uint32_t color, int maxStars, int wait) {
  static uint8_t brightness[RP_NEORING_LEDS_PER_RING];
  static bool growing[RP_NEORING_LEDS_PER_RING];
  static bool active[RP_NEORING_LEDS_PER_RING];
  static bool initialized = false;
  static unsigned long lastUpdate = 0;

  if (!initialized) {
    memset(brightness, 0, sizeof(brightness));
    memset(growing, 0, sizeof(growing));
    memset(active, 0, sizeof(active));
    initialized = true;
  }

  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  uint16_t ledsPerRing = px.getLedsPerRing();
  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8)  & 0xFF;
  uint8_t b =  color        & 0xFF;

  // Count active stars
  int activeCount = 0;
  for (int i = 0; i < ledsPerRing; i++)
    if (active[i]) activeCount++;

  // Randomly spawn new stars
  if (activeCount < maxStars && random(10) < 3) {
    int idx = random(ledsPerRing);
    if (!active[idx]) {
      active[idx] = true;
      growing[idx] = true;
      brightness[idx] = 0;
    }
  }

  // Update and render
  px.clear();
  for (int i = 0; i < ledsPerRing; i++) {
    if (!active[i]) continue;
    if (growing[i]) {
      brightness[i] = min(255, brightness[i] + random(5, 20));
      if (brightness[i] >= 240) { brightness[i] = 255; growing[i] = false; }
    } else {
      int dec = random(2, 8);
      if (brightness[i] <= dec) { brightness[i] = 0; active[i] = false; continue; }
      brightness[i] -= dec;
    }
    uint8_t v = brightness[i];
    uint32_t c = RobotechPixel::color((r * v) >> 8, (g * v) >> 8, (b * v) >> 8);
    for (int bd = 0; bd < px.getNumBoards(); bd++) {
      px.setRingLed(bd, 0, i, c);
      px.setRingLed(bd, 1, i, c);
    }
  }
  px.show();
}

// ============================================================
//  ORBIT — multiple colored dots at different speeds with trails
// ============================================================
void fxOrbit(RobotechPixel& px, unsigned long now,
             int numDots, int tailLen, int wait) {
  static float pos[4] = {0, 12, 24, 36};
  static unsigned long lastUpdate = 0;

  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  const float speeds[] = {1.0f, 1.6f, 0.7f, 2.1f};
  const uint32_t colors[] = {
    RobotechPixel::color(255, 0, 0),
    RobotechPixel::color(0, 255, 0),
    RobotechPixel::color(0, 100, 255),
    RobotechPixel::color(255, 200, 0),
  };

  uint16_t ledsPerRing = px.getLedsPerRing();
  int count = min(numDots, 4);

  px.clear();
  for (int d = 0; d < count; d++) {
    uint8_t r = (colors[d] >> 16) & 0xFF;
    uint8_t g = (colors[d] >> 8)  & 0xFF;
    uint8_t b =  colors[d]        & 0xFF;

    int hi = (int)pos[d];
    float frac = pos[d] - hi;

    for (int bd = 0; bd < px.getNumBoards(); bd++) {
      // Anti-aliased head
      int p0 = hi % ledsPerRing;
      int p1 = (hi + 1) % ledsPerRing;
      px.setRingLed(bd, d % 2, p0, RobotechPixel::color(
        (uint8_t)(r * (1.0f - frac)), (uint8_t)(g * (1.0f - frac)), (uint8_t)(b * (1.0f - frac))));
      px.setRingLed(bd, d % 2, p1, RobotechPixel::color(
        (uint8_t)(r * frac), (uint8_t)(g * frac), (uint8_t)(b * frac)));

      // Fading tail
      for (int j = 1; j < tailLen; j++) {
        int tp = (hi - j + ledsPerRing) % ledsPerRing;
        float fade = 1.0f - (float)j / tailLen;
        fade *= fade;
        px.setRingLed(bd, d % 2, tp, RobotechPixel::color(
          (uint8_t)(r * fade), (uint8_t)(g * fade), (uint8_t)(b * fade)));
      }
    }

    pos[d] += speeds[d];
    if (pos[d] >= ledsPerRing) pos[d] -= ledsPerRing;
  }
  px.show();
}

// ============================================================
//  HYPNOSIS — high-contrast plasma with glitter
//
//  Slow plasma waves (14–54 s cycles) pick colors from a
//  curated neon palette, then a cubic brightness curve crushes
//  most LEDs to black while peaks burn vivid.  White sparkles
//  flash on top.  Pattern repeat period ≈ 404 days.
//  Pure integer math — no floats, no trig.
// ============================================================
void fxHypnosis(RobotechPixel& px, unsigned long now, int wait) {
  static unsigned long lastUpdate = 0;
  static uint8_t glitter[RP_NEORING_LEDS_PER_RING * 2];  // 96 bytes
  static bool glitterInit = false;

  if (!glitterInit) { memset(glitter, 0, sizeof(glitter)); glitterInit = true; }
  if (now - lastUpdate < (unsigned long)wait) return;
  lastUpdate = now;

  const uint16_t N = px.getLedsPerRing();

  // slow time bases — prime divisors, cycles: ~14s / 23s / 35s / 54s
  uint8_t t1 = now / 53;
  uint8_t t2 = now / 89;
  uint8_t t3 = now / 137;
  uint8_t t4 = now / 211;

  // curated palette — ordered so every adjacent pair blends cleanly
  static const uint32_t pal[] = {
    0xFF0060,   // hot pink
    0x8000FF,   // violet
    0x0050FF,   // royal blue
    0x00FFAA,   // mint
    0xBBFF00,   // lime
    0xFF6000    // amber
  };
  static const uint8_t palSz = 6;

  // ---- glitter: decay then spawn ----
  for (uint8_t i = 0; i < N * 2; i++)
    glitter[i] = (glitter[i] > 35) ? glitter[i] - 35 : 0;
  for (uint8_t s = 0; s < 2 + (uint8_t)random(3); s++)
    glitter[random(N * 2)] = 255;

  // ---- render ----
  px.clear();

  for (int bd = 0; bd < px.getNumBoards(); bd++) {
    for (int ring = 0; ring < 2; ring++) {
      uint8_t rOfs = ring * 85;

      for (int i = 0; i < N; i++) {
        uint8_t a = (uint8_t)((uint16_t)i * 256 / N);

        // three slow plasma waves
        uint8_t w1 = rpTriWave((uint8_t)(a * 2 + t1 + rOfs));
        uint8_t w2 = rpTriWave((uint8_t)(a * 3 - t2 + rOfs));
        uint8_t w3 = rpTriWave((uint8_t)(a     + t3
                               + rpTriWave(t2 + rOfs)));

        // cubic brightness envelope — deep blacks, vivid peaks
        uint8_t env = (uint8_t)(((uint16_t)w1 + w2) >> 1);
        uint8_t sharp = (uint8_t)((uint16_t)env * env >> 8);
        sharp = (uint8_t)((uint16_t)sharp * env >> 8);

        // palette blend via w3 + ultra-slow drift
        uint8_t cIdx  = w3 + t4;
        uint16_t scl  = (uint16_t)cIdx * palSz;
        uint8_t  pA   = (uint8_t)(scl >> 8);
        uint8_t  frac = (uint8_t)(scl & 0xFF);
        uint8_t  pB   = (pA + 1 < palSz) ? pA + 1 : 0;

        uint8_t rA = (pal[pA] >> 16) & 0xFF, rB = (pal[pB] >> 16) & 0xFF;
        uint8_t gA = (pal[pA] >>  8) & 0xFF, gB = (pal[pB] >>  8) & 0xFF;
        uint8_t bA =  pal[pA]        & 0xFF, bB =  pal[pB]        & 0xFF;

        uint8_t cr = rA + ((int16_t)(rB - rA) * frac >> 8);
        uint8_t cg = gA + ((int16_t)(gB - gA) * frac >> 8);
        uint8_t cb = bA + ((int16_t)(bB - bA) * frac >> 8);

        // apply cubic brightness
        uint8_t r = (uint8_t)((uint16_t)cr * sharp >> 8);
        uint8_t g = (uint8_t)((uint16_t)cg * sharp >> 8);
        uint8_t b = (uint8_t)((uint16_t)cb * sharp >> 8);

        // glitter: white sparkles drawn on top
        uint8_t gv = glitter[ring * N + i];
        if (gv > r) r = gv;
        if (gv > g) g = gv;
        if (gv > b) b = gv;

        px.setRingLed(bd, ring, i, RobotechPixel::color(r, g, b));
      }
    }
  }

  px.show();
}
