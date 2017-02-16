#include <Arduino.h>
#include <MQTTClient.h>

#ifndef NETSTEPPER_STEPPER_H
#define NETSTEPPER_STEPPER_H

#define STEPPER_EN 5
#define STEPPER_MS1 4
#define STEPPER_MS2 3
#define STEPPER_MS3 2
#define STEPPER_STEP 1
#define STEPPER_DIR 0
#define STEPPER_POS 0

class Stepper {
private:
  unsigned long lastMicros = 0;
  boolean stepping = false;
  boolean on = false;
  int speed = 5000;
  boolean search = false;
  int lastPos = 0;
  MQTTClient *client;
  void setResolution(uint8_t, uint8_t, uint8_t);

public:
  void setup(MQTTClient*);
  void enable(boolean);
  void changeResolution(int);
  void changeDirection(int);
  void changeSpeed(int);
  void enableSearch(boolean);
  void loop();
};

#endif // NETSTEPPER_STEPPER_H
