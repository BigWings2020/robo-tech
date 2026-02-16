#ifndef ROBOTECH_PIXEL_HELPERS_H
#define ROBOTECH_PIXEL_HELPERS_H

#include <Arduino.h>

// HSV color wheel (0-255 position -> RGB packed uint32_t)
uint32_t rpWheel(byte pos);

// Fire/heat color mapping (temperature 0-255 -> RGB packed uint32_t)
uint32_t rpHeatColor(byte temperature);

// Triangle wave: 0->0, 64->128, 128->255, 192->128, 255->0
uint8_t rpTriWave(uint8_t x);

// Approximate gamma correction (quadratic)
uint8_t rpGamma8(uint8_t val);

#endif // ROBOTECH_PIXEL_HELPERS_H
