#pragma once
#define FASTLED_INTERNAL
#include <FastLED.h>

#define BRIGHTNESS 255
#define FRAMES_PER_SECOND 120

#define BUTTON_LEDS 8
#define BUTTON_LEDS_PIN 25

#define EFFECT_LEDS 44
#define EFFECT_LEDS_PIN 33

CRGB button_leds[BUTTON_LEDS];
CRGB effect_leds[EFFECT_LEDS];
uint8_t translate[EFFECT_LEDS] = {
    43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, // bottom
    32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, // right
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,           // top
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21  // left

};

CRGBPalette16 gCurrentPalette = PartyColors_p;