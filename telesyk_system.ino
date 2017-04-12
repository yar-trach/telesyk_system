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
unsigned long timeClockBlink = 0;

#define LED_PIN 2

#define SLIDER_INTERVAL 2000 // 2 seconds
#define CLOCK_INTERVAL 60000 // 1 minute
#define WEATHER_CURR_INTERVAL 600000 // 10 minutes

WiFiServer server(80);

String slideBottom1;
String slideBottom2;
String slideBottom3;

String slideTopRight1;
String slideTopRight2;
String slideTopRight3;

byte slide = 0;
byte blink = 0;

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
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
    
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

      // Checking if available WiFi point available in list
      for (int j = 0; j < SSID_PASS; j += 2) {
        if (WiFi.SSID(i) == ssidpass[j]) {
          Serial.print("Connecting to ");
          Serial.println(ssidpass[j]);
          lcd.setCursor(11, 0);
          lcd.print(ssidpass[j]);
          if (WiFi.status() != WL_CONNECTED) {
            WiFi.begin(ssidpass[j], ssidpass[j+1]);
          }
          goto checkingStatus;
        }
      }
    }
  }

  checkingStatus: while (WiFi.status() != WL_CONNECTED) {
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

  // Compiling RTC. or not
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
    Serial.print("loadingTime>>>");
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
 
  // Get current time
  RtcDateTime currentTime = rtcObject.GetDateTime();
  
  // Get current weather condition
  dailyTempObj.getWeatherCurrentCondition();
  getSlideTopRight(dailyTempObj);
  
  // Get daily weather
  dailyTempObj.getWeatherDailyCondition(currentTime.Hour());
  getSlideBottom(dailyTempObj);
}

/**
 * MAIN BODY OF SCATCH
 */
void loop() {
  RtcDateTime currentTime = rtcObject.GetDateTime();

  clockGen = millis();

  // Show time (every half second)
  if (clockGen - timeClockBlink >= 500) {
    timeClockBlink = clockGen;

    lcd.setCursor(2, 0);
    if (blink) {
      lcd.print(":");
    } else {
      lcd.print(" ");
    }
    blink = !blink;
  }
      

  // Show time (every minute)
  if (clockGen - timeClockUpd >= CLOCK_INTERVAL) {
    timeClockUpd = clockGen;

    byte hourNum = currentTime.Hour();
    byte minuteNum = currentTime.Minute();

    updateTime(hourNum, minuteNum);

    Serial.print(hourNum);
    Serial.println(minuteNum);

    // should be rewriten by using interrupt with SQW pin (using 6-pin DS3231) and alarms    
    if ((hourNum == 3 && minuteNum == 0)
     || (hourNum == 6 && minuteNum == 0)
     || (hourNum == 9 && minuteNum == 0)
     || (hourNum == 12 && minuteNum == 0)
     || (hourNum == 15 && minuteNum == 0)
     || (hourNum == 18 && minuteNum == 0)
     || (hourNum == 21 && minuteNum == 0)
     || (hourNum == 0 && minuteNum == 0)){
      dailyTempObj.getWeatherDailyCondition(hourNum);
      getSlideBottom(dailyTempObj);
    }
  }

  // Update slider info (every two seconds)
  if (clockGen - timeSliderUpd >= SLIDER_INTERVAL) {
    timeSliderUpd = clockGen;

    updateSlider();
  }

  // Updating current weather (every 10 minutes)
  if (clockGen - timeWeatherCurrentReq >= WEATHER_CURR_INTERVAL) {
    timeWeatherCurrentReq = clockGen;

    // update current weather condition
    dailyTempObj.getWeatherCurrentCondition();
    getSlideTopRight(dailyTempObj);
  }

  // connecting to wifi client
  wifiClient();
}
/**
 * END MAIN BODY OF SCATCH
 */

void getSlideTopRight(dailyTemperature dailyTempObj) {
  slideTopRight1 = dailyTempObj.getCurrentTemp();
  slideTopRight2 = dailyTempObj.getCurrentHumidity();
  slideTopRight3 = dailyTempObj.getCurrentWeather();
}

void getSlideBottom(dailyTemperature dailyTempObj) {
  slideBottom1 = dailyTempObj.getMorDayTemp();
  slideBottom2 = dailyTempObj.getEveNigTemp();
  slideBottom3 = dailyTempObj.getWeatherDescription();
}

/**
 * UPDATING LCD
 */
void updateSlider() {
  switch (slide) {
    case 0:
      lcd.setCursor(0, 1);
      lcd.print(slideBottom1);

      lcd.setCursor(9, 0);
      lcd.print(slideTopRight1 + "  ");
    break;
    case 1:
      lcd.setCursor(0, 1);
      lcd.print(slideBottom2);

      lcd.setCursor(9, 0);
      lcd.print(slideTopRight2 + "  ");
    break;
    case 2:
      lcd.setCursor(0, 1);
      lcd.print(slideBottom3 + "         ");
 
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
void updateTime(byte hourNum, byte minuteNum) {
  lcd.setCursor(0, 0);
  lcd.print(String(hourNum < 10 ? "0" : "") + String(hourNum) + ":" + String(minuteNum < 10 ? "0" : "") + String(minuteNum) + "    ");
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
    digitalWrite(LED_PIN, LOW);
    value = HIGH;
  }
  if (request.indexOf("/LED=OFF") != -1)  {
    digitalWrite(LED_PIN, HIGH);
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
 
