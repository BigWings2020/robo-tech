# RobotechPixel

NeoPixel LED control library for **Robotech NeoRing** and **QuadPixel** boards.

Wraps [FastLED](https://github.com/FastLED/FastLED) with board-aware addressing, 30+ non-blocking effects, and a clean API designed for daisy-chained PCBs.

## Supported Hardware

| Board | LEDs per board | Description |
|-------|---------------|-------------|
| **NeoRing** | 96 (2 x 48-LED rings) | Dual concentric ring PCB |
| **QuadPixel** | 4 | Compact 4-LED PCB |

Boards can be daisy-chained — the library handles all addressing automatically.

## Installation

### Arduino Library Manager (recommended)

1. Open the Arduino IDE
2. Go to **Sketch > Include Library > Manage Libraries...**
3. Search for **RobotechPixel**
4. Click **Install**

### Manual Install

1. Download the latest release ZIP from [GitHub Releases](https://github.com/BigWings2020/robo-tech/releases)
2. In the Arduino IDE, go to **Sketch > Include Library > Add .ZIP Library...**
3. Select the downloaded ZIP file

## Dependencies

- [FastLED](https://github.com/FastLED/FastLED) (installed automatically via Library Manager)

## Quick Start

```cpp
#include <RobotechPixel.h>

#define DATA_PIN   7
#define NUM_BOARDS 1

RobotechPixel px(BOARD_NEORING, NUM_BOARDS, DATA_PIN);

void setup() {
  px.begin();
  px.setBrightness(150);
}

void loop() {
  unsigned long now = millis();
  fxRainbowWheel(px, now);
}
```

## API Reference

### Constructor

```cpp
RobotechPixel(BoardType boardType, uint8_t numBoards, uint8_t pin);
```

- `boardType` — `BOARD_NEORING` or `BOARD_QUADPIXEL`
- `numBoards` — number of daisy-chained PCBs
- `pin` — data pin connected to the LED chain

### Core Methods

| Method | Description |
|--------|-------------|
| `begin()` | Initialize LEDs |
| `show()` | Push buffer to LEDs |
| `clear()` | Turn all LEDs off |
| `setBrightness(b)` | Set global brightness (0-255) |
| `getBrightness()` | Get current brightness |

### LED Access

| Method | Description |
|--------|-------------|
| `setLed(index, color)` | Set LED by global index |
| `setRingLed(pcb, ring, led, color)` | Set LED by board, ring (0=inner, 1=outer), position |
| `color(r, g, b)` | Create a 32-bit color value |

### Geometry Queries

| Method | Description |
|--------|-------------|
| `getBoardType()` | Returns `BOARD_NEORING` or `BOARD_QUADPIXEL` |
| `getNumBoards()` | Number of daisy-chained boards |
| `getTotalPixels()` | Total LEDs across all boards |
| `getLedsPerRing()` | LEDs per ring (48 for NeoRing) |
| `getLedsPerBoard()` | LEDs per board (96 for NeoRing, 4 for QuadPixel) |

## Effects

All effects are non-blocking and take the same pattern: `fxName(px, millis(), ...)`.

| Category | Effects |
|----------|---------|
| **Auto** | `fxAutoCycle` |
| **Orbit & Trail** | `fxPixelWalk`, `fxRadarSweep`, `fxDualSpin`, `fxComets`, `fxCometCrosser`, `fxOrbit`, `fxMeteorShower` |
| **Scan & Chase** | `fxCylon`, `fxTheaterChase` |
| **Sparkle & Particle** | `fxSparkle`, `fxConfetti`, `fxSparks`, `fxTwinkle` |
| **Pulse & Wave** | `fxColorCycle`, `fxRainbowWheel`, `fxWave`, `fxRipplePulse`, `fxColorWipe`, `fxBreathing`, `fxHeartbeat` |
| **Simulation** | `fxFireRing`, `fxStarfield` |
| **Signal & Flash** | `fxStrobe`, `fxPolice` |
| **Psychedelic** | `fxHypnosis` |
| **Table Games** | `fxRoulette`, `fxWheelOfFortune` |
| **Dial** | `fxClockEffect` |

See the [Demo example](examples/Demo/Demo.ino) for a complete showcase of all effects with serial control.

## License

MIT License — see [LICENSE](LICENSE) for details.
