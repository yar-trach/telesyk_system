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
  window.init(8); // init window slave handler at address "8"
}
void loop(){
  stepper.run(3000);
  delay(3000);
  stepper.run(-3000);
  delay(3000);
}
