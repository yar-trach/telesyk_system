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

#include <RtcDateTime.h> // https://github.com/Makuna/Rtc
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
unsigned long btnLastPress = 0;

#define ONBOARD_LED_PIN   2
#define RED_LED_PIN       D5
#define GREEN_LED_PIN     D6
#define BLUE_LED_PIN      D7
#define BTN_SWITCH_PIN    D3

#define SLIDER_INTERVAL         2000    // 2 sec
#define CLOCK_INTERVAL          1000    // 1 sec
#define WEATHER_CURR_INTERVAL   600000  // 10 min
#define BTN_DEBOUNCE_TIME       200     // .2 sec

WiFiServer server(80);

String slideBottom1;
String slideBottom2;
String slideBottom3;

String slideTopRight1;
String slideTopRight2;
String slideTopRight3;

byte slide = 0;
byte slideLocalInfo = 0;
boolean blink = 1;
boolean btnFlag = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);

dailyTemperature dailyTempObj = dailyTemperature(RED_LED_PIN, GREEN_LED_PIN, BLUE_LED_PIN);
currentTime currentTimeObj = currentTime();

RtcDS3231<TwoWire> rtcObject(Wire);
RtcDateTime rtcExactTime;
RtcTemperature rtcTemperature = rtcObject.GetTemperature();

byte hourNum;
byte minuteNum;
byte secondNum;

void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println("\nBooting");

  // Wifi LED   
  pinMode(ONBOARD_LED_PIN, OUTPUT);
  digitalWrite(ONBOARD_LED_PIN, !HIGH);

  // Button
  pinMode(BTN_SWITCH_PIN, INPUT_PULLUP);
    
  // LCD
  lcd.init();                     
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("SEARCHING WIFI  ");

  // Scanning WiFi network
  byte n = WiFi.scanNetworks();
  Serial.println("scan done");

  // Connect to WiFi network
  if (n == 0) {
    Serial.println("No networks found");
    lcd.setCursor(0, 0);
    lcd.print("NO WIFI FOUND   ");
  } else {
    for (byte i = 0; i < n; i++) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");

      // Checking if available WiFi point available in list
      for (byte j = 0; j < SSID_PASS; j += 2) {
        if (WiFi.SSID(i) == ssidpass[j]) {
          Serial.print("Connecting to ");
          Serial.println(ssidpass[j]);

          lcd.setCursor(0, 0);
          lcd.print("CONNECTING TO   ");
          lcd.setCursor(0, 1);
          lcd.print(ssidpass[j]);
          
          if (WiFi.status() != WL_CONNECTED) {
            WiFi.begin(ssidpass[j], ssidpass[j+1]);
          }
          goto checkingStatus;
        }
      } // END OF Checking if available WiFi point available in list
    }
  } // END OF Connect to WiFi network

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

  // Compiling RTC. Or not
  rtcObject.Begin();

  RtcDateTime compiledDateTime(__DATE__, __TIME__);
  Serial.print("compiledDateTime>>>");
  Serial.println(compiledDateTime);

  RtcDateTime loadingTime(currentTimeObj.getLocalTime());

  // Checking if RTC has valid time. If not - set RTC exact time
  if(!rtcObject.IsDateTimeValid()) {
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
    Serial.print("loadingTime>>>");
    Serial.println(loadingTime);

    rtcExactTime = rtcObject.GetDateTime();
    Serial.print("rtcExactTime>>>");
    Serial.println(rtcExactTime);

    Serial.print("CHECKING>>");
    Serial.println(abs((loadingTime - TIME_DIFF_2000) - rtcExactTime) > 100);
    
    if ((loadingTime > 0) && (abs((loadingTime - TIME_DIFF_2000) - rtcExactTime) > 100)) {
      Serial.println("Setting up timepoint from Internet");
      rtcObject.SetDateTime(loadingTime - TIME_DIFF_2000);
    } else {
      Serial.println("Time is fine!");
    }
  } // END OF Checking if RTC has valid time. If not - set RTC exact time
 
  // Get current time
  rtcExactTime = rtcObject.GetDateTime();
  hourNum = rtcExactTime.Hour();
  minuteNum = rtcExactTime.Minute();
  rtcTemperature = rtcObject.GetTemperature();

  // Show connection info for 1 sec
  delay(1000);
  lcd.clear();
  
  showTime();

  // Get current weather condition
  dailyTempObj.getWeatherCurrentCondition();
  getSlideTopRight(dailyTempObj);
  
  // Get daily weather
  dailyTempObj.getWeatherDailyCondition(hourNum);
  getSlideBottom(dailyTempObj);
}

/**
 * MAIN BODY OF SCATCH
 */
void loop() {
  clockGen = millis();

  // Button handler
  boolean btn = !digitalRead(BTN_SWITCH_PIN);
  if (btn == 1 && btnFlag == 0 && clockGen - btnLastPress > BTN_DEBOUNCE_TIME) {
    btnFlag = 1;
    slideLocalInfo = !slideLocalInfo;
    btnLastPress = clockGen;

    if (slideLocalInfo == 0) {
      showTime();
    } else if (slideLocalInfo == 1) {
      showTemperature();
    }
  } else if (btn == 0 && btnFlag == 1) {
    btnFlag = 0;
  } // END OF Button handler

  // Show time (every second)
  if (clockGen - timeClockUpd >= CLOCK_INTERVAL) {
    timeClockUpd = clockGen;

    // Update time
    rtcExactTime = rtcObject.GetDateTime();
    hourNum = rtcExactTime.Hour();
    minuteNum = rtcExactTime.Minute();
    secondNum = rtcExactTime.Second();

    if (secondNum == 0) {
      if (slideLocalInfo == 0) {
        showTime();
      } else if (slideLocalInfo == 1) {
        rtcTemperature = rtcObject.GetTemperature();
        showTemperature();
      }

      if (minuteNum == 0) {
        // should be rewriten by using interrupt with SQW pin (using 6-pin DS3231) and alarms
        if (hourNum == 3 || hourNum == 6 || hourNum ==  9 || hourNum ==  12 || hourNum ==  15 || hourNum ==  18 || hourNum ==  21 || hourNum ==  0) {
          dailyTempObj.getWeatherDailyCondition(hourNum);
          getSlideBottom(dailyTempObj);
        }
      }
    }

    if (slideLocalInfo == 0) {
      lcd.setCursor(2, 0);
      lcd.print(blink ? ":" : " ");
      blink = !blink;
    }
  } // END OF Show time (every second)

  // Update slider info (every two seconds)
  if (clockGen - timeSliderUpd >= SLIDER_INTERVAL) {
    timeSliderUpd = clockGen;

    updateSlider();
  } // END OF Update slider info (every two seconds)

  // Updating current weather (every 10 minutes)
  if (clockGen - timeWeatherCurrentReq >= WEATHER_CURR_INTERVAL) {
    timeWeatherCurrentReq = clockGen;

    // update current weather condition
    dailyTempObj.getWeatherCurrentCondition();
    getSlideTopRight(dailyTempObj);
  } // END OF Updating current weather (every 10 minutes)

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
 * SHOWING TIME
 */
void showTime() {
  lcd.setCursor(0, 0);
  lcd.print(String(hourNum < 10 ? "0" : "") + String(hourNum) + String(blink ? ":" : " ") + String(minuteNum < 10 ? "0" : "") + String(minuteNum));
}

/**
 * SHOWING TEMPERATURE
 */
void showTemperature() {
  lcd.setCursor(0, 0);
  lcd.print(String(rtcTemperature.AsWholeDegrees() > 0 ? "+" : "-") + String(rtcTemperature.AsWholeDegrees() < 10 ? "0" : "") + rtcTemperature.AsWholeDegrees() + "C" + String((char)223));
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
    digitalWrite(ONBOARD_LED_PIN, LOW);
    value = HIGH;
  }
  if (request.indexOf("/LED=OFF") != -1)  {
    digitalWrite(ONBOARD_LED_PIN, HIGH);
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
 
