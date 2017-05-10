#include "stepper.h";
#include "window_slave.h";

#define IN1  8
#define IN2  9
#define IN3  10
#define IN4  11

#define ONE_MINUTE_INTERVAL 60000 // 1 minute

unsigned long clockGen = 0;
unsigned long lastOneMinute = 0;

int commandPrev = 0;
int commandNew = 0;

STEPPER stepper(4096, IN1, IN2, IN3, IN4);
WINDOW_SLAVE window;

void setup(){
  stepper.setSpeed(16); // 16 revolutions per minute
  window.init(0x08); // init window slave handler at address "8"
}
void loop(){
  clockGen = millis();

  if (clockGen - lastOneMinute >= ONE_MINUTE_INTERVAL) {
    lastOneMinute = clockGen;
    
    commandNew = window.getPosition();

    if (commandNew != commandPrev) {
      if (commandNew > commandPrev) {
        // go forward
        stepper.run(500);
      } else {
        // go back
        stepper.run(-500);
      }
      commandPrev = commandNew;
    }
  }
}
