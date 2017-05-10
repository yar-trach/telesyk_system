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

int WINDOW_MASTER::shutterPosition(int brightness) {
  if (brightness < 800 && _shutterPosition < 2) {
    _shutterPosition++;
    sendCommand(_shutterPosition);
  } else if (brightness >= 800 && _shutterPosition > 0) {
    _shutterPosition--;
    sendCommand(_shutterPosition);
  }
  return _shutterPosition;
}

