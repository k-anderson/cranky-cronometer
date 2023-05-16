#include "main.h"
#include "clock.h"
#include <SparkFun_RV8803.h>
#include <Timezone.h>

RV8803 rtc;

// Enter your local time zone offset in hours from UTC, and daylight saving rules
TimeChangeRule myDST = {"EDT", Second, Sun, Mar, 2, -240}; // Eastern Daylight Time
TimeChangeRule mySTD = {"EST", First, Sun, Nov, 2, -300};  // Eastern Standard Time
Timezone myTZ(myDST, mySTD);

const int timeZoneOffset = -5 * 3600; // Modify this according to your timezone (in seconds). This example is for EST (-5 hours).
const int dstOffset = 3600;           // Daylight saving time offset (in seconds), usually 1 hour (3600 seconds)

void setup_clock()
{
  set_timezone(preferences.getString(PREFERENCES_TIMEZONE, DEFAULT_TIMEZONE));

  while (rtc.begin() == false)
  {
    Serial.println("rtc failure!");
    delay(1000);
  }

  rtc.set24Hour();

  load_from_rtc();
}

void save_to_rtc()
{
  time_t localTime = time(NULL);
  time_t utcTime = myTZ.toUTC(localTime);

  Serial.print("current local time: ");
  Serial.println(asctime(localtime(&localTime)));
  Serial.print("set rtc utc time: ");
  Serial.println(asctime(localtime(&utcTime)));

  struct tm *time = localtime(&utcTime);
  rtc.setYear(time->tm_year + 1900);
  rtc.setMonth(time->tm_mon + 1);
  rtc.setDate(time->tm_mday);
  rtc.setHours(time->tm_hour);
  rtc.setMinutes(time->tm_min);
  rtc.setSeconds(time->tm_sec);
}

void load_from_rtc()
{
  while (!rtc.updateTime())
  {
    delay(1000);
  }

  struct tm rtcTime;
  rtcTime.tm_year = rtc.getYear() - 1900; // tm struct starts at 1900
  rtcTime.tm_mon = rtc.getMonth() - 1;    // tm struct uses 0 as January
  rtcTime.tm_mday = rtc.getDate();
  rtcTime.tm_hour = rtc.getHours();
  rtcTime.tm_min = rtc.getMinutes();
  rtcTime.tm_sec = rtc.getSeconds();
  // rtcTime.tm_wday = rtc.getWeekday() - 1; // RV-8803 uses 1 as Sunday, while tm struct uses 0 as Sunday
  rtcTime.tm_isdst = -1;

  time_t utcTime = mktime(&rtcTime);
  time_t localTime = myTZ.toLocal(utcTime);
  Serial.print("read rtc utc time: ");
  Serial.println(asctime(localtime(&utcTime)));
  Serial.print("current local time: ");
  Serial.println(asctime(localtime(&localTime)));

  timeval tv = {localTime, 0};
  settimeofday(&tv, nullptr);
}

void set_time(struct tm *time)
{
  struct timeval now = {.tv_sec = mktime(time)};

  settimeofday(&now, NULL);

  save_to_rtc();
}

uint_fast8_t get_month()
{
  time_t current_time = time(NULL);
  return localtime(&current_time)->tm_mon + 1;
}

void set_month(uint_fast8_t month)
{
  time_t current_time = time(NULL);

  struct tm *time = localtime(&current_time);
  time->tm_mon = month - 1;

  set_time(time);
}

uint_fast8_t get_date()
{
  time_t current_time = time(NULL);
  return localtime(&current_time)->tm_mday;
}

void set_date(uint_fast8_t date)
{
  time_t current_time = time(NULL);

  struct tm *time = localtime(&current_time);
  time->tm_mday = date;

  set_time(time);
}

uint_fast16_t get_year()
{
  time_t current_time = time(NULL);

  return localtime(&current_time)->tm_year + 1900;
}

void set_year(uint_fast8_t year)
{
  time_t current_time = time(NULL);

  struct tm *time = localtime(&current_time);
  time->tm_year = year - 1900;

  set_time(time);
}

uint_fast8_t get_hours()
{
  time_t current_time = time(NULL);
  return localtime(&current_time)->tm_hour;
}

void set_hours(uint_fast8_t hours)
{
  time_t current_time = time(NULL);

  struct tm *time = localtime(&current_time);
  time->tm_hour = hours;

  set_time(time);
}

uint_fast8_t get_minutes()
{
  time_t current_time = time(NULL);
  return localtime(&current_time)->tm_min;
}

void set_minutes(uint_fast8_t minutes)
{
  time_t current_time = time(NULL);

  struct tm *time = localtime(&current_time);
  time->tm_min = minutes;

  set_time(time);
}

uint_fast8_t get_seconds()
{
  time_t current_time = time(NULL);
  return localtime(&current_time)->tm_sec;
}

void set_seconds(uint_fast8_t seconds)
{
  time_t current_time = time(NULL);

  struct tm *time = localtime(&current_time);
  time->tm_sec = seconds;

  set_time(time);
}

String get_date_string()
{
  static char buffer[11];
  time_t current_time = time(NULL);
  struct tm *time_tm = localtime(&current_time);

  snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d", time_tm->tm_mon + 1, time_tm->tm_mday, time_tm->tm_year + 1900);
  return buffer;
}

String get_time_string()
{
  static char buffer[11];
  time_t current_time = time(NULL);
  struct tm *time_tm = localtime(&current_time);

  if (is_24_hour())
  {
    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", time_tm->tm_hour, time_tm->tm_min, time_tm->tm_sec);
  }
  else
  {
    char half = 'A';
    uint8_t hour_correction = 0;
    if (time_tm->tm_hour >= 12)
    {
      half = 'P';
      if (time_tm->tm_hour > 12)
      {
        hour_correction = 12;
      }
    }
    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d:%02d%cM", time_tm->tm_hour - hour_correction, time_tm->tm_min, time_tm->tm_sec, half);
  }

  return buffer;
}

void set_timezone(String zone)
{
  Serial.printf("setting timezone to %s\n", zone.c_str());
  /*
  timezone.setLocation(zone);
  */
}

bool is_24_hour()
{
  return preferences.getBool(PREFERENCES_24HOUR, DEFAULT_24HOUR);
}

bool is_pm()
{
  if (is_24_hour())
  {
    return false;
  }
  else
  {
    return get_hours() >= 12;
  }
}

void initTime(String timezone)
{
  struct tm timeinfo;

  Serial.println("Setting up time");
  configTime(0, 0, "pool.ntp.org"); // First connect to NTP server, with 0 TZ offset
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("  Failed to obtain time");
    return;
  }
  Serial.println("  Got the time from NTP");
  // Now we can set the real timezone
  set_timezone(timezone);
}