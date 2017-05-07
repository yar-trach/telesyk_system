const char* openWeatherMapApiAddr = "http://api.openweathermap.org/data/2.5";
const char* cityid = "702550"; // Lviv id for weather API
//const char* openWeatherMapApiId = "................................"; // weather API key
// OR FILE WITH SAME TOKENS:
#include "openWeatherMapApiToken.h";

#include <SD.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include "ledrgb.h";
#include "dailyTemperature.h";
#include <ESP8266HTTPClient.h>
HTTPClient http;

DAILYTEMPERATURE::DAILYTEMPERATURE(LEDRGB indicator, File weatherFile)
:
  _indicator(indicator),
  _weatherFile(weatherFile)
{
}

void DAILYTEMPERATURE::showWeatherInfo(byte slide, LiquidCrystal_I2C &_lcd) {
  switch (slide) {
    case 0:
      _lcd.setCursor(9, 0);
      _lcd.print(String(tempCurrent > 0 ? "+" : "-") + String(tempCurrent < 10 ? "0" : "") + String(tempCurrent) + "C" + String((char)223) + "  ");
      
      _lcd.setCursor(0, 1);
      _lcd.print("MOR" + String(tempMorning > 0 ? "+" : "-") + String(tempMorning < 10 ? "0" : "") + String(tempMorning) + "C" + String((char)223)   
              + " DAY" + String(tempDay > 0 ? "+" : "-") + String(tempDay < 10 ? "0" : "") + String(tempDay) + "C" + String((char)223));
    break;
    case 1:
      _lcd.setCursor(9, 0);
      _lcd.print("Hum" + String(currentHumidity) + "%" + " ");
      
      _lcd.setCursor(0, 1);
      _lcd.print("EVE" + String(tempEvening > 0 ? "+" : "-") + String(tempEvening < 10 ? "0" : "") + String(tempEvening) + "C" + String((char)223)   
              + " NIG" + String(tempNight > 0 ? "+" : "-") + String(tempNight < 10 ? "0" : "") + String(tempNight) + "C" + String((char)223));
    break;
    case 2:
      _lcd.setCursor(9, 0);
      _lcd.print(currentWeather);
      
      _lcd.setCursor(0, 1);
      _lcd.print(weatherDescription + "         ");
    break;
  }
}

void DAILYTEMPERATURE::pushDataToFile(byte time) {
  StaticJsonBuffer<200> outputJsonBuffer;
  JsonObject& rootOutput = outputJsonBuffer.createObject();
  rootOutput["hour"] = time;
  rootOutput["mo"] = tempMorning;
  rootOutput["da"] = tempDay;
  rootOutput["ev"] = tempEvening;
  rootOutput["ni"] = tempNight;
  
  char buffer[200];
  rootOutput.printTo(buffer, sizeof(buffer));
  
  // If success, add weather line to fine
  _weatherFile = SD.open("weather.txt", FILE_WRITE);
  if (_weatherFile) {
    Serial.print("Writing to weather.txt...");
    _weatherFile.println(buffer);
    // close the file:
    _weatherFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening weather.txt");
  } // END OF If success, add weather line to fine
  
  // re-open the file for reading:
  _weatherFile = SD.open("weather.txt");
  if (_weatherFile) {
    Serial.println("weather.txt:");
  
    // read from the file until there's nothing else in it:
    while (_weatherFile.available()) {
      Serial.write(_weatherFile.read());
    }
    // close the file:
    _weatherFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening weather.txt");
  }
}

boolean DAILYTEMPERATURE::getWeatherCurrentCondition() {
  Serial.println("\nCURRENT WEATHER CONDITION (every 10 minutes)");
  //led busy on
  boolean stat = 0;

  char req[125];
  strcpy(req, openWeatherMapApiAddr);
  strcat(req, "/weather?id=");
  strcat(req, cityid);
  strcat(req, "&mode=json&units=metric&appid=");
  strcat(req, openWeatherMapApiId);

  http.begin(req);
  int httpCode = http.GET();
  if (httpCode > 0 && httpCode == HTTP_CODE_OK) {
    String response = http.getString();
    Serial.println(response);
    
    StaticJsonBuffer<2000> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(response);

    if (!root.success()) {
      Serial.println("parseObject() failed");
      return 0;
    }
    tempCurrent = root["main"]["temp"];
    currentHumidity = root["main"]["humidity"];
    currentWeather = String((const char*)root["weather"][0]["main"]);

    _indicator.changeColor(tempCurrent);
    stat = 1;
  }
  http.end();
  //led busy off
  return stat;
}

/**
 * GETTING DAILY WEATHER CONDITIONS
 */
boolean DAILYTEMPERATURE::getWeatherDailyCondition(byte time) {
  Serial.println("\nDAILY WEATHER CONDITIONS (every 3 hours)");

  boolean stat = 0;

  char req[125];
  strcpy(req, openWeatherMapApiAddr);
  strcat(req, "/forecast/daily?id=");
  strcat(req, cityid);
  strcat(req, "&mode=json&units=metric&cnt=1&appid=");
  strcat(req, openWeatherMapApiId);

  http.begin(req);
  int httpCode = http.GET();
  if (httpCode > 0 && httpCode == HTTP_CODE_OK) {
    String response = http.getString();
    Serial.println(response);
    
    StaticJsonBuffer<2000> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(response);

    if (!root.success()) {
      Serial.println("parseObject() failed");
      return 0;
    }

//      if ((time >= 3) && (time < 12)) {
      tempMorning = root["list"][0]["temp"]["morn"];
      tempDay = root["list"][0]["temp"]["day"];
      tempEvening = root["list"][0]["temp"]["eve"];
      tempNight = root["list"][0]["temp"]["night"];
//        Serial.println("Morning");
//      } else if ((time >= 3) && (time < 19)) {
//        tempDay = root["list"][0]["temp"]["day"];
//        tempEvening = root["list"][0]["temp"]["eve"];
//        tempNight = root["list"][0]["temp"]["night"];
//        Serial.println("Day");
//      } else if ((time >= 3) && (time < 23)) {
//        tempEvening = root["list"][0]["temp"]["eve"];
//        tempNight = root["list"][0]["temp"]["night"];
//        Serial.println("Evening");
//      } else {
//        tempNight = root["list"][0]["temp"]["night"];
//        Serial.println("Night");
//      }

    weatherDescription = String((const char*)root["list"][0]["weather"][0]["description"]);
    stat = 1;
    
    pushDataToFile(time);

    Serial.println("\ntempMorning:" + String(tempMorning) + "\ntempDay:" + String(tempDay) + "\ntempEvening:" + String(tempEvening) + "\ntempNight:" + String(tempNight) + "\nweatherDescription:" + weatherDescription);
  }
  http.end();
  return stat;
}



