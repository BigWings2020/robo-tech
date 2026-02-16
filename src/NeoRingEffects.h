#ifndef NEORING_EFFECTS_H
#define NEORING_EFFECTS_H

#include <Arduino.h>

class RobotechPixel;

// =====================================================
//  AUTO CYCLE (NeoRingEffects_Advanced.cpp)
// =====================================================

void fxAutoCycle    (RobotechPixel& px, unsigned long now,
                     unsigned long interval = 5000);

// =====================================================
//  ORBIT & TRAIL effects (NeoRingEffects_Basic.cpp)
// =====================================================

void fxPixelWalk    (RobotechPixel& px, unsigned long now,
                     uint32_t innerColor = 0x00FF00,
                     uint32_t outerColor = 0x0000FF,
                     int wait = 40);

void fxRadarSweep   (RobotechPixel& px, unsigned long now,
                     uint32_t color = 0x00FF40,
                     int tailLen = 12,
                     int wait = 30);

void fxDualSpin     (RobotechPixel& px, unsigned long now,
                     uint32_t innerColor = 0xFF00C8,
                     uint32_t outerColor = 0x00C8FF,
                     int arcLen = 10,
                     int wait = 40);

void fxComets       (RobotechPixel& px, unsigned long now,
                     uint32_t cometColor = 0x00C8FF,
                     int tailLen = 20,
                     uint32_t sparkleColor = 0x008080,
                     int numSparkles = 0,
                     int wait = 25);

void fxCometCrosser (RobotechPixel& px, unsigned long now,
                     uint32_t cometColor = 0xFF6400,
                     uint32_t sparkleColor = 0x008080,
                     int numSparkles = 1,
                     int tailLen = 18,
                     float cometSpeed = 1.5f,
                     float blendSpeed = 0.025f,
                     int wait = 20);

void fxOrbit        (RobotechPixel& px, unsigned long now,
                     int numDots = 4,
                     int tailLen = 6,
                     int wait = 15);

void fxMeteorShower (RobotechPixel& px, unsigned long now,
                     uint32_t color1 = 0xFFFFFF,
                     uint32_t color2 = 0xC8C8FF,
                     uint32_t color3 = 0xFFC8C8,
                     uint8_t fadeFactor = 180,
                     int wait = 25);

// =====================================================
//  SCAN & CHASE effects
// =====================================================

void fxCylon        (RobotechPixel& px, unsigned long now,
                     uint32_t color = 0xFF0000,
                     int width = 8,
                     int wait = 30);

void fxTheaterChase (RobotechPixel& px, unsigned long now,
                     uint32_t color = 0xFFB400,
                     int wait = 80);

// =====================================================
//  SPARKLE & PARTICLE effects
// =====================================================

void fxSparkle      (RobotechPixel& px, unsigned long now,
                     int numSparks = 8,
                     uint8_t fadeFactor = 200,
                     int wait = 50);

void fxConfetti     (RobotechPixel& px, unsigned long now,
                     uint8_t fadeFactor = 230,
                     int wait = 30);

void fxSparks       (RobotechPixel& px, unsigned long now,
                     uint8_t fadeFactor = 220,
                     int wait = 30);

void fxTwinkle      (RobotechPixel& px, unsigned long now,
                     uint32_t color = 0xCCCCFF,
                     int maxStars = 12,
                     int wait = 20);

// =====================================================
//  PULSE & WAVE effects
// =====================================================

void fxColorCycle   (RobotechPixel& px, unsigned long now,
                     int interval = 1000);

void fxRainbowWheel (RobotechPixel& px, unsigned long now,
                     int wait = 15);

void fxWave         (RobotechPixel& px, unsigned long now,
                     uint32_t innerColor = 0x00FF80,
                     uint32_t outerColor = 0x8000FF,
                     int speed = 3,
                     int wait = 20);

void fxRipplePulse  (RobotechPixel& px, unsigned long now,
                     uint32_t innerColor = 0xFF0055,
                     uint32_t outerColor = 0x0055FF,
                     int wait = 15);

void fxColorWipe    (RobotechPixel& px, unsigned long now,
                     uint32_t color = 0xFF0000,
                     int wait = 30);

void fxBreathing    (RobotechPixel& px, unsigned long now,
                     uint8_t r = 255, uint8_t g = 100, uint8_t b = 20,
                     int wait = 20);

void fxHeartbeat    (RobotechPixel& px, unsigned long now,
                     uint32_t color = 0xFF0000,
                     int wait = 10);

// =====================================================
//  SIMULATION & CARTESIAN effects
// =====================================================

void fxFireRing     (RobotechPixel& px, unsigned long now,
                     int cooling = 10,
                     int sparking = 2,
                     int wait = 30);

void fxStarfield    (RobotechPixel& px, unsigned long now,
                     int maxStars = 10,
                     int wait = 20);

// =====================================================
//  SIGNAL & FLASH effects
// =====================================================

void fxStrobe       (RobotechPixel& px, unsigned long now,
                     uint8_t hueStep = 32,
                     int wait = 60);

void fxPolice       (RobotechPixel& px, unsigned long now,
                     uint32_t color1 = 0xFF0000,
                     uint32_t color2 = 0x0000FF,
                     int wait = 80);

// =====================================================
//  PSYCHEDELIC effects (NeoRingEffects_Extra.cpp)
// =====================================================

void fxHypnosis    (RobotechPixel& px, unsigned long now,
                    int wait = 20);

// =====================================================
//  TABLE GAME effects (NeoRingEffects_Roulette.cpp)
// =====================================================

void fxRoulette     (RobotechPixel& px, unsigned long now,
                     int wait = 20);

void fxWheelOfFortune(RobotechPixel& px, unsigned long now,
                      int numCells = 4,
                      int wait = 20);

// =====================================================
//  DIAL effects
// =====================================================

void fxClockEffect  (RobotechPixel& px, unsigned long now,
                     uint32_t hourColor = 0xFF0000,
                     uint32_t minuteColor = 0x00FF00,
                     uint32_t secondColor = 0x0000FF,
                     int hourWidth = 3,
                     int clockOffset = 12);

#endif // NEORING_EFFECTS_H
