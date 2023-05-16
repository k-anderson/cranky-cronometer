#include "main.h"
#include "display.h"
#include "clock.h"
#include <SparkFun_Alphanumeric_Display.h>

#define RESOLUTION 12

HT16K33 display;

std::vector<char> display_buffer = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
std::vector<char> display_previous_buffer = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', false};

void setup_display()
{
  uint_fast8_t display_1_address = 0x70;
  uint_fast8_t display_2_address = 0x71;
  uint_fast8_t display_3_address = 0x72;

  while (display.begin(display_1_address, display_2_address, display_3_address) == false)
  {
    Serial.println("display failure!");
    delay(1000);
  }

  display_reset();

  display_brightness(preferences.getUChar(PREFERENCES_BRIGHTNESS, DEFAULT_BRIGHTNESS));
}

void display_reset()
{
  /*
   * chatGPT optimization:
   * memset(display_previous_buffer, ' ', RESOLUTION);
   */
  for (int index = 0; index < RESOLUTION; index++)
  {
    display_previous_buffer[index] = ' ';
  }

  /* blinking colon is tracked as the last element */
  display_previous_buffer[RESOLUTION + 1] = false;

  display.colonOff();
  display.decimalOff();
  display.setBlinkRate(0);
}

bool display_maybe_update(uint_fast8_t colon_display = 0, bool colon = false)
{
  bool updated = false;

  display.clear(false);

  for (int index = 0; index < RESOLUTION; index++)
  {
    char value = display_buffer[index];

    display.printChar(value, index);
    if (display_previous_buffer[index] != value)
    {
      display_previous_buffer[index] = value;
      updated = true;
    }
  }

  if (display_previous_buffer[RESOLUTION + 1] != colon)
  {
    display_previous_buffer[RESOLUTION + 1] = colon;

    display.setColonOnOff(colon_display, colon);
    updated = true;
  }
  else if (updated)
  {
    display.updateDisplay();
  }

  /*
  if (updated)
  {
    Serial.print("display update: ");
    Serial.print(std::string(display_buffer.begin(), display_buffer.end()).c_str());
    Serial.println();
  }
  */
 
  return updated;
}

void display_string(String string)
{
  for (int index = 0; index < RESOLUTION; index++)
  {
    if (index < string.length())
    {
      display_buffer[index] = string[index];
    }
    else
    {
      display_buffer[index] = ' ';
    }
  }
  display_maybe_update();
}

void display_value(uint_fast16_t value)
{
  char buffer[RESOLUTION + 1];
  snprintf(buffer, sizeof(buffer), "%u", value);
  display_string(buffer);
}

void display_brightness(uint8_t brightness)
{
  display.setBrightness(brightness);
}

void display_blink(uint8_t displayNumber, float rate)
{
  display.setBlinkRateSingle(displayNumber, rate);
}

char maybe_remove_leading_zeros(char character)
{
  if (!preferences.getUChar(PREFERENCES_LEADING_ZEROS, DEFAULT_LEADING_ZEROS))
  {
    if (character == '0')
    {
      character = ' ';
    }
  }

  return character;
}

char maybe_am_pm(uint_fast8_t hours)
{
  if (is_24_hour())
  {
    return ' ';
  }

  if (is_pm())
  {
    return 'P';
  }
  return 'A';
}

void display_date_components(uint_fast8_t months, uint_fast8_t date, uint_fast16_t years)
{
  char years_buffer[5];
  char months_buffer[3];
  char date_buffer[3];

  snprintf(years_buffer, sizeof(years_buffer), "%04d", years);
  snprintf(months_buffer, sizeof(months_buffer), "%02d", months);
  snprintf(date_buffer, sizeof(date_buffer), "%02d", date);

  display_buffer = {
      ' ',
      maybe_remove_leading_zeros(months_buffer[0]),
      months_buffer[1],
      'M',
      ' ',
      maybe_remove_leading_zeros(date_buffer[0]),
      date_buffer[1],
      'D',
      ' ',
      years_buffer[2],
      years_buffer[3],
      'Y'};
  display_maybe_update();
}

void display_time_components(uint_fast8_t hours, uint_fast8_t minutes, uint_fast8_t seconds)
{
  char hours_buffer[3];
  char minutes_buffer[3];
  char seconds_buffer[3];

  snprintf(hours_buffer, sizeof(hours_buffer), "%02d", hours);
  snprintf(minutes_buffer, sizeof(minutes_buffer), "%02d", minutes);
  snprintf(seconds_buffer, sizeof(seconds_buffer), "%02d", seconds);

  display_buffer = {
      ' ',
      maybe_remove_leading_zeros(hours_buffer[0]),
      hours_buffer[1],
      'H',
      ' ',
      maybe_remove_leading_zeros(minutes_buffer[0]),
      minutes_buffer[1],
      'M',
      ' ',
      maybe_remove_leading_zeros(seconds_buffer[0]),
      seconds_buffer[1],
      'S'};
  display_maybe_update();
}

void display_date(String date)
{
  date[3] = maybe_remove_leading_zeros(date[3]);

  if (date[3] == ' ')
  {
    display_buffer = {
        ' ',
        maybe_remove_leading_zeros(date[0]),
        date[1],
        date[2],
        date[4],
        date[5],
        date[6],
        date[7],
        date[8],
        date[9],
        ' ',
        ' '};
    display_maybe_update();
  }
  else
  {
    display_buffer = {
        ' ',
        maybe_remove_leading_zeros(date[0]),
        date[1],
        date[2],
        date[3],
        date[4],
        date[5],
        date[6],
        date[7],
        date[8],
        date[9],
        ' '};
    display_maybe_update();
  }
}

void display_time(String time, bool colon)
{
  display_buffer = {
      ' ',
      ' ',
      ' ',
      ' ',
      maybe_remove_leading_zeros(time[0]),
      time[1],
      time[3],
      time[4],
      ' ',
      maybe_am_pm(get_hours()),
      is_24_hour() ? ' ' : 'M',
      ' '};

  display_maybe_update(2, colon);
}

void display_date_time(String date, String time, bool colon)
{
  date[3] = maybe_remove_leading_zeros(date[3]);

  if (date[3] == ' ')
  {
    display_buffer = {
        maybe_remove_leading_zeros(time[0]),
        time[1],
        time[3],
        time[4],
        maybe_am_pm(get_hours()),
        is_24_hour() ? ' ' : 'M',
        ' ',
        ' ',
        maybe_remove_leading_zeros(date[0]),
        date[1],
        date[2],
        date[4]};
    display_maybe_update(1, colon);
  }
  else
  {
    display_buffer = {
        maybe_remove_leading_zeros(time[0]),
        time[1],
        time[3],
        time[4],
        maybe_am_pm(get_hours()),
        is_24_hour() ? ' ' : 'M',
        ' ',
        maybe_remove_leading_zeros(date[0]),
        date[1],
        date[2],
        date[3],
        date[4]};
    display_maybe_update(1, colon);
  }
}

void display_time_seconds(String time)
{
  display_buffer = {
      ' ',
      maybe_remove_leading_zeros(time[0]),
      time[1],
      'H',
      ' ',
      maybe_remove_leading_zeros(time[3]),
      time[4],
      'M',
      ' ',
      maybe_remove_leading_zeros(time[6]),
      time[7],
      'S'};
  display_maybe_update();
}

void display_clock()
{
  String date = get_date_string();
  String time = get_time_string();
  uint_fast8_t seconds = get_seconds();
  bool colon = (seconds % 2 == 0);

  switch (preferences.getUChar(PREFERENCES_DISPLAY_FORMAT, DEFAULT_DISPLAY_FORMAT))
  {
  case DISPLAY_AUTO:
    if (seconds % 10 == 0 || (seconds + 1) % 10 == 0)
    {
      display_date(date);
    }
    else
    {
      display_time(time, colon);
    }
    break;

  case DISPLAY_DATE_TIME:
    display_date_time(date, time, colon);
    break;

  case DISPLAY_DATE:
    display_date(date);
    break;

  case DISPLAY_TIME:
    display_time(time, colon);
    break;

  case DISPLAY_TIME_SECONDS:
    display_time_seconds(time);
    break;
  }
}

int days_in_month(int year, int month)
{
  int days;

  // plus one because tm struct starts at 0
  // and this came from the internet ;)
  switch (month + 1)
  {
  case 1:
  case 2:
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
    {
      days = 29;
    }
    else
    {
      days = 28;
    }
    break;
  case 3:
  case 5:
  case 7:
  case 8:
  case 10:
  case 12:
    days = 31;
    break;
  case 4:
  case 6:
  case 9:
  case 11:
    days = 30;
    break;
  }
  return days;
}

void display_birthday()
{
  char years_buffer[3];
  char days_buffer[4];
  char hours_buffer[3];

  uint_fast16_t birth_date = preferences.getUChar(PREFERENCES_BIRTH_DATE, DEFAULT_BIRTH_DATE),
                birth_month = preferences.getUChar(PREFERENCES_BIRTH_MONTH, DEFAULT_BIRTH_MONTH),
                birth_year = preferences.getUShort(PREFERENCES_BIRTH_YEAR, DEFAULT_BIRTH_YEAR),
                birth_hour = preferences.getUChar(PREFERENCES_BIRTH_HOUR, DEFAULT_BIRTH_HOUR),
                birth_minute = preferences.getUChar(PREFERENCES_BIRTH_MINUTE, DEFAULT_BIRTH_MINUTE),
                birth_second = preferences.getUChar(PREFERENCES_BIRTH_SECOND, DEFAULT_BIRTH_SECOND);
  int_fast16_t years = 0, days = 0, hours = 0;

  time_t now;
  struct tm birth_tm;
  struct tm now_tm;

  time(&now);              // Get current time
  gmtime_r(&now, &now_tm); // Convert time_t to tm struct

  // Set up birth_tm struct with provided birthdate
  birth_tm.tm_year = birth_year - 1900;
  birth_tm.tm_mon = birth_month - 1;
  birth_tm.tm_mday = birth_date;
  birth_tm.tm_hour = birth_hour;
  birth_tm.tm_min = birth_minute;
  birth_tm.tm_sec = birth_second;
  birth_tm.tm_isdst = -1;

  years = now_tm.tm_year - birth_tm.tm_year;

  bool birthday_already_happened = (now_tm.tm_mon > birth_tm.tm_mon || (now_tm.tm_mon == birth_tm.tm_mon && now_tm.tm_mday > birth_tm.tm_mday) || (now_tm.tm_mon == birth_tm.tm_mon && now_tm.tm_mday == birth_tm.tm_mday && now_tm.tm_hour >= birth_tm.tm_hour));

  if (!birthday_already_happened)
  {
    years--;
  }

  /*
  for (uint_fast8_t month = 0; month < 12; month++)
  {
    bool is_before_birth_month = month < birth_tm.tm_mon;
    bool is_current_month = month == now_tm.tm_mon;
    bool is_birth_month = month == birth_tm.tm_mon;

    if (birthday_already_happened && is_before_birth_month)
    {
      continue;
    }

    if (is_current_month)
    {
      days += now_tm.tm_mday - (is_birth_month ? birth_tm.tm_mday : 0);
    }
    else if (is_birth_month)
    {
      days += days_in_month(now_tm.tm_year, month) - birth_tm.tm_mday;
      if (!birthday_already_happened)
      {
        days += days_in_month(now_tm.tm_year - 1, month);
      }
    }
    else
    {
      int year = (birthday_already_happened || is_before_birth_month) ? now_tm.tm_year : now_tm.tm_year - 1;
      days += days_in_month(year, month);
    }
  }
  */
 
  for (uint_fast8_t month = 0; month < 12; month++)
  {
    if (birthday_already_happened && month < birth_tm.tm_mon)
    {
      // Your birthday already happened this year and this iteration
      // is for a month before your birthday month
      continue;
    }
    else if (birthday_already_happened && month >= birth_tm.tm_mon)
    {
      // Your birthday already happened this year and this iteration
      // is for your birthday month or one after that
      if (month == now_tm.tm_mon && month == birth_tm.tm_mon)
      {
        // it is currently your birthday month
        // but after your birthday day, add the difference
        days += now_tm.tm_mday - birth_tm.tm_mday;
      }
      else if (month == birth_tm.tm_mon)
      {
        // it is past your birthday month so add the remaining
        // days of your birthday month from this year
        days += days_in_month(now_tm.tm_year, month) - birth_tm.tm_mday;
      }
      else if (month < now_tm.tm_mon)
      {
        // add the number of days in a month that
        // ended this year after your birthday month
        days += days_in_month(now_tm.tm_year, month);
      }
      else if (month == now_tm.tm_mon)
      {
        // add the number of days in the current month
        days += now_tm.tm_mday;
      }
    }
    else if (!birthday_already_happened && month < birth_tm.tm_mon)
    {
      // Your birthday hasn't happened this year and this iteration
      // is for a month before your birthday month
      if (month < now_tm.tm_mon)
      {
        // add the number of days in a month that ended
        // this year leading up to your birthday month
        days += days_in_month(now_tm.tm_year, month);
      }
      else if (month == now_tm.tm_mon)
      {
        // add the number of days this month
        // leading up to your birthday month
        days += now_tm.tm_mday;
      }
    }
    else if (!birthday_already_happened && month >= birth_tm.tm_mon)
    {
      // Your birthday hasn't happened this year and this iteration
      // is for a month after your birthday month
      if (month == now_tm.tm_mon)
      {
        // it must be your birthday month but before your
        // birthday day (or hour), so add in the days so far
        days += now_tm.tm_mday;
      }

      if (month == birth_tm.tm_mon)
      {
        // add the remaining days of your birthday month from last year
        days += days_in_month(now_tm.tm_year - 1, month) - birth_tm.tm_mday;
      }
      else
      {
        // add the days in a month after your birthday but
        // before it has happened this year from last year
        days += days_in_month(now_tm.tm_year - 1, month);
      }
    }
  }

  hours = now_tm.tm_hour - birth_tm.tm_hour;
  if (hours < 0)
  {
    days--;
    hours += 24;
  }

  snprintf(years_buffer, sizeof(years_buffer), "%02d", years);
  snprintf(days_buffer, sizeof(days_buffer), "%03d", days);
  snprintf(hours_buffer, sizeof(hours_buffer), "%02d", hours);

  display_buffer = {
      maybe_remove_leading_zeros(years_buffer[0]),
      years_buffer[1],
      'Y',
      ' ',
      maybe_remove_leading_zeros(days_buffer[0]),
      maybe_remove_leading_zeros(days_buffer[1]),
      days_buffer[2],
      'D',
      ' ',
      maybe_remove_leading_zeros(hours_buffer[0]),
      hours_buffer[1],
      'H'};
  display_maybe_update();
}