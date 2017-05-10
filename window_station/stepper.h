/**
 * SWITCHING SEQUENCE FOR STEP MOTOR 28BYJ-48 WITH ULN2003A DRIVER
 * 
 * TECH DATA
 * Rev = 48
 * Spr = 4096
 * 

  ╔════════════╦═══════════════════════════════╗
  ║            ║          Direction ->         ║
  ║ Wire color ╠═══╦═══╦═══╦═══╦═══╦═══╦═══╦═══╣
  ║            ║ 1 ║ 2 ║ 3 ║ 4 ║ 5 ║ 6 ║ 7 ║ 8 ║
  ╠════════════╬═══╬═══╬═══╬═══╬═══╬═══╬═══╬═══╣
  ║ 4. Orange  ║███║███║   ║   ║   ║   ║   ║███║
  ╠════════════╬═══╬═══╬═══╬═══╬═══╬═══╬═══╬═══╣
  ║ 3. Yellow  ║   ║███║███║███║   ║   ║   ║   ║
  ╠════════════╬═══╬═══╬═══╬═══╬═══╬═══╬═══╬═══╣
  ║ 2. Pink    ║   ║   ║   ║███║███║███║   ║   ║
  ╠════════════╬═══╬═══╬═══╬═══╬═══╬═══╬═══╬═══╣
  ║ 1. Blue    ║   ║   ║   ║   ║   ║███║███║███║
  ╚════════════╩═══╩═══╩═══╩═══╩═══╩═══╩═══╩═══╝

 * 
 */

#ifndef STEPPER_H
#define STEPPER_H

class STEPPER {
  public:
  STEPPER(int totalNumberOfSteps, int pinIn1, int pinIn2, int pinIn3, int pinIn4);
  void run(int stepsToRun);
  void setSpeed(int rev);

  private:
  const int _pinIn1;
  const int _pinIn2;
  const int _pinIn3;
  const int _pinIn4;
  const int _totalNumberOfSteps;
  int _step;
  unsigned long _stepDelay;
  unsigned long _lastStepTime;
  boolean _direction;

  void impulse(int phase);
};

#endif
