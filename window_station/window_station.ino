#include "stepper.h";
#include "window_slave.h";

#define IN1  8
#define IN2  9
#define IN3  10
#define IN4  11

STEPPER stepper(4096, IN1, IN2, IN3, IN4);
WINDOW_SLAVE window;

void setup(){
  stepper.setSpeed(16); // 16 revolutions per minute
  window.init(0x08); // init window slave handler at address "8"
}
void loop(){
  stepper.run(300);
  delay(5000);
  stepper.run(-300);
  delay(5000);
}
