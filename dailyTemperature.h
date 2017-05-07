#ifndef DAILYTEMPERATURE_H
#define DAILYTEMPERATURE_H
#include "ledrgb.h";
//#include <SD.h>

class DAILYTEMPERATURE {
  public:
  DAILYTEMPERATURE(LEDRGB indicator, File weatherFile, LiquidCrystal_I2C lcd);
  boolean getWeatherCurrentCondition();
  boolean getWeatherDailyCondition(byte time);
  void showWeatherInfo(byte slide);

  private:
  int8_t tempCurrent = 0;
  int8_t tempMorning = 0;
  int8_t tempDay = 0;
  int8_t tempEvening = 0;
  int8_t tempNight = 0;

  String currentWeather;
  String weatherDescription;
  byte currentHumidity;
  
  LEDRGB _indicator;
  File _weatherFile;
  LiquidCrystal_I2C _lcd;
  
  void pushDataToFile(byte time);
};

#endif
