#include <Arduino.h>
#include <Wire.h>
#include "window_slave.h";

int WINDOW_SLAVE::_shutterPosition;

WINDOW_SLAVE::WINDOW_SLAVE(void) {
  pinMode(13, OUTPUT);
}

void WINDOW_SLAVE::init(int address) {
  Wire.begin(address);
  Wire.onReceive(receiveEvent);
}

static void WINDOW_SLAVE::receiveEvent(int command) {
  while (Wire.available()) {
    int command = Wire.read();

    if (command != _shutterPosition) {
      if (command > _shutterPosition) {
        // step forward
      } else if (command < _shutterPosition) {
        // step back
      }
      _shutterPosition = command;
    }
  }
}

