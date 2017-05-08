#include <Arduino.h>
#include "ledrgb.h"

LEDRGB::LEDRGB(const int pin_red, const int pin_green, const int pin_blue)
:
  _pin_red(pin_red),
  _pin_green(pin_green),
  _pin_blue(pin_blue)
{
  pinMode(pin_red, OUTPUT);
  pinMode(pin_green, OUTPUT);
  pinMode(pin_blue, OUTPUT);
}

void LEDRGB::changeColor(int tempCurrent) {
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
  
  analogWrite(_pin_red, red);
  analogWrite(_pin_green, green);
  analogWrite(_pin_blue, blue);
}

