#ifndef WINDOW_MASTER_H
#define WINDOW_MASTER_H

class WINDOW_MASTER {  
  public:
  WINDOW_MASTER(int address);
  void sendCommand(int command);
  int shutterPosition(int brightness);

  int _shutterPosition;

  private:
  int _address;
};

#endif
