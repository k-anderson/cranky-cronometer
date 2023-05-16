#include "main.h"
#include "palettes.h"
#include "patterns.h"
#include "leds.h"
#include "effects.h"
#include "clock.h"

// #include <FastLED.h>

FASTLED_USING_NAMESPACE

uint8_t button_brightness = 25;

void button_led_effects(uint_fast8_t state);
void next_auto_palette();
void next_auto_pattern();

void setup_effects()
{
  FastLED.addLeds<NEOPIXEL, BUTTON_LEDS_PIN>(button_leds, BUTTON_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<NEOPIXEL, EFFECT_LEDS_PIN>(effect_leds, EFFECT_LEDS).setCorrection(TypicalLEDStrip);

  effects_brightness();
  buttons_brightness();

  set_pattern();
  set_palette();

  random16_set_seed(analogRead(34));

  next_auto_pattern();
  next_auto_palette();

  FastLED.clear();
  FastLED.show();
}

void effects_brightness()
{
  effects_brightness(preferences.getUChar(PREFERENCES_EFFECT_BRIGHTNESS, DEFAULT_LED_BRIGHTNESS) * 10);
}

void effects_brightness(uint8_t brightness)
{
  FastLED.setBrightness(brightness);
}

void buttons_brightness()
{
  buttons_brightness(preferences.getUChar(PREFERENCES_BUTTON_BRIGHTNESS, DEFAULT_BUTTON_BRIGHTNESS) * 10);
}

void buttons_brightness(uint8_t brightness)
{
  button_brightness = brightness;
}

void set_pattern()
{
  set_pattern(preferences.getUChar(PREFERENCES_EFFECT_PATTERN, DEFAULT_EFFECT_PATTERN));
}

void set_pattern(uint8_t pattern)
{
  current_pattern_number = pattern;
  if (current_pattern_number != 0)
  {
    manual_pattern_number = pattern;
  }
}

void next_pattern()
{
  manual_pattern_number++;
  if (manual_pattern_number >= number_of_patterns())
  {
    manual_pattern_number = 1;
  }

  if (manual_pattern_number == 0)
  {
    manual_pattern_number = 1;
  }

  if (current_pattern_number == 0)
  {
    auto_pattern_number = manual_pattern_number;
    /*
    Serial.print("set auto pattern to ");
    Serial.println(get_pattern_name(auto_pattern_number));
    */
  }
  else
  {
    current_pattern_number = manual_pattern_number;
    /*
    Serial.print("set current pattern to ");
    Serial.println(get_pattern_name(current_pattern_number));
    */
  }
}

const uint8_t number_of_patterns()
{
  return sizeof(gPatterns) / sizeof(gPatterns[0]);
}

String get_pattern_name()
{
  return get_pattern_name(manual_pattern_number);
}

String get_pattern_name(uint8_t pattern)
{
  return gPatterns[pattern].name;
}

void set_palette()
{
  set_palette(preferences.getUChar(PREFERENCES_EFFECT_PALETTE, DEFAULT_EFFECT_PALETTE));
}

void set_palette(uint8_t palette)
{
  current_palette_number = palette;
  if (current_palette_number != 0)
  {
    manual_palette_number = palette;
  }

  if (current_palette_number)
  {
    gCurrentPalette = *(gPalettes[current_palette_number].palette);
  }
}

void next_palette()
{
  manual_palette_number++;
  if (manual_palette_number >= number_of_palettes())
  {
    manual_palette_number = 1;
  }

  if (manual_palette_number == 0)
  {
    manual_palette_number = 1;
  }

  if (current_palette_number == 0)
  {
    auto_palette_number = manual_palette_number;
    /*
    Serial.print("set auto palette to ");
    Serial.println(get_palette_name(auto_palette_number));
    */
  }
  else
  {
    current_palette_number = manual_palette_number;
    /*
    Serial.print("set current palette to ");
    Serial.println(get_palette_name(current_palette_number));
    */
  }
}

const uint8_t number_of_palettes()
{
  return sizeof(gPalettes) / sizeof(gPalettes[0]);
}

String get_palette_name()
{
  return get_palette_name(manual_palette_number);
}

String get_palette_name(uint8_t palette)
{
  return gPalettes[palette].name;
}

uint8_t calculate_hourly_bells()
{
  if (!preferences.getUChar(PREFERENCES_HOUR_BELLS, DEFAULT_HOUR_BELLS))
  {
    return 0;
  }

  switch (get_hours())
  {
  case 4:
  case 8:
  case 12:
  case 16:
  case 20:
  case 0:
    return 8;
  case 1:
  case 5:
  case 9:
  case 13:
  case 17:
  case 21:
    return 2;
  case 2:
  case 6:
  case 10:
  case 14:
  case 18:
  case 22:
    return 4;
  case 3:
  case 7:
  case 11:
  case 15:
  case 19:
  case 23:
    return 6;
  default:
    return 0;
  }
}

uint8_t calculate_half_hour_bells()
{
  if (!preferences.getUChar(PREFERENCES_HALF_HOUR_BELLS, DEFAULT_HALF_HOUR_BELLS))
  {
    return 0;
  }

  switch (get_hours())
  {
  case 4:
  case 8:
  case 12:
  case 16:
  case 20:
  case 0:
    return 1;
  case 1:
  case 5:
  case 9:
  case 13:
  case 17:
  case 21:
    return 3;
  case 2:
  case 6:
  case 10:
  case 14:
  case 18:
  case 22:
    return 5;
  case 3:
  case 7:
  case 11:
  case 15:
  case 19:
  case 23:
    return 7;
  default:
    return 0;
  }
}

void effects_loop(uint_fast8_t state)
{
  static bool minutes_advance = true;
  uint8_t current_minutes = get_minutes();
  static uint8_t prev_minutes = current_minutes;
  static bool do_bells = false;
  static uint8_t bells = 0;
  static CRGB prev_effect_leds[EFFECT_LEDS];

  /* on the hours and every 15 minutes change the pattern */
  switch (current_minutes)
  {
  case 0:
    if (!do_bells)
    {
      bells = calculate_hourly_bells();
      bells *= 2;
      do_bells = true;
      for (uint_fast8_t i = 0; i < EFFECT_LEDS; i++)
      {
        prev_effect_leds[i] = effect_leds[i];
      }
    }
    if (minutes_advance)
    {
      next_auto_pattern();
      minutes_advance = false;
    }
    break;
  case 15:
    if (minutes_advance)
    {
      next_auto_pattern();
      minutes_advance = false;
    }
    break;
  case 30:
    if (!do_bells)
    {
      bells = calculate_half_hour_bells();
      bells *= 2;
      do_bells = true;
      for (uint_fast8_t i = 0; i < EFFECT_LEDS; i++)
      {
        prev_effect_leds[i] = effect_leds[i];
      }
    }
    if (minutes_advance)
    {
      next_auto_pattern();
      minutes_advance = false;
    }
    break;
  case 45:
    if (minutes_advance)
    {
      next_auto_pattern();
      minutes_advance = false;
    }
    break;
  default:
    do_bells = false;
    minutes_advance = true;
    break;
  }

  /* on the minute change the palette */
  if (current_minutes != prev_minutes)
  {
    next_auto_palette();
  }

  prev_minutes = current_minutes;

  /* update the leds showing the buttons action */
  button_led_effects(state);

  /* simulate ships bells via white / black flashing */
  if (do_bells && bells)
  {
    EVERY_N_MILLISECONDS(300)
    {
      if (bells % 2)
      {
        for (uint_fast8_t i = 0; i < EFFECT_LEDS; i++)
        {
          effect_leds[i] = prev_effect_leds[i];
        }
      }
      else
      {
        uint8_t brightness = preferences.getUChar(PREFERENCES_EFFECT_BRIGHTNESS, DEFAULT_LED_BRIGHTNESS) * 10;
        for (uint_fast8_t i = 0; i < EFFECT_LEDS; i++)
        {
          effect_leds[i] = CRGB::White;
          effect_leds[i].maximizeBrightness(brightness * .65);
        }
      }
      bells--;
    }
    FastLED.show();
    return;
  }

  /* if its in birthday mode, just show the joke */
  if (state == BIRTHDAY)
  {
    for (uint_fast8_t i = 0; i < EFFECT_LEDS; i++)
    {
      effect_leds[i] = CRGB::White;
    }
    FastLED.show();
    return;
  }

  // Add entropy to random number generator; we use a lot of it.
  random16_add_entropy(analogRead(34));

  EVERY_N_MILLISECONDS(1000 / FRAMES_PER_SECOND)
  {
    CRGBPalette16 gTargetPalette;

    if (current_palette_number == 0)
    {
      gTargetPalette = *(gPalettes[auto_palette_number].palette);
    }
    else
    {
      gTargetPalette = *(gPalettes[current_palette_number].palette);
    }
    nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, 10);

    frame_hue++;

    gPatterns[current_pattern_number].pattern();

    /* glitter bomb if the timer is complete */
    if (state == TIMER_RUNNING && timer == 0)
    {
      for (uint_fast8_t i = 0; i < EFFECT_LEDS; i++)
      {
        effect_leds[i] = CRGB::Black;
      }
      add_glitter(50);
    }
    FastLED.show();
  }
}

void button_led_effects(uint_fast8_t state)
{
  for (uint_fast8_t i = 0; i < BUTTON_LEDS; i++)
  {
    button_leds[i] = CRGB::Black;
  }

  switch (state)
  {
  case STOPWATCH_PAUSED:
  case STOPWATCH_RUNNING:
    button_leds[0] = CRGB::Green;
    button_leds[2] = CRGB::White;
    button_leds[3] = CRGB::White;
    break;

  case TIMER_CHANGE_TIME:
  case TIMER_SET_HOURS:
  case TIMER_SET_MINUTES:
  case TIMER_SET_SECONDS:
    button_leds[4] = CRGB::Green;
    button_leds[5] = CRGB::White;
    button_leds[6] = CRGB::White;
    button_leds[7] = CRGB::Red;
    break;

  case TIMER_PAUSED:
  case TIMER_RUNNING:
    button_leds[0] = CRGB::Green;
    button_leds[1] = CRGB::White;
    button_leds[2] = CRGB::White;
    button_leds[3] = CRGB::White;
    break;

  case CHANGE_MODE:
  case SET_MODE:
  case CHANGE_DISPLAY_FORMAT:
  case SET_DISPLAY_FORMAT:
  case CHANGE_CLOCK_BRIGHTNESS:
  case SET_CLOCK_BRIGHTNESS:
  case CHANGE_BUTTON_BRIGHTNESS:
  case SET_BUTTON_BRIGHTNESS:
  case CHANGE_EFFECT_BRIGHTNESS:
  case SET_EFFECT_BRIGHTNESS:
  case CHANGE_EFFECT_PATTERN:
  case SET_EFFECT_PATTERN:
  case CHANGE_EFFECT_PALETTE:
  case SET_EFFECT_PALETTE:
  case CHANGE_24HOUR:
  case SET_24HOUR:
  case CHANGE_LEADING_ZEROS:
  case SET_LEADING_ZEROS:
  case CHANGE_HOUR_BELLS:
  case SET_HOUR_BELLS:
  case CHANGE_HALF_HOUR_BELLS:
  case SET_HALF_HOUR_BELLS:
  case CHANGE_DATE:
  case SET_DATE_MONTH:
  case SET_DATE_DAY:
  case SET_DATE_YEAR:
  case CHANGE_TIME:
  case SET_TIME_HOURS:
  case SET_TIME_MINUTES:
  case SET_TIME_SECONDS:
  case CHANGE_TARGET_DATE:
  case SET_TARGET_MONTH:
  case SET_TARGET_DAY:
  case SET_TARGET_YEAR:
  case CHANGE_TARGET_TIME:
  case SET_TARGET_HOURS:
  case SET_TARGET_MINUTES:
  case SET_TARGET_SECONDS:
  case RESET_PREFERENCES:
    button_leds[4] = CRGB::Green;
    button_leds[5] = CRGB::White;
    button_leds[6] = CRGB::White;
    button_leds[7] = CRGB::Red;
    break;
  case CONFIRM_RESET_PREFERENCES:
    button_leds[4] = CRGB::Green;
    button_leds[5] = CRGB::White;
    button_leds[6] = CRGB::White;
    button_leds[7] = CRGB::Red;
    break;

  case CLOCK:
  case BIRTHDAY:
  default:
    button_leds[0] = CRGB::Green;
    break;
  }

  for (uint_fast8_t i = 0; i < BUTTON_LEDS; i++)
  {
    button_leds[i].maximizeBrightness(button_brightness);
  }
}

void next_auto_palette()
{
  const uint8_t number_of_palettes = sizeof(gPalettes) / sizeof(gPalettes[0]);
  static std::vector<uint8_t> available_palettes;
  uint8_t index = 0;

  if (available_palettes.empty())
  {
    for (int i = 0; i < number_of_palettes; i++)
    {
      if (gPalettes[i].auto_cycle)
      {
        available_palettes.push_back(i);
      }
    }
  }

  index = random16(available_palettes.size());
  auto_palette_number = available_palettes[index];
  available_palettes.erase(available_palettes.begin() + index);

  /*
  Serial.print("moving auto to palette ");
  Serial.println(get_palette_name(auto_palette_number));
  */
}

void next_auto_pattern()
{
  const uint8_t number_of_patterns = sizeof(gPatterns) / sizeof(gPatterns[0]);
  static std::vector<uint8_t> available_patterns;
  uint8_t index = 0;

  if (available_patterns.empty())
  {
    for (int i = 0; i < number_of_patterns; i++)
    {
      if (gPatterns[i].auto_cycle)
      {
        available_patterns.push_back(i);
      }
    }
  }

  index = random16(available_patterns.size());
  auto_pattern_number = available_patterns[index];
  available_patterns.erase(available_patterns.begin() + index);

  /*
  Serial.print("moving auto to pattern ");
  Serial.print(get_pattern_name(auto_pattern_number));
  Serial.println();
  */
}
