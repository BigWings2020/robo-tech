#include <RobotechPixel.h>

// --- Configuration ---
#define DATA_PIN       7    // change this to match your wiring
#define NUM_BOARDS     3    // number of daisy-chained NeoRings
#define MAX_BRIGHTNESS 150

RobotechPixel px(BOARD_NEORING, NUM_BOARDS, DATA_PIN);
char currentMode = 'a';

// serial line buffer for multi-char commands (e.g. B255)
char serialBuf[8];
uint8_t serialLen = 0;

const __FlashStringHelper* effectName(char mode) {
  switch (mode) {
    case '1': return F("Full White");
    case '2': return F("Roulette");
    case '3': return F("Wheel of Fortune");
    case '4': return F("Hypnosis");
    case 'a': return F("Auto Cycle");
    case 'b': return F("Pixel Walk");
    case 'c': return F("Radar Sweep");
    case 'd': return F("Dual Spin");
    case 'e': return F("Comets");
    case 'f': return F("Comet Crosser");
    case 'g': return F("Orbit");
    case 'h': return F("Meteor Shower");
    case 'i': return F("Cylon");
    case 'j': return F("Theater Chase");
    case 'k': return F("Sparkle");
    case 'l': return F("Confetti");
    case 'm': return F("Sparks");
    case 'n': return F("Twinkle");
    case 'o': return F("Color Cycle");
    case 'p': return F("Rainbow Wheel");
    case 'q': return F("Wave");
    case 'r': return F("Ripple Pulse");
    case 's': return F("Color Wipe");
    case 't': return F("Breathing");
    case 'u': return F("Heartbeat");
    case 'v': return F("Fire Ring");
    case 'w': return F("Starfield");
    case 'x': return F("Strobe");
    case 'y': return F("Police");
    case 'z': return F("Clock");
    case '0': return F("All OFF");
    default:  return F("Unknown");
  }
}

void printMenu() {
  Serial.println(F("=== RobotechPixel Demo ==="));
  Serial.println(F("--- Test ---"));
  Serial.println(F("  1: Full White"));
  Serial.println(F("  2: Roulette"));
  Serial.println(F("  3: Wheel of Fortune"));
  Serial.println(F("  4: Hypnosis"));
  Serial.println(F("--- Auto ---"));
  Serial.println(F("  a: Auto Cycle (default)"));
  Serial.println(F("--- Orbit & Trail ---"));
  Serial.println(F("  b: Pixel Walk     c: Radar Sweep   d: Dual Spin"));
  Serial.println(F("  e: Comets         f: Comet Crosser  g: Orbit"));
  Serial.println(F("  h: Meteor Shower"));
  Serial.println(F("--- Scan & Chase ---"));
  Serial.println(F("  i: Cylon          j: Theater Chase"));
  Serial.println(F("--- Sparkle & Particle ---"));
  Serial.println(F("  k: Sparkle        l: Confetti       m: Sparks"));
  Serial.println(F("  n: Twinkle"));
  Serial.println(F("--- Pulse & Wave ---"));
  Serial.println(F("  o: Color Cycle    p: Rainbow Wheel  q: Wave"));
  Serial.println(F("  r: Ripple Pulse   s: Color Wipe     t: Breathing"));
  Serial.println(F("  u: Heartbeat"));
  Serial.println(F("--- Simulation ---"));
  Serial.println(F("  v: Fire Ring      w: Starfield"));
  Serial.println(F("--- Signal & Flash ---"));
  Serial.println(F("  x: Strobe         y: Police"));
  Serial.println(F("--- Dial ---"));
  Serial.println(F("  z: Clock"));
  Serial.println(F("  0: All OFF"));
  Serial.println(F("--- Brightness ---"));
  Serial.println(F("  B<0-255>  e.g. B255"));
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  randomSeed(analogRead(A0));
  px.begin();
  px.setBrightness(MAX_BRIGHTNESS);
  printMenu();
}

void loop() {
  unsigned long now = millis();

  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (serialLen == 0) continue;
      serialBuf[serialLen] = '\0';

      if ((serialBuf[0] == 'B' || serialBuf[0] == 'b') && serialLen > 1) {
        int val = atoi(serialBuf + 1);
        val = constrain(val, 0, 255);
        px.setBrightness((uint8_t)val);
        Serial.print(F("\n>>> Brightness: "));
        Serial.println(val);
      } else {
        currentMode = serialBuf[0];
        Serial.print(F("\n>>> "));
        Serial.println(effectName(currentMode));
      }
      serialLen = 0;
    } else if (serialLen < sizeof(serialBuf) - 1) {
      serialBuf[serialLen++] = c;
    }
  }

  // ================================================================
  //  EFFECT CALLS — all parameters are shown explicitly.
  //  Tweak any value below, recompile, and upload to experiment.
  //  Colors are 0xRRGGBB hex. "wait" is frame delay in ms (lower = faster).
  // ================================================================
  switch (currentMode) {

    // --- Test ---
    case '1':
      for (int i = 0; i < px.getTotalPixels(); i++)
        px.setLed(i, RobotechPixel::color(255, 255, 255));
      px.show();
      break;

    case '2':
      fxRoulette(px, now,
        /*wait*/ 20);
      break;

    case '3':
      fxWheelOfFortune(px, now,
        /*numCells*/ 4,
        /*wait*/     20);
      break;

    case '4':
      fxHypnosis(px, now,
        /*wait*/ 20);
      break;

    // --- Auto ---
    case 'a':
      fxAutoCycle(px, now,
        /*interval*/ 5000);
      break;

    // --- Orbit & Trail ---
    case 'b':
      fxPixelWalk(px, now,
        /*innerColor*/ 0x00FF00,
        /*outerColor*/ 0x0000FF,
        /*wait*/       40);
      break;

    case 'c':
      fxRadarSweep(px, now,
        /*color*/   0x00FF40,
        /*tailLen*/ 12,
        /*wait*/    30);
      break;

    case 'd':
      fxDualSpin(px, now,
        /*innerColor*/ 0xFF00C8,
        /*outerColor*/ 0x00C8FF,
        /*arcLen*/     10,
        /*wait*/       40);
      break;

    case 'e':
      fxComets(px, now,
        /*cometColor*/   0x00C8FF,
        /*tailLen*/      20,
        /*sparkleColor*/ 0x008080,
        /*numSparkles*/  0,
        /*wait*/         25);
      break;

    case 'f':
      fxCometCrosser(px, now,
        /*cometColor*/   0xFF6400,
        /*sparkleColor*/ 0x008080,
        /*numSparkles*/  1,
        /*tailLen*/      18,
        /*cometSpeed*/   1.5f,
        /*blendSpeed*/   0.025f,
        /*wait*/         20);
      break;

    case 'g':
      fxOrbit(px, now,
        /*numDots*/ 4,
        /*tailLen*/ 6,
        /*wait*/    15);
      break;

    case 'h':
      fxMeteorShower(px, now,
        /*color1*/     0xFFFFFF,
        /*color2*/     0xC8C8FF,
        /*color3*/     0xFFC8C8,
        /*fadeFactor*/ 180,
        /*wait*/       25);
      break;

    // --- Scan & Chase ---
    case 'i':
      fxCylon(px, now,
        /*color*/ 0xFF0000,
        /*width*/ 8,
        /*wait*/  30);
      break;

    case 'j':
      fxTheaterChase(px, now,
        /*color*/ 0xFFB400,
        /*wait*/  80);
      break;

    // --- Sparkle & Particle ---
    case 'k':
      fxSparkle(px, now,
        /*numSparks*/  8,
        /*fadeFactor*/ 200,
        /*wait*/       50);
      break;

    case 'l':
      fxConfetti(px, now,
        /*fadeFactor*/ 230,
        /*wait*/       30);
      break;

    case 'm':
      fxSparks(px, now,
        /*fadeFactor*/ 220,
        /*wait*/       30);
      break;

    case 'n':
      fxTwinkle(px, now,
        /*color*/    0xCCCCFF,
        /*maxStars*/ 12,
        /*wait*/     20);
      break;

    // --- Pulse & Wave ---
    case 'o':
      fxColorCycle(px, now,
        /*interval*/ 1000);
      break;

    case 'p':
      fxRainbowWheel(px, now,
        /*wait*/ 15);
      break;

    case 'q':
      fxWave(px, now,
        /*innerColor*/ 0x00FF80,
        /*outerColor*/ 0x8000FF,
        /*speed*/      3,         // negative reverses direction
        /*wait*/       20);
      break;

    case 'r':
      fxRipplePulse(px, now,
        /*innerColor*/ 0xFF0055,
        /*outerColor*/ 0x0055FF,
        /*wait*/       15);
      break;

    case 's':
      fxColorWipe(px, now,
        /*color*/ 0xFF0000,
        /*wait*/  30);
      break;

    case 't':
      fxBreathing(px, now,
        /*r*/    255,
        /*g*/    100,
        /*b*/    20,
        /*wait*/ 20);
      break;

    case 'u':
      fxHeartbeat(px, now,
        /*color*/ 0xFF0000,
        /*wait*/  10);
      break;

    // --- Simulation ---
    case 'v':
      fxFireRing(px, now,
        /*cooling*/  10,
        /*sparking*/ 2,
        /*wait*/     30);
      break;

    case 'w':
      fxStarfield(px, now,
        /*maxStars*/ 10,
        /*wait*/     20);
      break;

    // --- Signal & Flash ---
    case 'x':
      fxStrobe(px, now,
        /*hueStep*/ 32,
        /*wait*/    60);
      break;

    case 'y':
      fxPolice(px, now,
        /*color1*/ 0xFF0000,
        /*color2*/ 0x0000FF,
        /*wait*/   80);
      break;

    // --- Dial ---
    case 'z':
      fxClockEffect(px, now,
        /*hourColor*/   0xFF0000,
        /*minuteColor*/ 0x00FF00,
        /*secondColor*/ 0x0000FF,
        /*hourWidth*/   3,
        /*clockOffset*/ 12);
      break;

    case '0':
    default:
      px.clear();
      px.show();
      break;
  }
}
