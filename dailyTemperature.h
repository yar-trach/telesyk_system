//const char* openWeatherMapApiAddr = "http://api.openweathermap.org/data/2.5";
//const char* openWeatherMapApiId = "................................"; // weather API key
//const char* cityid = "......"; // Lviv id for weather API
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

  public:
  dailyTemperature() {
    
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
    if (httpCode > 0) {
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
    if (httpCode > 0) {
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

