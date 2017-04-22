#include <Servo.h>

#define ANGLE_ZERO      10
#define ANGLE_HALF      95
#define ANGLE_FULL      179

class servo {
  private:
  Servo servoTemp;
  uint8_t servoState = 0; // 0 (closed), 1 (half), 2 (opened)

  void changeAngle(Servo servoTemp, uint8_t servoState) {
    switch (servoState) {
      case 1:
        servoTemp.write(ANGLE_HALF);
        break;

      case 2:
        servoTemp.write(ANGLE_FULL);
        break;

      case 0:
      default:
        servoTemp.write(ANGLE_ZERO);
        break;
    }
  }

  public:
  servo(char SERVO_PIN) {
    servoTemp.attach(SERVO_PIN);
  }

  void checkBrightness(uint8_t brightness) {
    Serial.print("\nChecking bightness>>>");
    Serial.println(brightness);
    if (brightness > 900 && servoState > 0) {
      servoState--;
      changeAngle(servoTemp, servoState);
    } else if (brightness <= 900 && servoState < 2) {
      servoState++;
      changeAngle(servoTemp, servoState);
    }
    Serial.print("Window state>>>");
    Serial.println(servoState);
  } 
};

