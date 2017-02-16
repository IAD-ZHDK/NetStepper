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

enum DriveMode { Idle, Single, Continuous };

enum Direction { Left, Right };

class Stepper {
private:
  boolean enabled = false;
  DriveMode mode = Idle;
  Direction direction = Left;
  int speed = 5000;
  boolean search = false;

  unsigned long lastStep = 0;
  boolean stepping = false;
  int steps = 0;
  int lastReading = 0;
  MQTTClient *client;

  void _setResolution(uint8_t, uint8_t, uint8_t);

public:
  /**
   * Setup the stepper motor.
   */
  void setup(MQTTClient *);

  /**
   * Enable or disable the stepper motor.
   *
   * @param yes
   */
  void setEnabled(boolean yes);

  /**
   * Change the stepper drive mode.
   *
   * @param mode
   */
  void setDriveMode(DriveMode mode);

  /**
   * Change the resolution of the stepper.
   *
   * @param resolution Should be between 1, 2, 4, 8 or 16.
   */
  void setResolution(int resolution);

  /**
   * Change the direction of the stepper.
   *
   * @param direction Should be -1, 0 or 1.
   */
  void setDirection(Direction direction);

  /**
   * Change the speed of the stepper. Lower is faster.
   *
   * @param speed Should be between 10 and 10000.
   */
  void setSpeed(int speed);

  /**
   * Enable or disable the zero point search.
   *
   * @param yes
   */
  void setSearch(boolean yes);

  /**
   * Set the amount of steps to move in Single mode.
   *
   * @param steps
   */
  void setSteps(int steps);

  /**
   * Do one loop.
   */
  void loop();
};

#endif // NETSTEPPER_STEPPER_H
