#include "currentTime.h";

const char* timeZoneApi = "http://api.timezonedb.com/v2";
const char* zone = "Europe/Kiev"; // Kyiv title for time zone API
//const char* timeZoneApiId = "............"; // time zone API key
// OR FILE WITH SAME TOKENS:
#include "timeZoneApiToken.h";
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
HTTPClient httpCliCurrentTime;

CURRENTTIME::CURRENTTIME(void){}

int CURRENTTIME::getLocalTime(void) {
  Serial.println("\nGETTING LOCAL TIME (timestamp): ");

  int returnTime = 0;
  
  char req[100];
  strcpy(req, timeZoneApi);
  strcat(req, "/get-time-zone?key=");
  strcat(req, timeZoneApiId);
  strcat(req, "&format=json&by=zone&zone=");
  strcat(req, zone);

  httpCliCurrentTime.begin(req);
  int httpCode = httpCliCurrentTime.GET();
  if (httpCode > 0 && httpCode == HTTP_CODE_OK) {
    String response = httpCliCurrentTime.getString();
    Serial.println(response);

    StaticJsonBuffer<2000> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(response);

    if (!root.success()) {
      Serial.println("parseObject() failed");
      return 0;
    }

    returnTime = root["timestamp"];
  }
  httpCliCurrentTime.end();
  return returnTime;
}
