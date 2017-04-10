/**
 * File with id and password to access to wifi point
#define SSID_PASS 4 // size of ssid and pass

const char* ssidpass[SSID_PASS] = {
  "...", // ssid name of first point
  "...", // password of first point
  
  "...", // ssid name of second point
  "..." // password of second point
};
 */
#include "ssidpassword.h"

#include <RtcDateTime.h>
#include <RtcUtility.h>
#include <RtcDS3231.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <ArduinoJson.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>


HTTPClient http;
#include "dailyTemperature.h"
#include "currentTime.h"



unsigned long clockGen = 0;
unsigned long timeWeatherCurrentReq = 0;
unsigned long timeSliderUpd = 0;
unsigned long timeClockUpd = 0;

#define ledPin 2

#define sliderInterval 2000 // 2 seconds
#define clockInterval 1000 // 1 second
#define weatherCurrentReqInterval 600000 // 10 minutes

WiFiServer server(80);

//String slideBottom1;
//String slideBottom2;
//String slideBottom3;

String slideTopRight1;
String slideTopRight2;
String slideTopRight3;

int slide = 0;

String* test;

boolean alarm = false;

LiquidCrystal_I2C lcd(0x27, 16, 2);

dailyTemperature dailyTempObj = dailyTemperature();
currentTime currentTimeObj = currentTime();

RtcDS3231<TwoWire> rtcObject(Wire);

void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println("\nBooting");

  // Wifi LED   
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
    
  // LCD
  lcd.init();                     
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("CONNECTING");

  // Connect to WiFi network
  int n = WiFi.scanNetworks();
  Serial.println("scan done");

  if (n == 0) {
    Serial.println("No networks found");
  } else {
    for (int i = 0; i < n; i++) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");

      for (int i = 0; i < SSID_PASS; i += 2) {
        if (WiFi.SSID(i) == ssidpass[i]) {
          if (WiFi.status() != WL_CONNECTED) {
            WiFi.begin(ssidpass[i], ssidpass[i+1]);
          }
        }
      }
    }
  }

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  lcd.setCursor(0, 0);
  lcd.print("WIFI CONNECTED  ");
  Serial.println("\nWiFi connected");

  // Start i2c
  Wire.begin();
  
  // Start the server
  server.begin();
  
  Serial.print("Server started at: ");
  Serial.println(WiFi.localIP());

  
  
  dailyTempObj.getWeatherCurrentCondition();
  slideTopRight1 = dailyTempObj.getCurrentTemp();
  slideTopRight2 = dailyTempObj.getCurrentHumidity();
  slideTopRight3 = dailyTempObj.getCurrentWeather();



  
  
  
  rtcObject.Begin();

  RtcDateTime compiledDateTime(__DATE__, __TIME__);
  Serial.print("compiledDateTime>>>");
  Serial.println(compiledDateTime);

  if(!rtcObject.IsDateTimeValid()) {
    RtcDateTime loadingTime(currentTimeObj.getLocalTime());
    Serial.print("loadingTime>>>");
    Serial.println(loadingTime);

    if (loadingTime > 0) {
      Serial.println("Invalid time\nSetting time from Internet");
      rtcObject.SetDateTime(loadingTime - TIME_DIFF_2000);
    } else {
      Serial.println("Invalid time\nSetting compiled time");
      rtcObject.SetDateTime(compiledDateTime - TIME_DIFF_2000);
    }
  } else {
    RtcDateTime loadingTime(currentTimeObj.getLocalTime());
    Serial.print("loadingTime >>>");
    Serial.println(loadingTime);

    RtcDateTime currentTime = rtcObject.GetDateTime();
    Serial.print("currentTime>>>");
    Serial.println(currentTime);

    Serial.print("CHECKING>>");
    Serial.println(abs((loadingTime - TIME_DIFF_2000) - currentTime) > 100);
    
    if ((loadingTime > 0) && (abs((loadingTime - TIME_DIFF_2000) - currentTime) > 100)) {
      Serial.println("Setting up timepoint from Internet");
      rtcObject.SetDateTime(loadingTime - TIME_DIFF_2000);
    } else {
      Serial.println("Time is fine!");
    }
  }

//  int loadingTime = currentTimeObj.getLocalTime();
//  rtcObject.SetDateTime(loadingTime - TIME_DIFF_2000);
  

  RtcDateTime currentTime = rtcObject.GetDateTime();
  dailyTempObj.getWeatherDailyCondition(currentTime.Hour());

    test = dailyTempObj.getDailyWeather();
  for (int i = 0; i < sizeof(test); i++) {
    Serial.println(String(test[i]));
  }
}











void loop() {
  RtcDateTime currentTime = rtcObject.GetDateTime();
  char str[15];

  sprintf(str, "%d/%d/%d %d:%d:%d", currentTime.Year(), currentTime.Month(), currentTime.Day(), currentTime.Hour(), currentTime.Minute(), currentTime.Second());

  clockGen = millis();

  // Show time (every second)
  if (clockGen - timeClockUpd >= clockInterval) {
    timeClockUpd = clockGen;

    lcd.setCursor(0, 0);
    updateTime(currentTime);
//    Serial.println(str);

    // should be rewriten by using interrupt with SQW pin (using 6-pin DS3231) and alarms    
    if ((currentTime.Hour() == 3 && currentTime.Minute() == 0 && currentTime.Second() == 0)
     || (currentTime.Hour() == 6 && currentTime.Minute() == 0 && currentTime.Second() == 0)
     || (currentTime.Hour() == 9 && currentTime.Minute() == 0 && currentTime.Second() == 0)
     || (currentTime.Hour() == 12 && currentTime.Minute() == 0 && currentTime.Second() == 0)
     || (currentTime.Hour() == 15 && currentTime.Minute() == 0 && currentTime.Second() == 0)
     || (currentTime.Hour() == 18 && currentTime.Minute() == 0 && currentTime.Second() == 0)
     || (currentTime.Hour() == 21 && currentTime.Minute() == 0 && currentTime.Second() == 0)
     || (currentTime.Hour() == 0 && currentTime.Minute() == 0 && currentTime.Second() == 0)
     || (currentTime.Hour() == 12 && currentTime.Minute() == 2 && currentTime.Second() == 0)
     || (currentTime.Hour() == 12 && currentTime.Minute() == 4 && currentTime.Second() == 0)){
      Serial.println("<<<>>>");
      alarmT(currentTime.Hour());
    }
  }

  // Update slider info (every two seconds)
  if (clockGen - timeSliderUpd >= sliderInterval) {
    timeSliderUpd = clockGen;

    updateSlider();
  }

  // Updating current weather (every 10 minutes)
  if (clockGen - timeWeatherCurrentReq >= weatherCurrentReqInterval) {
    timeWeatherCurrentReq = clockGen;

    // update current weather condition
    dailyTempObj.getWeatherCurrentCondition();
    slideTopRight1 = dailyTempObj.getCurrentTemp();
    slideTopRight2 = dailyTempObj.getCurrentHumidity();
    slideTopRight3 = dailyTempObj.getCurrentWeather();
  }

  // connecting to wifi client
  wifiClient();
}

void alarmT(byte hourT) {
  Serial.println(hourT);
}

/**
 * UPDATING LCD
 */
void updateSlider() {
  switch (slide) {
    case 0:
//      lcd.setCursor(0, 1);
//      lcd.print(slideBottom1);

      lcd.setCursor(9, 0);
      lcd.print(slideTopRight1 + "  ");
    break;
    case 1:
//      lcd.setCursor(0, 1);
//      lcd.print(slideBottom2);

      lcd.setCursor(9, 0);
      lcd.print(slideTopRight2 + "  ");
    break;
    case 2:
//      lcd.setCursor(0, 1);
//      lcd.print(slideBottom3 + "         ");

      lcd.setCursor(9, 0);
      lcd.print(slideTopRight3 + "  ");
    break;
  }

  // 0, 1, 2, 3 (for 3)
  if (slide++ == 3) {
    slide = 0;
  }
}

/**
 * UPDATE CLOCK
 */
void updateTime(RtcDateTime currentTime) {
  int hourNum = currentTime.Hour();
  int minuteNum = currentTime.Minute();
  int secondNum = currentTime.Second();

  lcd.print(String(hourNum < 10 ? "0" : "") + String(hourNum) + ":" + String(minuteNum < 10 ? "0" : "") + String(minuteNum) + ":" + String(secondNum < 10 ? "0" : "") + String(secondNum) + " ");
}

/**
 * HANDLING WIFI CLIENT
 * 
 * doesn't work properly
 */
void wifiClient() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Match the request
  int value = LOW;
  if (request.indexOf("/LED=ON") != -1)  {
    digitalWrite(ledPin, LOW);
    value = HIGH;
  }
  if (request.indexOf("/LED=OFF") != -1)  {
    digitalWrite(ledPin, HIGH);
    value = LOW;
  }
  
  // Return the response
  String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n";
  String body = "GPIO is now ";
         body += (value == HIGH) ? "ON" : "OFF";
         body += "<a href=\"/LED=ON\"\"><button>Turn On </button></a>";
         body += "<a href=\"/LED=OFF\"\"><button>Turn Off </button></a><br />";
  String footer = "</html>\n";

  String response = header + body + footer;
  client.println(response);

 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}
 
