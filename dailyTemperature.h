#ifndef DAILYTEMPERATURE_H
#define DAILYTEMPERATURE_H
#include "ledrgb.h";
#include <SD.h>

class DAILYTEMPERATURE {
  public:
  DAILYTEMPERATURE(LEDRGB indicator, File weatherFile);
  boolean getWeatherCurrentCondition();
  boolean getWeatherDailyCondition(byte time);

  private:
  int8_t tempCurrent = 0;
  int8_t tempMorning = 0;
  int8_t tempDay = 0;
  int8_t tempEvening = 0;
  int8_t tempNight = 0;

  String currentWeather;
  String weatherDescription;
  byte currentHumidity;
  
  LEDRGB indicator;
  File weatherFile;
  void pushDataToFile(byte time);
};

#endif
