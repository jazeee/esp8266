#include <time.h>

// Redwood City
#define LATITUDE        37.4852
#define LONGITUDE       122.2364
#define PST_OFFSET      -8
#define PDT_OFFSET      -7

// Standard time only: (In other words, subtracted 60 minutes for Summer
const double winterSunSetInMinutesFromMidnight = 1014;
const double summerSunSetInMinutesFromMidnight = 1174;
const double deltaSunSetInMinutes = summerSunSetInMinutesFromMidnight - winterSunSetInMinutesFromMidnight;
const double averageSunSetInMinutesFromMidnight = (summerSunSetInMinutesFromMidnight + winterSunSetInMinutesFromMidnight) / 2.0;

double getSunSetMinutesFromMidnight() {
  time_t now = time(nullptr);
  struct tm* tmInfo = localtime(&now);
  // See https://arduinodiy.wordpress.com/2017/03/07/calculating-sunrise-and-sunset-on-arduino-or-other-microcontroller/
  // PST
  if (tmInfo -> tm_year < 100) {
  	// 1900's means we don't have a valid date.
  	return averageSunSetInMinutesFromMidnight;
  }
  double sunSetInMinutesFromMidnight = averageSunSetInMinutesFromMidnight - (deltaSunSetInMinutes / 2.0) * cos((tmInfo -> tm_yday + 8)/58.09);
  return sunSetInMinutesFromMidnight;
}

double getSunSetMinutesFromNow() {
  time_t now = time(nullptr);
  struct tm* tmInfo = localtime(&now);
  double minutesFromMidnight = (tmInfo -> tm_hour * 60.0 + tmInfo -> tm_min) + (tmInfo -> tm_sec / 60.0);
  double sunSetMinutesFromNow = getSunSetMinutesFromMidnight() - minutesFromMidnight;
  return sunSetMinutesFromNow;
}
