#include "stepper.h";

#define IN1  8
#define IN2  9
#define IN3  10
#define IN4  11

STEPPER stepper(4096, IN1, IN2, IN3, IN4);

void setup(){
  Serial.begin(115200);
  stepper.setSpeed(16); // 16 revolutions per minute
}
void loop(){
  stepper.run(3000);
  delay(3000);
  stepper.run(-3000);
  delay(3000);
}
