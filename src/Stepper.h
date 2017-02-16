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
  unsigned long lastStep = 0;
  boolean stepping = false;
  boolean enabled = false;
  int direction = 0;
  int speed = 5000;
  boolean search = false;
  int lastReading = 0;
  MQTTClient *client;
  void setResolution(uint8_t, uint8_t, uint8_t);

public:
  void setup(MQTTClient*);

  /**
   * Enable the stepper motor.
   *
   * @param yes
   */
  void enable(boolean yes);

  /**
   * Change the resolution of the stepper.
   *
   * @param res Should be between 1, 2, 4, 8 or 16.
   */
  void changeResolution(int res);

  /**
   * Change direction of the stepper.
   *
   * @param dir Should be -1, 0 or 1.
   */
  void changeDirection(int dir);

  /**
   * Change the speed of the stepper. Lower is faster.
   *
   * @param speed Should be between 10 and 10000.
   */
  void changeSpeed(int speed);

  /**
   * Enable the zero point search.
   *
   * @param yes
   */
  void enableSearch(boolean yes);

  void loop();
};

#endif // NETSTEPPER_STEPPER_H
