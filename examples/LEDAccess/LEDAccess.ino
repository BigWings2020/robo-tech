// ============================================================
//  LEDAccess — Low-level LED control for NeoRing
//
//  Shows how to address individual LEDs by:
//    1. Global index  (setLed)
//    2. Ring position  (setRingLed)
//    3. Ring helpers   (innerPixel / outerPixel)
//
//  Open Serial Monitor at 115200 baud and send a number
//  to switch between demos.
// ============================================================

#include <RobotechPixel.h>

// --- Configuration ---
#define DATA_PIN   7
#define NUM_BOARDS 1

RobotechPixel px(BOARD_NEORING, NUM_BOARDS, DATA_PIN);
char mode = '1';

void setup() {
  Serial.begin(115200);
  while (!Serial);
  px.begin();
  px.setBrightness(150);

  Serial.println(F("=== LED Access Examples ==="));
  Serial.println(F("  1: Color gradients (inner=red, outer=blue)"));
  Serial.println(F("  2: Single LED walk (global index)"));
  Serial.println(F("  3: Checkerboard pattern"));
  Serial.println(F("  4: Quadrant colors"));
  Serial.println(F("  5: Alternating rings"));
  Serial.println(F("  6: Brightness ramp"));
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c >= '1' && c <= '6') {
      mode = c;
      Serial.print(F(">>> Mode ")); Serial.println(mode);
    }
  }

  unsigned long now = millis();
  uint8_t lpr = px.getLedsPerRing(0);

  switch (mode) {

    // -------------------------------------------------------
    //  1: Color gradients — setRingLed(board, ring, led, color)
    // -------------------------------------------------------
    case '1': {
      px.clear();
      for (int i = 0; i < lpr; i++) {
        uint8_t v = (i * 255) / lpr;
        // Inner ring: red gradient
        px.setRingLed(0, 0, i, RobotechPixel::color(v, 0, 0));
        // Outer ring: blue gradient
        px.setRingLed(0, 1, i, RobotechPixel::color(0, 0, v));
      }
      px.show();
      break;
    }

    // -------------------------------------------------------
    //  2: Single LED walk — setLed(globalIndex, color)
    //     Global index 0..95: 0-47 = inner ring, 48-95 = outer
    // -------------------------------------------------------
    case '2': {
      static int pos = 0;
      static unsigned long lastUpdate = 0;
      if (now - lastUpdate < 40) break;
      lastUpdate = now;

      uint16_t total = px.getTotalPixels();
      px.clear();
      px.setLed(pos, RobotechPixel::color(255, 255, 255));
      px.show();
      pos = (pos + 1) % total;
      break;
    }

    // -------------------------------------------------------
    //  3: Checkerboard — even LEDs on one ring, odd on the other
    // -------------------------------------------------------
    case '3': {
      px.clear();
      for (int i = 0; i < lpr; i++) {
        if (i % 2 == 0) {
          px.setRingLed(0, 0, i, RobotechPixel::color(255, 100, 0));
        } else {
          px.setRingLed(0, 1, i, RobotechPixel::color(0, 100, 255));
        }
      }
      px.show();
      break;
    }

    // -------------------------------------------------------
    //  4: Quadrant colors — divide ring into 4 colored sections
    // -------------------------------------------------------
    case '4': {
      const uint32_t quadColors[] = {
        RobotechPixel::color(255, 0, 0),     // top: red
        RobotechPixel::color(0, 255, 0),     // right: green
        RobotechPixel::color(0, 0, 255),     // bottom: blue
        RobotechPixel::color(255, 255, 0),   // left: yellow
      };

      px.clear();
      for (int i = 0; i < lpr; i++) {
        int quadrant = (i * 4) / lpr;
        px.setRingLed(0, 0, i, quadColors[quadrant]);
        px.setRingLed(0, 1, i, quadColors[quadrant]);
      }
      px.show();
      break;
    }

    // -------------------------------------------------------
    //  5: Alternating rings — innerPixel / outerPixel helpers
    //     These return the global pixel index for a given ring LED
    // -------------------------------------------------------
    case '5': {
      static uint8_t hue = 0;
      static unsigned long lastUpdate2 = 0;
      if (now - lastUpdate2 < 30) break;
      lastUpdate2 = now;

      px.clear();
      for (int i = 0; i < lpr; i++) {
        // Use innerPixel/outerPixel to get global index, then setLed
        int inner = px.innerPixel(0, i);
        int outer = px.outerPixel(0, i);

        // Simple HSV-like hue rotation using sin approximation
        uint8_t phase = (i * 255 / lpr + hue);
        uint32_t c1 = RobotechPixel::color(phase, 255 - phase, 128);
        uint32_t c2 = RobotechPixel::color(255 - phase, 128, phase);

        px.setLed(inner, c1);
        px.setLed(outer, c2);
      }
      px.show();
      hue++;
      break;
    }

    // -------------------------------------------------------
    //  6: Brightness ramp — same color, increasing brightness
    // -------------------------------------------------------
    case '6': {
      px.clear();
      for (int i = 0; i < lpr; i++) {
        uint8_t b = (i * 255) / lpr;
        uint32_t c = RobotechPixel::color(b, b, b);
        px.setRingLed(0, 0, i, c);
        px.setRingLed(0, 1, i, c);
      }
      px.show();
      break;
    }
  }
}
