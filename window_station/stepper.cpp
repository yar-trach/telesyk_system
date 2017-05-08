#include <Arduino.h>
#include "stepper.h";

STEPPER::STEPPER(int totalNumberOfSteps, const int pinIn1, const int pinIn2, const int pinIn3, const int pinIn4)
:
  _totalNumberOfSteps(totalNumberOfSteps),
  _pinIn1(pinIn1),
  _pinIn2(pinIn2),
  _pinIn3(pinIn3),
  _pinIn4(pinIn4)
{
  pinMode(pinIn1, OUTPUT);
  pinMode(pinIn2, OUTPUT);
  pinMode(pinIn3, OUTPUT);
  pinMode(pinIn4, OUTPUT);
}

void STEPPER::run(int stepsToRun) {
  int steps_left = abs(stepsToRun);

  _direction = stepsToRun > 0 ? 1 : 0;

  while (steps_left > 0) {
    unsigned long now = micros();

    if (now - _lastStepTime >= _stepDelay) {
      _lastStepTime = now;

      if (_direction) {
        _step++;
        if (_step == _totalNumberOfSteps) _step = 0;
      } else {
        if (_step == 0) _step = _totalNumberOfSteps;
        _step--;
      }
      
      steps_left--;
      impulse(_step % 8);
    }
  }
}

void STEPPER::setSpeed(int rev) {
  _stepDelay = 60L * 1000L * 1000L / _totalNumberOfSteps / rev;
}

void STEPPER::impulse(int phase) {
  switch(phase){
    case 0:
      digitalWrite(_pinIn1, LOW); 
      digitalWrite(_pinIn2, LOW);
      digitalWrite(_pinIn3, LOW);
      digitalWrite(_pinIn4, HIGH);
    break; 
    case 1:
      digitalWrite(_pinIn1, LOW); 
      digitalWrite(_pinIn2, LOW);
      digitalWrite(_pinIn3, HIGH);
      digitalWrite(_pinIn4, HIGH);
    break; 
    case 2:
      digitalWrite(_pinIn1, LOW); 
      digitalWrite(_pinIn2, LOW);
      digitalWrite(_pinIn3, HIGH);
      digitalWrite(_pinIn4, LOW);
    break; 
    case 3:
      digitalWrite(_pinIn1, LOW); 
      digitalWrite(_pinIn2, HIGH);
      digitalWrite(_pinIn3, HIGH);
      digitalWrite(_pinIn4, LOW);
    break; 
    case 4:
      digitalWrite(_pinIn1, LOW); 
      digitalWrite(_pinIn2, HIGH);
      digitalWrite(_pinIn3, LOW);
      digitalWrite(_pinIn4, LOW);
    break; 
    case 5:
      digitalWrite(_pinIn1, HIGH); 
      digitalWrite(_pinIn2, HIGH);
      digitalWrite(_pinIn3, LOW);
      digitalWrite(_pinIn4, LOW);
    break; 
    case 6:
      digitalWrite(_pinIn1, HIGH); 
      digitalWrite(_pinIn2, LOW);
      digitalWrite(_pinIn3, LOW);
      digitalWrite(_pinIn4, LOW);
    break; 
    case 7:
      digitalWrite(_pinIn1, HIGH); 
      digitalWrite(_pinIn2, LOW);
      digitalWrite(_pinIn3, LOW);
      digitalWrite(_pinIn4, HIGH);
    break; 
    default:
      digitalWrite(_pinIn1, LOW); 
      digitalWrite(_pinIn2, LOW);
      digitalWrite(_pinIn3, LOW);
      digitalWrite(_pinIn4, LOW);
    break; 
  }
}

