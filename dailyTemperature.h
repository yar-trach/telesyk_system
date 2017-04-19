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

//#ifndef http
//HTTPClient http;
//#endif

class dailyTemperature {
  private:
  byte tempCurrent = 0;
  byte tempMorning = 0;
  byte tempDay = 0;
  byte tempEvening = 0;
  byte tempNight = 0;

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
  dailyTemperature(char RED_LED_PIN, char GREEN_LED_PIN, char BLUE_LED_PIN) {
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(BLUE_LED_PIN, OUTPUT);

    _RED_LED_PIN = RED_LED_PIN;
    _GREEN_LED_PIN = GREEN_LED_PIN;
    _BLUE_LED_PIN = BLUE_LED_PIN;
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
  void getWeatherCurrentCondition() {
    Serial.println("\nCURRENT WEATHER CONDITION (every 10 minutes)");

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
        return;
      }
      tempCurrent = root["main"]["temp"];
      currentHumidity = root["main"]["humidity"];
      currentWeather = String((const char*)root["weather"][0]["main"]);

      changeColor();
    }
    http.end();
  }
  
  /**
   * GETTING DAILY WEATHER CONDITIONS
   */
  void getWeatherDailyCondition(byte time) {
    Serial.println("\nDAILY WEATHER CONDITIONS (every 3 hours)");

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
        return;
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
  
      Serial.println("\ntempMorning:" + String(tempMorning) + "\ntempDay:" + String(tempDay) + "\ntempEvening:" + String(tempEvening) + "\ntempNight:" + String(tempNight) + "\nweatherDescription:" + weatherDescription);
    }
    http.end();
  }
};

