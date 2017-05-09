#include <Arduino.h>
#include <Wire.h>
#include "window_slave.h";

WINDOW_SLAVE::WINDOW_SLAVE(void) {
  pinMode(13, OUTPUT);
}

void WINDOW_SLAVE::init(int address) {
  Wire.begin(address);
  Wire.onReceive(receiveEvent);
}

static void WINDOW_SLAVE::receiveEvent(int command) {
  while (Wire.available()) {
    char c = Wire.read();
    if (c == 1) {
      digitalWrite(13, LOW);
    } else {
      digitalWrite(13, HIGH);
    }
  }
}

