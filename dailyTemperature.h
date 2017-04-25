/**
 *  Color scheme
    ╔════╦══════════════════╦═════════════╗
    ║    ║    RGB Color     ║ Temperature ║
    ╠════╬══════════════════╬═════════════╣
    ║  1 ║ rgb(255, 0, 255) ║ -273 to -29 ║
    ║  2 ║ rgb(128, 0, 255) ║ -28 to -21  ║
    ║  3 ║ rgb(0, 0, 255)   ║ -20 to 0    ║
    ║  4 ║ rgb(0, 128, 255) ║ 0 to 7      ║
    ║  5 ║ rgb(0, 255, 255) ║ 8 to 15     ║
    ║  6 ║ rgb(0, 255, 128) ║ 16 to 20    ║
    ║  7 ║ rgb(0, 255, 0)   ║ 21 to 25    ║
    ║  8 ║ rgb(255, 255, 0) ║ 26 to 32    ║
    ║  9 ║ rgb(255, 128, 0) ║ 33 to 37    ║
    ║ 10 ║ rgb(255, 64, 0)  ║ 38 to 54    ║
    ║ 11 ║ rgb(255, 0, 0)   ║ > 54        ║
    ╚════╩══════════════════╩═════════════╝
 */

const char* openWeatherMapApiAddr = "http://api.openweathermap.org/data/2.5";
const char* cityid = "702550"; // Lviv id for weather API
//const char* openWeatherMapApiId = "................................"; // weather API key
// OR FILE WITH SAME TOKENS:
#include "openWeatherMapApiToken.h";

#ifndef __SD_H__
#define __SD_H__
#endif

class dailyTemperature {
  private:
  int8_t tempCurrent = 0;
  int8_t tempMorning = 0;
  int8_t tempDay = 0;
  int8_t tempEvening = 0;
  int8_t tempNight = 0;

  File weatherFile;
  //weatherFile = SD.open("test.txt", FILE_WRITE);

  String currentWeather;
  String weatherDescription;
  byte currentHumidity;
  
  char _RED_LED_PIN;
  char _GREEN_LED_PIN;
  char _BLUE_LED_PIN;

  /**
   * SETTING COLOR OF LED INDICATOR
   */
  void changeColor() {
    unsigned char red;
    unsigned char green;
    unsigned char blue;

    byte tempt = tempCurrent;
    
    if (tempt < -29) {
      tempt = -29;
    } else if (tempt > 54) {
      tempt = 54;
    }
    
    if (tempt <= 0) {
      red = map(tempt, 0, -29, 0, 255);
      green = 0;
      blue = 255;
    } else if (tempt > 0 && tempt <= 25) {
      red = 0;
      green = 255;
      blue = map(tempt, 1, 25, 255, 1);
    } else {
      red = 255;
      green = map(tempt, 26, 54, 255, 0);
      blue = 0;
    }

    Serial.print("red>>");
    Serial.print(red);

    Serial.print("  green>>");
    Serial.print(green);

    Serial.print("  blue>>");
    Serial.println(blue);
    
    analogWrite(_RED_LED_PIN, red);
    analogWrite(_GREEN_LED_PIN, green);
    analogWrite(_BLUE_LED_PIN, blue);
  }

  public:
  dailyTemperature(char RED_LED_PIN, char GREEN_LED_PIN, char BLUE_LED_PIN, File weatherFile) {
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(BLUE_LED_PIN, OUTPUT);

    _RED_LED_PIN = RED_LED_PIN;
    _GREEN_LED_PIN = GREEN_LED_PIN;
    _BLUE_LED_PIN = BLUE_LED_PIN;

    weatherFile = weatherFile;
  }

  String getCurrentTemp() {
    return String(tempCurrent > 0 ? "+" : "-") + String(tempCurrent < 10 ? "0" : "") + String(tempCurrent) + "C" + String((char)223);
  }

  String getCurrentHumidity() {
    return "Hum" + String(currentHumidity) + "%";
  }

  String getCurrentWeather() {
    return currentWeather;
  }

  String getMorDayTemp() {
    return "MOR" + String(tempMorning > 0 ? "+" : "-") + String(tempMorning < 10 ? "0" : "") + String(tempMorning) + "C" + String((char)223)
           + " DAY" + String(tempDay > 0 ? "+" : "-") + String(tempDay < 10 ? "0" : "") + String(tempDay) + "C" + String((char)223);
  }

  String getEveNigTemp() {
    return "EVE" + String(tempEvening > 0 ? "+" : "-") + String(tempEvening < 10 ? "0" : "") + String(tempEvening) + "C" + String((char)223)
           + " NIG" + String(tempNight > 0 ? "+" : "-") + String(tempNight < 10 ? "0" : "") + String(tempNight) + "C" + String((char)223);
  }

  String getWeatherDescription() {
    return weatherDescription;
  }

  /**
   * GETTINT CURRENT WEATHER CONDITION
   */
  boolean getWeatherCurrentCondition() {
    Serial.println("\nCURRENT WEATHER CONDITION (every 10 minutes)");
    Serial.println(millis());
    boolean stat = 0;

    char req[125];
    strcpy(req, openWeatherMapApiAddr);
    strcat(req, "/weather?id=");
    strcat(req, cityid);
    strcat(req, "&mode=json&units=metric&appid=");
    strcat(req, openWeatherMapApiId);

    Serial.println(millis());
    http.begin(req);
    Serial.println(millis());
    int httpCode = 0;
    while(!httpCode) {
      Serial.println(millis());
      httpCode = http.GET();
    }
    Serial.println(millis());
    if (httpCode > 0 && httpCode == HTTP_CODE_OK) {
      Serial.println(millis());
      String response = http.getString();
      Serial.println(millis());
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

      changeColor();
      stat = 1;
    }
    http.end();
    Serial.println(millis());
    return stat;
  }
  
  /**
   * GETTING DAILY WEATHER CONDITIONS
   */
  boolean getWeatherDailyCondition(byte time) {
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

      if ((time >= 3) && (time < 12)) {
        tempMorning = root["list"][0]["temp"]["morn"];
        tempDay = root["list"][0]["temp"]["day"];
        tempEvening = root["list"][0]["temp"]["eve"];
        tempNight = root["list"][0]["temp"]["night"];
        Serial.println("Morning");
      } else if ((time >= 3) && (time < 19)) {
        tempDay = root["list"][0]["temp"]["day"];
        tempEvening = root["list"][0]["temp"]["eve"];
        tempNight = root["list"][0]["temp"]["night"];
        Serial.println("Day");
      } else if ((time >= 3) && (time < 23)) {
        tempEvening = root["list"][0]["temp"]["eve"];
        tempNight = root["list"][0]["temp"]["night"];
        Serial.println("Evening");
      } else {
        tempNight = root["list"][0]["temp"]["night"];
        Serial.println("Night");
      }

      weatherDescription = String((const char*)root["list"][0]["weather"][0]["description"]);
      stat = 1;



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
      weatherFile = SD.open("weather.txt", FILE_WRITE);
      if (weatherFile) {
        Serial.print("Writing to weather.txt...");
        weatherFile.println(buffer);
        // close the file:
        weatherFile.close();
        Serial.println("done.");
      } else {
        // if the file didn't open, print an error:
        Serial.println("error opening test.txt");
      } // END OF If success, add weather line to fine
    
      // re-open the file for reading:
      weatherFile = SD.open("weather.txt");
      if (weatherFile) {
        Serial.println("weather.txt:");
    
        // read from the file until there's nothing else in it:
        while (weatherFile.available()) {
          Serial.write(weatherFile.read());
        }
        // close the file:
        weatherFile.close();
      } else {
        // if the file didn't open, print an error:
        Serial.println("error opening weather.txt");
      }
  
      Serial.println("\ntempMorning:" + String(tempMorning) + "\ntempDay:" + String(tempDay) + "\ntempEvening:" + String(tempEvening) + "\ntempNight:" + String(tempNight) + "\nweatherDescription:" + weatherDescription);
    }
    http.end();
    return stat;
  }
};

