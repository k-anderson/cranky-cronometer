#pragma once

void setup_effects();

void effects_brightness();
void effects_brightness(uint8_t brightness);

void buttons_brightness();
void buttons_brightness(uint8_t brightness);

void set_pattern();
void set_pattern(uint8_t pattern);
void next_pattern();
const uint8_t number_of_patterns();
String get_pattern_name();
String get_pattern_name(uint8_t pattern);

void set_palette();
void set_palette(uint8_t palette);
void next_palette();
const uint8_t number_of_palettes();
String get_palette_name();
String get_palette_name(uint8_t palette);

void effects_loop(uint_fast8_t state);