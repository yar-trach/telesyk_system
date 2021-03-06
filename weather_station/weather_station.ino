/**
 * File with id and password to access to wifi point
#define SSID_PASS 4 // size of ssid and pass (including ssids' and passwords for them

const char* ssidpass[SSID_PASS] = {
  "...", // ssid name of first point
  "...", // password of first point
  
  "...", // ssid name of second point
  "..."  // password of second point
};
// Here we have 2 WiFi access poins, so SSID_PASS = 4
 */
// Uncomment text above or use file below for presenting WiFi access point for a program
#include "ssidpassword.h" // file with Wifi access points and passwords for them

#include <RtcDateTime.h> // https://github.com/Makuna/Rtc
#include <RtcUtility.h>
#include <RtcDS3231.h>

// I2C and SPI protocols
#include <Wire.h>
#include <SPI.h>

#include <SD.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>

#include "dailyTemperature.h"
#include "currentTime.h"
#include "ledrgb.h";
#include "window_master.h";

unsigned long clockGen = 0;
unsigned long lastQuickCycle = 0;
unsigned long lastOneSecond = 0;
unsigned long lastTwoSeconds = 0;
unsigned long lastTenMinutes = 0;
unsigned long lastButtonPressed = 0;

#define PHOTOCELL_PIN       A0 // ADC0
#define BTN_SWITCH_PIN      D3 // GPIO0
#define MICRO_SD_PIN        D8 // GPIO15
#define RED_LED_PIN         10 // SD3 (GPIO10) - can't recognise SD3 like pin number
#define GREEN_LED_PIN       D4 // GPIO2
#define BLUE_LED_PIN        D0 // GPIO16

#define BTN_DEBOUNCE_TIME       200     // .2 sec
#define BUSY_FLAG_TIME          200     // .2 sec
#define ONE_SECOND_INTERVAL     1000    // 1 sec
#define TWO_SECOND_INTERVAL     2000    // 2 sec
#define TEN_MINUTES_INTERVAL    600000  // 10 min

uint8_t slide = 0;
uint8_t localInfo = 0;
uint8_t hourNum;
uint8_t minuteNum;
uint8_t secondNum;
uint8_t numberOfTry = 0;
uint8_t shutterPosition = 0;

boolean blinkCursor = 1;
boolean btnFlag = 0;
boolean busyFlag = 0;

uint8_t brightness = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);
File weatherFile;
LEDRGB indicator(RED_LED_PIN, GREEN_LED_PIN, BLUE_LED_PIN);
DAILYTEMPERATURE dailyTempObj;
CURRENTTIME currentTimeObj;
WINDOW_MASTER window(0x08);

RtcDS3231<TwoWire> rtcObject(Wire);
RtcDateTime rtcExactTime;
RtcTemperature rtcTemperature = rtcObject.GetTemperature();

void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println("\nBooting");

  // Photocell
  pinMode(PHOTOCELL_PIN, INPUT);

  // Button
  pinMode(BTN_SWITCH_PIN, INPUT_PULLUP);

  // LCD
  lcd.init(); // LCD init() function initializing Wire() instead of us
  lcd.backlight();
  lcd.setCursor(0, 0);

  Serial.print("Initializing SD card...");
  if (!SD.begin(MICRO_SD_PIN)) {
    Serial.println("initialization failed!");

    goto connectingToWifi;
  }
  Serial.println("initialization done.");

  // Scanning WiFi network
  connectingToWifi: lcd.print("SEARCHING WIFI  ");
  scanNetworks: byte n = WiFi.scanNetworks();
  Serial.println("scan done");

  // Connect to WiFi network
  if (n == 0) {
    Serial.println("No networks found");
    lcd.setCursor(0, 0);
    lcd.print("NO WIFI FOUND   ");

    // Wait 10 seconds and scan network again
    delay(10000);
    goto scanNetworks;
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
    
//    blinkOnboardLed();
    
    delay(200);
  }
  
  checkBusy();

  lcd.setCursor(0, 0);
  lcd.print("WIFI CONNECTED  ");
  Serial.println("\nWiFi connected");

  // Compiling RTC
  rtcObject.Begin();

  RtcDateTime compiledDateTime(__DATE__, __TIME__);
  Serial.print("compiledDateTime>>>");
  Serial.println(compiledDateTime);

  int localTimeRespond = 0;
  while(!localTimeRespond) {
    localTimeRespond = currentTimeObj.getLocalTime();
    
//    blinkOnboardLed();
    
    numberOfTry++;
    if (numberOfTry == 10) {
      numberOfTry = 0;
      Serial.println("\nNO RESPOND FROM DATE TIME SERVER");
      goto cannotGetLocalTime;
    }
  }

  cannotGetLocalTime: checkBusy();

  RtcDateTime loadingTime(localTimeRespond);

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

  // Show connection info for 1 sec
  delay(1000);
  lcd.clear();

  // Get current weather condition
  boolean gotWeatherCurrent = false;
  while(!gotWeatherCurrent) {
    gotWeatherCurrent = dailyTempObj.getWeatherCurrentCondition(indicator);

//    blinkOnboardLed();

    delay(200);
    
    numberOfTry++;
    if (numberOfTry == 10) {
      numberOfTry = 0;
      Serial.println("\nNO RESPOND FROM WEATHER SERVER");
      goto cannotGetCurrentWeather;
    }
  }

  cannotGetCurrentWeather: checkBusy();
  
  // Get daily weather
  boolean gotWeatherDaily = false;
  while(!gotWeatherDaily) {
    gotWeatherDaily = dailyTempObj.getWeatherDailyCondition(hourNum, weatherFile);

//    blinkOnboardLed();

    delay(200);
    
    numberOfTry++;
    if (numberOfTry == 10) {
      numberOfTry = 0;
      Serial.println("\nNO RESPOND FROM WEATHER SERVER");
      goto cannotGetDailyWeather;
    }
  }

  cannotGetDailyWeather: checkBusy();
  
  // Get current time
  rtcTemperature = rtcObject.GetTemperature();
  rtcExactTime = rtcObject.GetDateTime();
  hourNum = rtcExactTime.Hour();
  minuteNum = rtcExactTime.Minute();
  showTime();
}

/**
 * MAIN BODY OF SCATCH
 */
void loop() {
  clockGen = millis();

  // Button handler
  boolean btn = !digitalRead(BTN_SWITCH_PIN);
  if (btn == 1 && btnFlag == 0 && clockGen - lastButtonPressed > BTN_DEBOUNCE_TIME) {
    btnFlag = 1;
    
    localInfo++;
    if (localInfo == 3) localInfo = 0;
    
    lastButtonPressed = clockGen;

    if (localInfo == 0) {
      showTime();
    } else if (localInfo == 1) {
      showTemperature();
    } else if (localInfo == 2) {
      showBrightness();
    }
  } else if (btn == 0 && btnFlag == 1) {
    btnFlag = 0;
  } // END OF Button handler

//  // Every .2 second cycle
//  if (clockGen - lastQuickCycle >= BUSY_FLAG_TIME) {
//    lastQuickCycle = clockGen;
//
////    busyFlag
//  } // Every .2 second cycle

  // Every second cycle
  if (clockGen - lastOneSecond >= ONE_SECOND_INTERVAL) {
    lastOneSecond = clockGen;

    // Update time
    rtcExactTime = rtcObject.GetDateTime();
    hourNum = rtcExactTime.Hour();
    minuteNum = rtcExactTime.Minute();
    secondNum = rtcExactTime.Second();

    if (secondNum == 0) {
      if (localInfo == 0) {
        showTime();
      } else if (localInfo == 1) {
        // Get local temperature (in room)
        rtcTemperature = rtcObject.GetTemperature();
        showTemperature();
      } else if (localInfo == 2) {
        // Get light intens every minute
        brightness = analogRead(PHOTOCELL_PIN);
        shutterPosition = window.shutterPosition(brightness);
        showBrightness();
      }

      if (minuteNum == 0) {
        // should be rewriten by using interrupt with SQW pin (using 6-pin DS3231) and alarms
//        if (hourNum == 3 || hourNum == 6 || hourNum ==  9 || hourNum ==  12 || hourNum ==  15 || hourNum ==  18 || hourNum ==  21 || hourNum ==  0) {
          dailyTempObj.getWeatherDailyCondition(hourNum, weatherFile);
//        }
      }
    }

    if (localInfo == 0) {
      lcd.setCursor(2, 0);
      lcd.print(blinkCursor ? ":" : " ");
      blinkCursor = !blinkCursor;
    }
  } // END OF Every second cycle

  // Every 2 seconds cycle
  if (clockGen - lastTwoSeconds >= TWO_SECOND_INTERVAL) {
    lastTwoSeconds = clockGen;

    dailyTempObj.showWeatherInfo(slide, lcd);
    if (slide++ == 3) {
      slide = 0;
    }
  } // END OF Every 2 seconds cycle

  // Every ten minuter cycle
  if (clockGen - lastTenMinutes >= TEN_MINUTES_INTERVAL) {
    lastTenMinutes = clockGen;

    // update current weather condition
    dailyTempObj.getWeatherCurrentCondition(indicator);
  } // END OF Every ten minuter cycle
}
/**
 * END MAIN BODY OF SCATCH
 */


void blinkOnboardLed() {
  busyFlag = !busyFlag;
//  digitalWrite(ONBOARD_LED_PIN, !busyFlag);
}

void checkBusy() {
  if (busyFlag) {
    busyFlag = !busyFlag;
//    digitalWrite(ONBOARD_LED_PIN, !busyFlag);
  }
  numberOfTry = 0;
}

void showTime() {
  lcd.setCursor(0, 0);
  lcd.print(String(hourNum < 10 ? "0" : "") + String(hourNum) + String(blinkCursor ? ":" : " ") + String(minuteNum < 10 ? "0" : "") + String(minuteNum));
}

void showTemperature() {
  lcd.setCursor(0, 0);
  lcd.print(String(rtcTemperature.AsWholeDegrees() > 0 ? "+" : "-") + String(rtcTemperature.AsWholeDegrees() < 10 ? "0" : "") + rtcTemperature.AsWholeDegrees() + "C" + String((char)223));
}

void showBrightness() {
  lcd.setCursor(0, 0);
  lcd.print(String(brightness) + "(" + shutterPosition + ")");
}

