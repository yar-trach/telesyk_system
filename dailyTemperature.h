#ifndef DAILYTEMPERATURE_H
#define DAILYTEMPERATURE_H
#include "ledrgb.h";

class DAILYTEMPERATURE {
  public:
  DAILYTEMPERATURE(LEDRGB indicator, File weatherFile);
  boolean getWeatherCurrentCondition();
  boolean getWeatherDailyCondition(byte time);
  void showWeatherInfo(byte slide, LiquidCrystal_I2C &_lcd);

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
  
  void pushDataToFile(byte time);
};

#endif
