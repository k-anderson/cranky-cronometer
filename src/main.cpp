#include "main.h"
#include "clock.h"
#include "display.h"
#include "effects.h"
#include <Wire.h>

#define I2C_SDA 21
#define I2C_SCL 22

#define BUTTON_MENU 17
#define BUTTON_UP 5
#define BUTTON_DOWN 18
#define BUTTON_CONFIRM 19

#define PDLC_RELAY 14

#define DEBOUNCE_TIME_uS 150000

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

Preferences preferences;
uint_fast32_t timer = 0;

uint_fast8_t state = BIRTHDAY;
uint_fast8_t mode = MODE_BIRTHDAY;

uint_fast16_t menu_variable = 0;
uint_fast16_t menu_variable_max = 0;
uint_fast16_t menu_variable_min = 0;
bool menu_variable_modified = false;
bool confirm = false;

bool stopwatch_reset = false;
bool stopwatch_show_state = false;
u_long stopwatch_last_micros = 0;

uint_fast32_t timer_setpoint = 0;
bool timer_reset = false;
bool timer_show_state = false;
u_long timer_last_micros = 0;

bool ignore_buttons = false;
bool serial_debug = false;

bool trigger_next_pattern = false;
u_long trigger_next_palette = 0;

void setup_hadware();
void setup_preferences();
void setup_i2c();
void setup_state();
void loop();
void handle_confirm_action();
bool handle_state_change();
void handle_display();
void handle_stopwatch();
void handle_timer();
String get_state_name(uint_fast8_t state);
String get_mode_name(uint_fast8_t mode);
String get_display_name(uint_fast8_t display_format);
String get_bool_name(uint_fast8_t boolean);
void IRAM_ATTR handle_menu_button();
void IRAM_ATTR handle_up_button();
void IRAM_ATTR handle_down_button();
void IRAM_ATTR handle_confirm_button();

void setup()
{
  Serial.begin(9600);
  // Serial.begin(115200);
  Serial.println();

  setup_hadware();

  setup_preferences();

  setup_i2c();

  setup_display();

  setup_clock();

  setup_effects();

  setup_state();

  // WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void setup_hadware()
{
  pinMode(BUTTON_MENU, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_MENU), handle_menu_button, FALLING);

  pinMode(BUTTON_UP, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_UP), handle_up_button, FALLING);

  pinMode(BUTTON_DOWN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_DOWN), handle_down_button, FALLING);

  pinMode(BUTTON_CONFIRM, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_CONFIRM), handle_confirm_button, CHANGE);

  pinMode(PDLC_RELAY, OUTPUT);
  digitalWrite(PDLC_RELAY, 1);
}

void setup_preferences()
{
  preferences.begin("clock", false);
}

void setup_i2c()
{
  byte error, address;

  Wire.begin();

  Serial.println("scanning i2c...");

  for (address = 1; address < 127; address++)
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("i2c device found at address 0x");
      if (address < 16)
      {
        Serial.print("0");
      }
      Serial.print(address, HEX);
      Serial.println();
    }
    else if (error == 4)
    {
      Serial.print("unknown i2c error at address 0x");
      if (address < 16)
      {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }
  }
}

void setup_state()
{
  mode = preferences.getUChar(PREFERENCES_MODE, DEFAULT_MODE);

  switch (mode)
  {
  case MODE_BIRTHDAY:
    state = BIRTHDAY;
    break;
  case MODE_CLOCK:
    state = CLOCK;
    break;
  case MODE_TIMER:
    state = TIMER_PAUSED;
    break;
  case MODE_STOPWATCH:
    state = STOPWATCH_PAUSED;
    break;
  }
}

void loop()
{
  handle_confirm_action();

  handle_state_change();

  handle_display();

  effects_loop(state);
}

void handle_confirm_action()
{
  if (!confirm)
  {
    return;
  }

  uint_fast8_t hours = 0;
  uint_fast8_t minutes = 0;
  uint_fast8_t seconds = 0;

  // Serial.printf("confirm %s with value %u\n", get_state_name(state).c_str(), menu_variable);

  switch (state)
  {
  case STOPWATCH_PAUSED:
    state = STOPWATCH_RUNNING;
    break;

  case STOPWATCH_RUNNING:
    state = STOPWATCH_PAUSED;
    break;

  case TIMER_PAUSED:
    state = TIMER_RUNNING;
    break;

  case TIMER_RUNNING:
    state = TIMER_PAUSED;
    break;

  case TIMER_SET_HOURS:
    minutes = (timer_setpoint / SECONDS_PER_MINUTE) % SECONDS_PER_MINUTE;
    seconds = timer_setpoint % SECONDS_PER_MINUTE;
    timer_setpoint = (menu_variable * SECONDS_PER_HOUR) + (minutes * SECONDS_PER_MINUTE) + seconds;
    state = TIMER_SET_MINUTES;
    break;
  case TIMER_SET_MINUTES:
    hours = (timer_setpoint / SECONDS_PER_HOUR) % SECONDS_PER_HOUR;
    seconds = timer_setpoint % SECONDS_PER_MINUTE;
    timer_setpoint = (hours * SECONDS_PER_HOUR) + (menu_variable * SECONDS_PER_MINUTE) + seconds;
    state = TIMER_SET_SECONDS;
    break;
  case TIMER_SET_SECONDS:
    hours = (timer_setpoint / SECONDS_PER_HOUR) % SECONDS_PER_HOUR;
    minutes = (timer_setpoint / SECONDS_PER_MINUTE) % SECONDS_PER_MINUTE;
    timer_setpoint = (hours * SECONDS_PER_HOUR) + (minutes * SECONDS_PER_MINUTE) + menu_variable;
    timer_reset = true;
    state = TIMER_PAUSED;
    break;

  case SET_MODE:
    if (menu_variable != preferences.getUChar(PREFERENCES_MODE, DEFAULT_MODE))
    {
      preferences.putUChar(PREFERENCES_MODE, menu_variable);
    }
    mode = menu_variable;
    state = CHANGE_MODE;
    break;

  case SET_DISPLAY_FORMAT:
    if (menu_variable != preferences.getUChar(PREFERENCES_DISPLAY_FORMAT, DEFAULT_DISPLAY_FORMAT))
    {
      preferences.putUChar(PREFERENCES_DISPLAY_FORMAT, menu_variable);
    }
    state = CHANGE_DISPLAY_FORMAT;
    break;

  case SET_24HOUR:
    if (menu_variable != preferences.getBool(PREFERENCES_24HOUR, DEFAULT_24HOUR))
    {
      preferences.putBool(PREFERENCES_24HOUR, menu_variable);
    }
    state = CHANGE_24HOUR;
    break;

  case SET_LEADING_ZEROS:
    if (menu_variable != preferences.getUChar(PREFERENCES_LEADING_ZEROS, DEFAULT_LEADING_ZEROS))
    {
      preferences.putUChar(PREFERENCES_LEADING_ZEROS, menu_variable);
    }
    state = CHANGE_LEADING_ZEROS;
    break;

  case SET_HOUR_BELLS:
    if (menu_variable != preferences.getUChar(PREFERENCES_HOUR_BELLS, DEFAULT_HOUR_BELLS))
    {
      preferences.putUChar(PREFERENCES_HOUR_BELLS, menu_variable);
    }
    state = CHANGE_HOUR_BELLS;
    break;

  case SET_HALF_HOUR_BELLS:
    if (menu_variable != preferences.getUChar(PREFERENCES_HALF_HOUR_BELLS, DEFAULT_HALF_HOUR_BELLS))
    {
      preferences.putUChar(PREFERENCES_HALF_HOUR_BELLS, menu_variable);
    }
    state = CHANGE_HALF_HOUR_BELLS;
    break;

  case SET_CLOCK_BRIGHTNESS:
    if (menu_variable != preferences.getUChar(PREFERENCES_BRIGHTNESS, DEFAULT_BRIGHTNESS))
    {
      preferences.putUChar(PREFERENCES_BRIGHTNESS, menu_variable);
    }
    state = CHANGE_CLOCK_BRIGHTNESS;
    break;

  case SET_BUTTON_BRIGHTNESS:
    if (menu_variable != preferences.getUChar(PREFERENCES_BUTTON_BRIGHTNESS, DEFAULT_BUTTON_BRIGHTNESS))
    {
      preferences.putUChar(PREFERENCES_BUTTON_BRIGHTNESS, menu_variable);
    }
    state = CHANGE_BUTTON_BRIGHTNESS;
    break;

  case SET_EFFECT_BRIGHTNESS:
    if (menu_variable != preferences.getUChar(PREFERENCES_EFFECT_BRIGHTNESS, DEFAULT_LED_BRIGHTNESS))
    {
      preferences.putUChar(PREFERENCES_EFFECT_BRIGHTNESS, menu_variable);
    }
    state = CHANGE_EFFECT_BRIGHTNESS;
    break;

  case SET_EFFECT_PATTERN:
    if (menu_variable != preferences.getUChar(PREFERENCES_EFFECT_PATTERN, DEFAULT_EFFECT_PATTERN))
    {
      preferences.putUChar(PREFERENCES_EFFECT_PATTERN, menu_variable);
    }
    state = CHANGE_EFFECT_PATTERN;
    break;

  case SET_EFFECT_PALETTE:
    if (menu_variable != preferences.getUChar(PREFERENCES_EFFECT_PALETTE, DEFAULT_EFFECT_PALETTE))
    {
      preferences.putUChar(PREFERENCES_EFFECT_PALETTE, menu_variable);
    }
    state = CHANGE_EFFECT_PALETTE;
    break;

  case SET_DATE_MONTH:
    if (menu_variable_modified)
    {
      set_month(menu_variable);
    }
    state = SET_DATE_DAY;
    break;
  case SET_DATE_DAY:
    if (menu_variable_modified)
    {
      set_date(menu_variable);
    }
    state = SET_DATE_YEAR;
    break;
  case SET_DATE_YEAR:
    if (menu_variable_modified)
    {
      set_year(menu_variable);
    }
    state = CHANGE_DATE;
    break;

  case SET_TIME_HOURS:
    if (menu_variable_modified)
    {
      set_hours(menu_variable);
    }
    state = SET_TIME_MINUTES;
    break;
  case SET_TIME_MINUTES:
    if (menu_variable_modified)
    {
      set_minutes(menu_variable);
    }
    state = SET_TIME_SECONDS;
    break;
  case SET_TIME_SECONDS:
    if (menu_variable_modified)
    {
      set_seconds(menu_variable);
    }
    state = CHANGE_TIME;
    break;

  case SET_TARGET_MONTH:
    if (menu_variable != preferences.getUChar(PREFERENCES_BIRTH_MONTH, DEFAULT_BIRTH_MONTH))
    {
      preferences.putUChar(PREFERENCES_BIRTH_MONTH, menu_variable);
    }
    state = SET_TARGET_DAY;
    break;
  case SET_TARGET_DAY:
    if (menu_variable != preferences.getUChar(PREFERENCES_BIRTH_DATE, DEFAULT_BIRTH_DATE))
    {
      preferences.putUChar(PREFERENCES_BIRTH_DATE, menu_variable);
    }
    state = SET_TARGET_YEAR;
    break;
  case SET_TARGET_YEAR:
    if (menu_variable != preferences.getUShort(PREFERENCES_BIRTH_YEAR, DEFAULT_BIRTH_YEAR))
    {
      preferences.getUShort(PREFERENCES_BIRTH_YEAR, menu_variable);
    }
    state = CHANGE_TARGET_DATE;
    break;

  case SET_TARGET_HOURS:
    if (menu_variable != preferences.getUChar(PREFERENCES_BIRTH_HOUR, DEFAULT_BIRTH_HOUR))
    {
      preferences.putUChar(PREFERENCES_BIRTH_HOUR, menu_variable);
    }
    state = SET_TARGET_MINUTES;
    break;
  case SET_TARGET_MINUTES:
    if (menu_variable != preferences.getUChar(PREFERENCES_BIRTH_MINUTE, DEFAULT_BIRTH_MINUTE))
    {
      preferences.putUChar(PREFERENCES_BIRTH_MINUTE, menu_variable);
    }
    state = SET_TARGET_SECONDS;
    break;
  case SET_TARGET_SECONDS:
    if (menu_variable != preferences.getUChar(PREFERENCES_BIRTH_SECOND, DEFAULT_BIRTH_SECOND))
    {
      preferences.putUChar(PREFERENCES_BIRTH_SECOND, menu_variable);
    }
    state = CHANGE_TARGET_TIME;
    break;

  case CONFIRM_RESET_PREFERENCES:
    // https://randomnerdtutorials.com/esp32-save-data-permanently-preferences/
    preferences.clear();
    setup();
    break;
  }

  confirm = false;
}

bool handle_state_change()
{
  static uint_fast8_t last_state = LAST_STATE;

  if (last_state == state)
  {
    return false;
  }

  int pdlc_state = digitalRead(PDLC_RELAY);
  if (state == BIRTHDAY && pdlc_state == 1)
  {
    ignore_buttons = true;
    digitalWrite(PDLC_RELAY, 0);
    delay(150);
    ignore_buttons = false;
  }
  else if (pdlc_state == 0)
  {
    ignore_buttons = true;
    digitalWrite(PDLC_RELAY, 1);
    delay(150);
    ignore_buttons = false;
  }

  // Serial.printf("%s -> %s\n", get_state_name(last_state).c_str(), get_state_name(state).c_str());
  last_state = state;

  display_reset();

  switch (state)
  {
  case BIRTHDAY:
  case CLOCK:
    break;

  case STOPWATCH_PAUSED:
    stopwatch_last_micros = micros();
    stopwatch_show_state = true;
    break;
  case STOPWATCH_RUNNING:
    stopwatch_last_micros = micros();
    stopwatch_show_state = false;
    break;

  case TIMER_PAUSED:
    timer_last_micros = micros();
    timer_show_state = true;
    break;
  case TIMER_RUNNING:
    timer_last_micros = micros();
    timer_show_state = false;
    break;

  case TIMER_SET_HOURS:
    timer_setpoint = 0;
    menu_variable = (timer_setpoint / SECONDS_PER_HOUR) % SECONDS_PER_HOUR;
    menu_variable_min = 0;
    menu_variable_max = 99;
    display_blink(1, 1);
    break;
  case TIMER_SET_MINUTES:
    menu_variable = (timer_setpoint / SECONDS_PER_MINUTE) % SECONDS_PER_MINUTE;
    menu_variable_min = 0;
    menu_variable_max = 59;
    display_blink(2, 1);
    break;
  case TIMER_SET_SECONDS:
    menu_variable = timer_setpoint % SECONDS_PER_MINUTE;
    menu_variable_min = 0;
    menu_variable_max = 59;
    display_blink(3, 1);
    break;

  case SET_MODE:
    menu_variable = preferences.getUChar(PREFERENCES_MODE, DEFAULT_MODE);
    menu_variable_min = 0;
    menu_variable_max = 3;
    break;

  case SET_DISPLAY_FORMAT:
    menu_variable = preferences.getUChar(PREFERENCES_DISPLAY_FORMAT, DEFAULT_DISPLAY_FORMAT);
    menu_variable_min = 0;
    menu_variable_max = 4;
    break;

  case SET_CLOCK_BRIGHTNESS:
    menu_variable = preferences.getUChar(PREFERENCES_BRIGHTNESS, DEFAULT_BRIGHTNESS);
    menu_variable_min = 1;
    menu_variable_max = 15;
    break;

  case SET_24HOUR:
    menu_variable = is_24_hour();
    menu_variable_min = 0;
    menu_variable_max = 1;
    break;

  case SET_LEADING_ZEROS:
    menu_variable = preferences.getUChar(PREFERENCES_LEADING_ZEROS, DEFAULT_LEADING_ZEROS);
    menu_variable_min = 0;
    menu_variable_max = 1;
    break;

  case SET_HOUR_BELLS:
    menu_variable = preferences.getUChar(PREFERENCES_HOUR_BELLS, DEFAULT_HOUR_BELLS);
    menu_variable_min = 0;
    menu_variable_max = 1;
    break;

  case SET_HALF_HOUR_BELLS:
    menu_variable = preferences.getUChar(PREFERENCES_HALF_HOUR_BELLS, DEFAULT_HALF_HOUR_BELLS);
    menu_variable_min = 0;
    menu_variable_max = 1;
    break;

  case SET_DATE_MONTH:
    menu_variable = get_month();
    menu_variable_min = 1;
    menu_variable_max = 12;
    display_blink(1, 1);
    break;
  case SET_DATE_DAY:
    menu_variable = get_date();
    menu_variable_min = 1;
    menu_variable_max = 31;
    display_blink(2, 1);
    break;
  case SET_DATE_YEAR:
    menu_variable = get_year();
    menu_variable_min = 0;
    menu_variable_max = 9999;
    display_blink(3, 1);
    break;

  case SET_TIME_HOURS:
    menu_variable = get_hours();
    menu_variable_min = 0;
    menu_variable_max = 23;
    display_blink(1, 1);
    break;
  case SET_TIME_MINUTES:
    menu_variable = get_minutes();
    menu_variable_min = 0;
    menu_variable_max = 59;
    display_blink(2, 1);
    break;
  case SET_TIME_SECONDS:
    menu_variable = get_seconds();
    menu_variable_min = 0;
    menu_variable_max = 59;
    display_blink(3, 1);
    break;

  case SET_TARGET_MONTH:
    menu_variable = preferences.getUChar(PREFERENCES_BIRTH_MONTH, DEFAULT_BIRTH_MONTH);
    menu_variable_min = 1;
    menu_variable_max = 12;
    display_blink(1, 1);
    break;
  case SET_TARGET_DAY:
    menu_variable = preferences.getUChar(PREFERENCES_BIRTH_DATE, DEFAULT_BIRTH_DATE);
    menu_variable_min = 1;
    menu_variable_max = 31;
    display_blink(2, 1);
    break;
  case SET_TARGET_YEAR:
    menu_variable = preferences.getUShort(PREFERENCES_BIRTH_YEAR, DEFAULT_BIRTH_YEAR);
    menu_variable_min = 0;
    menu_variable_max = 9999;
    display_blink(3, 1);
    break;

  case SET_TARGET_HOURS:
    menu_variable = preferences.getUChar(PREFERENCES_BIRTH_HOUR, DEFAULT_BIRTH_HOUR);
    menu_variable_min = 0;
    menu_variable_max = 23;
    display_blink(1, 1);
    break;
  case SET_TARGET_MINUTES:
    menu_variable = preferences.getUChar(PREFERENCES_BIRTH_MINUTE, DEFAULT_BIRTH_MINUTE);
    menu_variable_min = 0;
    menu_variable_max = 59;
    display_blink(2, 1);
    break;
  case SET_TARGET_SECONDS:
    menu_variable = preferences.getUChar(PREFERENCES_BIRTH_SECOND, DEFAULT_BIRTH_SECOND);
    menu_variable_min = 0;
    menu_variable_max = 59;
    display_blink(3, 1);
    break;

  case SET_BUTTON_BRIGHTNESS:
    menu_variable = preferences.getUChar(PREFERENCES_BUTTON_BRIGHTNESS, DEFAULT_BUTTON_BRIGHTNESS);
    menu_variable_min = 0;
    menu_variable_max = 25;
    break;

  case SET_EFFECT_BRIGHTNESS:
    menu_variable = preferences.getUChar(PREFERENCES_EFFECT_BRIGHTNESS, DEFAULT_LED_BRIGHTNESS);
    menu_variable_min = 0;
    menu_variable_max = 25;
    break;

  case SET_EFFECT_PATTERN:
    menu_variable = preferences.getUChar(PREFERENCES_EFFECT_PATTERN, DEFAULT_EFFECT_PATTERN);
    menu_variable_min = 0;
    menu_variable_max = number_of_patterns() - 1;
    break;

  case SET_EFFECT_PALETTE:
    menu_variable = preferences.getUChar(PREFERENCES_EFFECT_PALETTE, DEFAULT_EFFECT_PALETTE);
    menu_variable_min = 0;
    menu_variable_max = number_of_palettes() - 1;
    break;

  case CHANGE_CLOCK_BRIGHTNESS:
    display_brightness(preferences.getUChar(PREFERENCES_BRIGHTNESS, DEFAULT_BRIGHTNESS));
    display_string(get_state_name(state));
    break;

  case CHANGE_EFFECT_BRIGHTNESS:
    effects_brightness();
    display_string(get_state_name(state));
    break;

  case CHANGE_EFFECT_PATTERN:
    set_pattern();
    display_string(get_state_name(state));
    break;

  case CHANGE_EFFECT_PALETTE:
    set_palette();
    display_string(get_state_name(state));
    break;

  default:
    display_string(get_state_name(state));
    break;
  }

  menu_variable_modified = false;

  return true;
}

void handle_display()
{
  uint_fast8_t hours = 0;
  uint_fast8_t minutes = 0;
  uint_fast8_t seconds = 0;
  static u_long manual_pattern_update = 0;
  static u_long manual_palette_update = 0;
  static bool startup_complete = false;

  if (trigger_next_pattern)
  {
    next_pattern();
    manual_pattern_update = micros();
    trigger_next_pattern = false;
    startup_complete = true;
  }

  if (trigger_next_palette)
  {
    next_palette();
    manual_palette_update = micros();
    trigger_next_palette = false;
    startup_complete = true;
  }

  switch (state)
  {
  case CLOCK:
    if (startup_complete && micros() - manual_pattern_update < 3000000)
    {
      display_string(get_pattern_name());
    }
    else if (startup_complete && micros() - manual_palette_update < 3000000)
    {
      display_string(get_palette_name());
    }
    else
    {
      display_clock();
    }
    break;

  case BIRTHDAY:
    if (micros() - manual_pattern_update < 3000000)
    {
      display_string(get_pattern_name());
    }
    else if (micros() - manual_palette_update < 3000000)
    {
      display_string(get_palette_name());
    }
    else
    {
      display_birthday();
    }
    break;

  case STOPWATCH_RUNNING:
  case STOPWATCH_PAUSED:
    handle_stopwatch();
    break;

  case TIMER_PAUSED:
  case TIMER_RUNNING:
    handle_timer();
    break;

  case TIMER_SET_HOURS:
    minutes = (timer_setpoint / SECONDS_PER_MINUTE) % SECONDS_PER_MINUTE;
    seconds = timer_setpoint % SECONDS_PER_MINUTE;
    display_time_components(menu_variable, minutes, seconds);
    break;
  case TIMER_SET_MINUTES:
    hours = (timer_setpoint / SECONDS_PER_HOUR) % SECONDS_PER_HOUR;
    seconds = timer_setpoint % SECONDS_PER_MINUTE;
    display_time_components(hours, menu_variable, seconds);
    break;
  case TIMER_SET_SECONDS:
    hours = (timer_setpoint / SECONDS_PER_HOUR) % SECONDS_PER_HOUR;
    minutes = (timer_setpoint / SECONDS_PER_MINUTE) % SECONDS_PER_MINUTE;
    display_time_components(hours, minutes, menu_variable);
    break;

  case SET_MODE:
    display_string(get_mode_name(menu_variable));
    break;

  case SET_DISPLAY_FORMAT:
    display_string(get_display_name(menu_variable));
    break;

  case SET_24HOUR:
    display_string(get_bool_name(menu_variable));
    break;

  case SET_LEADING_ZEROS:
    display_string(get_bool_name(menu_variable));
    break;

  case SET_HOUR_BELLS:
    display_string(get_bool_name(menu_variable));
    break;

  case SET_HALF_HOUR_BELLS:
    display_string(get_bool_name(menu_variable));
    break;

  case SET_DATE_MONTH:
    display_date_components(menu_variable, get_date(), get_year());
    break;
  case SET_DATE_DAY:
    display_date_components(get_month(), menu_variable, get_year());
    break;
  case SET_DATE_YEAR:
    display_date_components(get_month(), get_date(), menu_variable);
    break;

  case SET_TIME_HOURS:
    display_time_components(menu_variable, get_minutes(), get_seconds());
    break;
  case SET_TIME_MINUTES:
    display_time_components(get_hours(), menu_variable, get_seconds());
    break;
  case SET_TIME_SECONDS:
    display_time_components(get_hours(), get_minutes(), menu_variable);
    break;

  case SET_TARGET_MONTH:
    display_date_components(menu_variable, preferences.getUChar(PREFERENCES_BIRTH_DATE, DEFAULT_BIRTH_DATE), preferences.getUShort(PREFERENCES_BIRTH_YEAR, DEFAULT_BIRTH_YEAR));
    break;
  case SET_TARGET_DAY:
    display_date_components(preferences.getUChar(PREFERENCES_BIRTH_MONTH, DEFAULT_BIRTH_MONTH), menu_variable, preferences.getUShort(PREFERENCES_BIRTH_YEAR, DEFAULT_BIRTH_YEAR));
    break;
  case SET_TARGET_YEAR:
    display_date_components(preferences.getUChar(PREFERENCES_BIRTH_MONTH, DEFAULT_BIRTH_MONTH), preferences.getUChar(PREFERENCES_BIRTH_DATE, DEFAULT_BIRTH_DATE), menu_variable);
    break;

  case SET_TARGET_HOURS:
    display_time_components(menu_variable, preferences.getUChar(PREFERENCES_BIRTH_MINUTE, DEFAULT_BIRTH_MINUTE), preferences.getUChar(PREFERENCES_BIRTH_SECOND, DEFAULT_BIRTH_SECOND));
    break;
  case SET_TARGET_MINUTES:
    display_time_components(preferences.getUChar(PREFERENCES_BIRTH_HOUR, DEFAULT_BIRTH_HOUR), menu_variable, preferences.getUChar(PREFERENCES_BIRTH_SECOND, DEFAULT_BIRTH_SECOND));
    break;
  case SET_TARGET_SECONDS:
    display_time_components(preferences.getUChar(PREFERENCES_BIRTH_HOUR, DEFAULT_BIRTH_HOUR), preferences.getUChar(PREFERENCES_BIRTH_MINUTE, DEFAULT_BIRTH_MINUTE), menu_variable);
    break;

  case SET_CLOCK_BRIGHTNESS:
    display_brightness(menu_variable);
    display_value(menu_variable);
    break;

  case SET_BUTTON_BRIGHTNESS:
    buttons_brightness(menu_variable * 10);
    display_value(menu_variable);
    break;

  case SET_EFFECT_BRIGHTNESS:
    effects_brightness(menu_variable * 10);
    display_value(menu_variable);
    break;

  case SET_EFFECT_PATTERN:
    set_pattern(menu_variable);
    display_string(get_pattern_name(menu_variable));
    break;

  case SET_EFFECT_PALETTE:
    set_palette(menu_variable);
    display_string(get_palette_name(menu_variable));
    break;
  }
}

void handle_stopwatch()
{
  static uint_fast32_t stopwatch = 0;

  if (stopwatch_reset)
  {
    stopwatch_reset = false;
    stopwatch_last_micros = micros();
    stopwatch_show_state = false;
    stopwatch = 0;
  }

  if (state == STOPWATCH_PAUSED && stopwatch_show_state)
  {
    display_string("   PAUSED   ");
  }
  else
  {
    uint_fast8_t hours = (stopwatch / SECONDS_PER_HOUR) % SECONDS_PER_HOUR;
    uint_fast8_t minutes = (stopwatch / SECONDS_PER_MINUTE) % SECONDS_PER_MINUTE;
    uint_fast8_t seconds = stopwatch % SECONDS_PER_MINUTE;
    display_time_components(hours, minutes, seconds);
  }

  if (micros() - stopwatch_last_micros < MICROS_PER_SECOND)
  {
    return;
  }
  stopwatch_last_micros = micros();

  /*
   * at 99 hours stop the stopwatch
   * because we don't have resolution
   * on the display above that.
   */
  if (state == STOPWATCH_RUNNING && stopwatch < SECONDS_AT_99_HOURS)
  {
    stopwatch++;
  }
  else if (state == STOPWATCH_PAUSED)
  {
    stopwatch_show_state = !stopwatch_show_state;
  }
}

void handle_timer()
{
  if (timer_reset)
  {
    timer_reset = false;
    timer_last_micros = micros();
    timer_show_state = false;
    timer = timer_setpoint;
  }

  if (timer_show_state)
  {
    if (state == TIMER_PAUSED)
    {
      display_string("   PAUSED   ");
    }
    else if (timer == 0)
    {
      display_string("  COMPLETE  ");
    }
  }
  else
  {
    uint_fast8_t hours = (timer / SECONDS_PER_HOUR) % SECONDS_PER_HOUR;
    uint_fast8_t minutes = (timer / SECONDS_PER_MINUTE) % SECONDS_PER_MINUTE;
    uint_fast8_t seconds = timer % SECONDS_PER_MINUTE;
    display_time_components(hours, minutes, seconds);
  }

  if (micros() - timer_last_micros < MICROS_PER_SECOND)
  {
    return;
  }
  timer_last_micros = micros();

  if (state == TIMER_RUNNING && timer > 0)
  {
    timer--;
  }
  else if (state == TIMER_PAUSED || timer == 0)
  {
    timer_show_state = !timer_show_state;
  }
}

String get_state_name(uint_fast8_t state)
{
  switch (state)
  {
  case CLOCK:
    return "clock";
  case BIRTHDAY:
    return "birthday";
  case STOPWATCH_PAUSED:
    return "stopwatch paused";
  case STOPWATCH_RUNNING:
    return "stopwatch running";
  case TIMER_CHANGE_TIME:
    return "set timer";
  case TIMER_SET_HOURS:
    return "set timer hours";
  case TIMER_SET_MINUTES:
    return "set timer minutes";
  case TIMER_SET_SECONDS:
    return "set timer seconds";
  case TIMER_PAUSED:
    return "timer paused";
  case TIMER_RUNNING:
    return "timer running";
  case CHANGE_MODE:
    return "mode";
  case SET_MODE:
    return "set mode";
  case CHANGE_DISPLAY_FORMAT:
    return "clock format";
  case SET_DISPLAY_FORMAT:
    return "set display format";
  case CHANGE_CLOCK_BRIGHTNESS:
    return "c bright";
  case SET_CLOCK_BRIGHTNESS:
    return "set c bright";
  case CHANGE_24HOUR:
    return "24hr";
  case SET_24HOUR:
    return "set 24 mode";
  case CHANGE_LEADING_ZEROS:
    return "zeros";
  case SET_LEADING_ZEROS:
    return "set leading zeros";
  case CHANGE_HOUR_BELLS:
    return "hour bells";
  case SET_HOUR_BELLS:
    return "set hour bells";
  case CHANGE_HALF_HOUR_BELLS:
    return "1/2 hr bells";
  case SET_HALF_HOUR_BELLS:
    return "set 1/2 hr bells";
  case CHANGE_DATE:
    return "set date";
  case SET_DATE_MONTH:
    return "set date month";
  case SET_DATE_DAY:
    return "set date day";
  case SET_DATE_YEAR:
    return "set date year";
  case CHANGE_TIME:
    return "set time";
  case SET_TIME_HOURS:
    return "set time hours";
  case SET_TIME_MINUTES:
    return "set time minutes";
  case SET_TIME_SECONDS:
    return "set time seconds";
  case CHANGE_TARGET_DATE:
    return "set b date";
  case SET_TARGET_MONTH:
    return "set target month";
  case SET_TARGET_DAY:
    return "set target day";
  case SET_TARGET_YEAR:
    return "set target year";
  case CHANGE_TARGET_TIME:
    return "set b time";
  case SET_TARGET_HOURS:
    return "set targt hours";
  case SET_TARGET_MINUTES:
    return "set target minutes";
  case SET_TARGET_SECONDS:
    return "set target seconds";
  case CHANGE_BUTTON_BRIGHTNESS:
    return "b bright";
  case SET_BUTTON_BRIGHTNESS:
    return "set b bright";
  case CHANGE_EFFECT_BRIGHTNESS:
    return "e bright";
  case SET_EFFECT_BRIGHTNESS:
    return "set e bright";
  case CHANGE_EFFECT_PATTERN:
    return "effects";
  case SET_EFFECT_PATTERN:
    return "set effects";
  case CHANGE_EFFECT_PALETTE:
    return "palette";
  case SET_EFFECT_PALETTE:
    return "set palette";
  case RESET_PREFERENCES:
    return "reset";
  case CONFIRM_RESET_PREFERENCES:
    return "are you sure?";
  default:
    return "unknown";
  }
}

String get_mode_name(uint_fast8_t mode)
{
  switch (mode)
  {
  case 0:
    return "birthday";
  case 1:
    return "stopwatch";
  case 2:
    return "timer";
  case 3:
    return "clock";
  default:
    return "unknown";
  }
}

String get_display_name(uint_fast8_t display_format)
{
  switch (display_format)
  {
  case 0:
    return "cycle";
  case 1:
    return "date";
  case 2:
    return "time";
  case 3:
    return "time/date";
  case 4:
    return "time w/ sec";
  default:
    return "unknown";
  }
}

String get_bool_name(uint_fast8_t boolean)
{
  if (boolean)
  {
    return "on";
  }

  return "off";
}

void IRAM_ATTR handle_menu_button()
{
  static u_long debounce_micros = 0;

  if (ignore_buttons)
  {
    return;
  }

  if ((micros() - debounce_micros) < DEBOUNCE_TIME_uS)
  {
    return;
  }
  debounce_micros = micros();

  switch (state)
  {
  case CLOCK:
  case BIRTHDAY:
  case STOPWATCH_PAUSED:
  case STOPWATCH_RUNNING:
  case TIMER_CHANGE_TIME:
  case TIMER_PAUSED:
  case TIMER_RUNNING:
    state = CHANGE_MODE;
    break;

  case TIMER_SET_HOURS:
  case TIMER_SET_MINUTES:
  case TIMER_SET_SECONDS:
    state = TIMER_PAUSED;
    break;

  case CHANGE_MODE:
  case CHANGE_DISPLAY_FORMAT:
  case CHANGE_24HOUR:
  case CHANGE_LEADING_ZEROS:
  case CHANGE_HOUR_BELLS:
  case CHANGE_HALF_HOUR_BELLS:
  case CHANGE_CLOCK_BRIGHTNESS:
  case CHANGE_BUTTON_BRIGHTNESS:
  case CHANGE_EFFECT_BRIGHTNESS:
  case CHANGE_EFFECT_PATTERN:
  case CHANGE_EFFECT_PALETTE:
  case CHANGE_DATE:
  case CHANGE_TIME:
  case CHANGE_TARGET_DATE:
  case CHANGE_TARGET_TIME:
  case RESET_PREFERENCES:
    switch (mode)
    {
    case MODE_BIRTHDAY:
      state = BIRTHDAY;
      break;
    case MODE_STOPWATCH:
      state = STOPWATCH_PAUSED;
      break;
    case MODE_TIMER:
      state = TIMER_PAUSED;
      break;
    case MODE_CLOCK:
      state = CLOCK;
      break;
    }
    break;

  case SET_MODE:
    state = CHANGE_MODE;
    break;

  case SET_DISPLAY_FORMAT:
    state = CHANGE_DISPLAY_FORMAT;
    break;

  case SET_24HOUR:
    state = CHANGE_24HOUR;
    break;

  case SET_LEADING_ZEROS:
    state = CHANGE_LEADING_ZEROS;
    break;

  case SET_HOUR_BELLS:
    state = CHANGE_HOUR_BELLS;
    break;

  case SET_HALF_HOUR_BELLS:
    state = CHANGE_HALF_HOUR_BELLS;
    break;

  case SET_CLOCK_BRIGHTNESS:
    state = CHANGE_CLOCK_BRIGHTNESS;
    break;

  case SET_BUTTON_BRIGHTNESS:
    state = CHANGE_BUTTON_BRIGHTNESS;
    break;

  case SET_EFFECT_BRIGHTNESS:
    state = CHANGE_EFFECT_BRIGHTNESS;
    break;

  case SET_EFFECT_PATTERN:
    state = CHANGE_EFFECT_PATTERN;
    break;

  case SET_EFFECT_PALETTE:
    state = CHANGE_EFFECT_PALETTE;
    break;

  case SET_DATE_MONTH:
  case SET_DATE_DAY:
  case SET_DATE_YEAR:
    state = CHANGE_DATE;
    break;

  case SET_TIME_HOURS:
  case SET_TIME_MINUTES:
  case SET_TIME_SECONDS:
    state = CHANGE_TIME;
    break;

  case SET_TARGET_MONTH:
  case SET_TARGET_DAY:
  case SET_TARGET_YEAR:
    state = CHANGE_TARGET_DATE;
    break;

  case SET_TARGET_HOURS:
  case SET_TARGET_MINUTES:
  case SET_TARGET_SECONDS:
    state = CHANGE_TARGET_TIME;
    break;

  case CONFIRM_RESET_PREFERENCES:
    state = RESET_PREFERENCES;
    break;

  default:
    state = CLOCK;
    break;
  }
}

void IRAM_ATTR handle_up_button()
{
  static u_long debounce_micros = 0;

  if (ignore_buttons)
  {
    return;
  }

  if ((micros() - debounce_micros) < DEBOUNCE_TIME_uS)
  {
    return;
  }
  debounce_micros = micros();

  switch (state)
  {
  case CLOCK:
  case BIRTHDAY:
    trigger_next_pattern = true;
    break;

  case TIMER_PAUSED:
  case TIMER_RUNNING:
    state = TIMER_SET_HOURS;
    break;

  case CHANGE_MODE:
    state = CHANGE_DISPLAY_FORMAT;
    break;

  case CHANGE_DISPLAY_FORMAT:
    state = CHANGE_CLOCK_BRIGHTNESS;
    break;

  case CHANGE_CLOCK_BRIGHTNESS:
    state = CHANGE_BUTTON_BRIGHTNESS;
    break;

  case CHANGE_BUTTON_BRIGHTNESS:
    state = CHANGE_EFFECT_BRIGHTNESS;
    break;

  case CHANGE_EFFECT_BRIGHTNESS:
    state = CHANGE_EFFECT_PATTERN;
    break;

  case CHANGE_EFFECT_PATTERN:
    state = CHANGE_EFFECT_PALETTE;
    break;

  case CHANGE_EFFECT_PALETTE:
    state = CHANGE_24HOUR;
    break;

  case CHANGE_24HOUR:
    state = CHANGE_LEADING_ZEROS;
    break;

  case CHANGE_LEADING_ZEROS:
    state = CHANGE_HOUR_BELLS;
    break;

  case CHANGE_HOUR_BELLS:
    state = CHANGE_HALF_HOUR_BELLS;
    break;

  case CHANGE_HALF_HOUR_BELLS:
    state = CHANGE_DATE;
    break;

  case CHANGE_DATE:
    state = CHANGE_TIME;
    break;

  case CHANGE_TIME:
    state = CHANGE_TARGET_DATE;
    break;

  case CHANGE_TARGET_DATE:
    state = CHANGE_TARGET_TIME;
    break;

  case CHANGE_TARGET_TIME:
    state = RESET_PREFERENCES;
    break;

  case RESET_PREFERENCES:
    state = CHANGE_MODE;
    break;

  default:
    if (menu_variable == std::numeric_limits<std::uint_fast16_t>::max())
    {
      menu_variable = menu_variable_min;
    }
    else
    {
      menu_variable++;
      if (menu_variable > menu_variable_max)
      {
        menu_variable = menu_variable_min;
      }
    }
    menu_variable_modified = true;
    break;
  }
}

void IRAM_ATTR handle_down_button()
{
  static u_long debounce_micros = 0;

  if (ignore_buttons)
  {
    return;
  }

  if ((micros() - debounce_micros) < DEBOUNCE_TIME_uS)
  {
    return;
  }
  debounce_micros = micros();

  switch (state)
  {

  case CLOCK:
  case BIRTHDAY:
    trigger_next_palette = true;
    break;

  case STOPWATCH_PAUSED:
  case STOPWATCH_RUNNING:
    stopwatch_reset = true;
    break;

  case TIMER_PAUSED:
  case TIMER_RUNNING:
    timer_reset = true;
    break;

  case CHANGE_MODE:
    state = RESET_PREFERENCES;
    break;

  case CHANGE_DISPLAY_FORMAT:
    state = CHANGE_MODE;
    break;

  case CHANGE_CLOCK_BRIGHTNESS:
    state = CHANGE_DISPLAY_FORMAT;
    break;

  case CHANGE_BUTTON_BRIGHTNESS:
    state = CHANGE_CLOCK_BRIGHTNESS;
    break;

  case CHANGE_EFFECT_BRIGHTNESS:
    state = CHANGE_BUTTON_BRIGHTNESS;
    break;

  case CHANGE_EFFECT_PATTERN:
    state = CHANGE_EFFECT_BRIGHTNESS;
    break;

  case CHANGE_EFFECT_PALETTE:
    state = CHANGE_EFFECT_PATTERN;
    break;

  case CHANGE_24HOUR:
    state = CHANGE_EFFECT_PALETTE;
    break;

  case CHANGE_LEADING_ZEROS:
    state = CHANGE_24HOUR;
    break;

  case CHANGE_HOUR_BELLS:
    state = CHANGE_LEADING_ZEROS;
    break;

  case CHANGE_HALF_HOUR_BELLS:
    state = CHANGE_HOUR_BELLS;
    break;

  case CHANGE_DATE:
    state = CHANGE_HALF_HOUR_BELLS;
    break;

  case CHANGE_TIME:
    state = CHANGE_DATE;
    break;

  case CHANGE_TARGET_DATE:
    state = CHANGE_TIME;
    break;

  case CHANGE_TARGET_TIME:
    state = CHANGE_TARGET_DATE;
    break;

  case RESET_PREFERENCES:
    state = CHANGE_TARGET_TIME;
    break;

  default:
    if (menu_variable == std::numeric_limits<std::uint16_t>::min())
    {
      menu_variable = menu_variable_max;
    }
    else
    {
      menu_variable--;
      if (menu_variable < menu_variable_min)
      {
        menu_variable = menu_variable_max;
      }
    }
    menu_variable_modified = true;
    break;
  }
}

void IRAM_ATTR handle_confirm_button()
{
  static u_long debounce_micros = 0;
  static u_long confirm_press_micros = 0;
  static uint_fast8_t long_press_state = BIRTHDAY;

  if (ignore_buttons)
  {
    return;
  }

  if (!digitalRead(BUTTON_CONFIRM))
  {
    confirm_press_micros = micros();
    return;
  }
  else
  {
    if ((micros() - debounce_micros) < DEBOUNCE_TIME_uS)
    {
      return;
    }
    debounce_micros = micros();
  }

  if (micros() - confirm_press_micros >= 1000000)
  {
    switch (state)
    {
    case BIRTHDAY:
      state = long_press_state;
      return;
    case CLOCK:
    case STOPWATCH_PAUSED:
    case STOPWATCH_RUNNING:
    case TIMER_PAUSED:
    case TIMER_RUNNING:
    case TIMER_CHANGE_TIME:
    case TIMER_SET_HOURS:
    case TIMER_SET_MINUTES:
    case TIMER_SET_SECONDS:
      long_press_state = state;
      state = BIRTHDAY;
      return;
    default:
      break;
    }
  }

  switch (state)
  {
  case CHANGE_MODE:
    state = SET_MODE;
    break;

  case CHANGE_DISPLAY_FORMAT:
    state = SET_DISPLAY_FORMAT;
    break;

  case CHANGE_CLOCK_BRIGHTNESS:
    state = SET_CLOCK_BRIGHTNESS;
    break;

  case CHANGE_BUTTON_BRIGHTNESS:
    state = SET_BUTTON_BRIGHTNESS;
    break;

  case CHANGE_EFFECT_BRIGHTNESS:
    state = SET_EFFECT_BRIGHTNESS;
    break;

  case CHANGE_EFFECT_PATTERN:
    state = SET_EFFECT_PATTERN;
    break;

  case CHANGE_EFFECT_PALETTE:
    state = SET_EFFECT_PALETTE;
    break;

  case CHANGE_24HOUR:
    state = SET_24HOUR;
    break;

  case CHANGE_LEADING_ZEROS:
    state = SET_LEADING_ZEROS;
    break;

  case CHANGE_HOUR_BELLS:
    state = SET_HOUR_BELLS;
    break;

  case CHANGE_HALF_HOUR_BELLS:
    state = SET_HALF_HOUR_BELLS;
    break;

  case CHANGE_DATE:
    state = SET_DATE_MONTH;
    break;

  case CHANGE_TIME:
    state = SET_TIME_HOURS;
    break;

  case CHANGE_TARGET_DATE:
    state = SET_TARGET_MONTH;
    break;

  case CHANGE_TARGET_TIME:
    state = SET_TARGET_HOURS;
    break;

  case RESET_PREFERENCES:
    state = CONFIRM_RESET_PREFERENCES;
    break;

  default:
    confirm = true;
    break;
  }
}