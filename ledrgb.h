/**
 *  Color scheme
    ╔════╦══════════════════╦═════════════╗
    ║    ║    RGB Color     ║ Temperature ║
    ╠════╬══════════════════╬═════════════╣
    ║  1 ║ rgb(255, 0, 255) ║ -273 to -29 ║
    ║  2 ║ rgb(128, 0, 255) ║ -28 to -21  ║
    ║  3 ║ rgb(0, 0, 255)   ║ -20 to 0    ║
    ║  4 ║ rgb(0, 128, 255) ║ 0 to 7      ║
    ║  5 ║ rgb(0, 255, 255) ║ 8 to 15     ║
    ║  6 ║ rgb(0, 255, 128) ║ 16 to 20    ║
    ║  7 ║ rgb(0, 255, 0)   ║ 21 to 25    ║
    ║  8 ║ rgb(255, 255, 0) ║ 26 to 32    ║
    ║  9 ║ rgb(255, 128, 0) ║ 33 to 37    ║
    ║ 10 ║ rgb(255, 64, 0)  ║ 38 to 54    ║
    ║ 11 ║ rgb(255, 0, 0)   ║ > 54        ║
    ╚════╩══════════════════╩═════════════╝
 */

#ifndef LEDRGB_H
#define LEDRGB_H

class LEDRGB {
  public:
  LEDRGB(int pin_red, int pin_green, int pin_blue);
  void changeColor(int tempCurrent);

  private:
  const int _pin_red;
  const int _pin_green;
  const int _pin_blue;
};

#endif
