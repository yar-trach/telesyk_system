#include "window_master.h";
#include <Wire.h>

WINDOW_MASTER::WINDOW_MASTER(int address)
:
  _address(address)
{}

void WINDOW_MASTER::sendCommand(int command) {
  Wire.beginTransmission(_address);
  Wire.write(command);
  Wire.endTransmission();
}

