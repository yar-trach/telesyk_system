#ifndef WINDOW_SLAVE_H
#define WINDOW_SLAVE_H

class WINDOW_SLAVE {  
  public:
  WINDOW_SLAVE(void);
  void init(int address);
  static void receiveEvent(int command);
  int getPosition(void);

  private:
  static int _shutterPosition;
};

#endif
