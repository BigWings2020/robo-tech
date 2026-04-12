# RobotechPixel

Arduino library for **Robotech** LED boards — NeoRing, NeoRing XL, and QuadPixel.

Wraps [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel) with board-aware addressing, 26 non-blocking effects, and support for mixed board types on a single daisy chain.

## Supported Hardware

| Board | LEDs per board | Description |
|-------|---------------|-------------|
| **NeoRing** | 96 (2 x 48-LED rings) | Dual concentric ring PCB |
| **NeoRing XL** | 180 (3 x 60-LED rings) | Triple concentric ring PCB |
| **QuadPixel** | 4 | Compact 4-LED PCB |

Boards can be daisy-chained — including mixed types on the same data pin.

## Installation

### Direct Download

1. Download **[RobotechPixel-1.0.1.zip](https://robo-tech.ca/downloads/RobotechPixel-1.0.1.zip)**
2. In Arduino IDE: **Sketch → Include Library → Add .ZIP Library...**
3. Install the **Adafruit NeoPixel** dependency via Library Manager

### Arduino Library Manager

Search for **RobotechPixel** in **Sketch → Include Library → Manage Libraries**.

## Quick Start

```cpp
#include <RobotechPixel.h>

// Board type, number of boards, data pin
RobotechPixel px(BOARD_NEORING, 1, 7);

void setup() {
  Serial.begin(115200);
  px.begin();
  px.setBrightness(150);
}

void loop() {
  fxAutoCycle(px, millis());
}
```

Open the Serial Monitor (115200 baud) and type a letter to switch effects.

## Mixed Chain

```cpp
const BoardType chain[] = { BOARD_NEORING, BOARD_NEORING_XL, BOARD_NEORING };
RobotechPixel px(chain, 3, 7);
```

## Effects (26 built-in)

| Category | Effects |
|----------|---------|
| **Orbit & Trail** | `fxPixelWalk`, `fxRadarSweep`, `fxDualSpin`, `fxComets`, `fxCometCrosser`, `fxOrbit`, `fxMeteorShower` |
| **Scan & Chase** | `fxCylon`, `fxTheaterChase` |
| **Sparkle & Particle** | `fxSparkle`, `fxConfetti`, `fxSparks`, `fxTwinkle` |
| **Pulse & Wave** | `fxColorCycle`, `fxRainbowWheel`, `fxWave`, `fxRipplePulse`, `fxColorWipe`, `fxBreathing`, `fxHeartbeat` |
| **Simulation** | `fxFireRing`, `fxStarfield` |
| **Signal & Flash** | `fxStrobe`, `fxPolice` |
| **Psychedelic** | `fxHypnosis` |
| **Table Games** | `fxRoulette`, `fxWheelOfFortune` |
| **Dial** | `fxClockEffect` |
| **Auto** | `fxAutoCycle` |

Try them live at [robo-tech.ca/docs/neoring/effects](https://robo-tech.ca/docs/neoring/effects)

## Documentation

Full docs at **[robo-tech.ca/docs](https://robo-tech.ca/docs)**

## Dependencies

- [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel)

## License

MIT — see [LICENSE](LICENSE)
