#include "RobotechPixelHelpers.h"

uint32_t rpWheel(byte pos) {
  pos = 255 - pos;
  if (pos < 85)
    return ((uint32_t)(255 - pos * 3) << 16) | ((uint32_t)(pos * 3));
  if (pos < 170) {
    pos -= 85;
    return ((uint32_t)(pos * 3) << 8) | (uint32_t)(255 - pos * 3);
  }
  pos -= 170;
  return ((uint32_t)(pos * 3) << 16) | ((uint32_t)(255 - pos * 3) << 8);
}

uint32_t rpHeatColor(byte temperature) {
  byte t192 = (temperature * 191UL) >> 8;
  byte ramp = (t192 & 0x3F) << 2;
  if (t192 > 128) return ((uint32_t)255 << 16) | ((uint32_t)255 << 8) | ramp;
  if (t192 > 64)  return ((uint32_t)255 << 16) | ((uint32_t)ramp << 8);
  return ((uint32_t)ramp << 16);
}

uint8_t rpTriWave(uint8_t x) {
  return (x < 128) ? x * 2 : (255 - x) * 2;
}

uint8_t rpGamma8(uint8_t val) {
  return ((uint16_t)val * val + 128) >> 8;
}
