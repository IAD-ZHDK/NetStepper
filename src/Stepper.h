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

enum DriveMode { Idle, Absolute, Continuous };

enum Direction { CW, CCW };

class Stepper {
private:
  boolean powered = false;
  DriveMode mode = Idle;
  int resolution = 1;
  Direction direction = CW;
  int speed = 5000;
  int threshold = 0;
  double target = 0.0;
  double position = 0.0;

  unsigned long lastStep = 0;
  boolean stepping = false;
  int lastReading = 0;
  MQTTClient *client;

  void _setResolution(uint8_t, uint8_t, uint8_t);

public:
  /**
   * Setup the stepper motor.
   */
  void setup(MQTTClient *);

  /**
   * Handle incoming messages.
   *
   * @param topic
   * @param payload
   */
  void handle(String topic, String payload);

  /**
   * Enable or disable the stepper motor.
   *
   * @param on
   */
  void setPower(boolean on);

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
   * @param threshold Should be between 0 and 1024.
   */
  void setSearch(int threshold);

  /**
   * Set the absolute position in revolutions from the zero point.
   *
   * @param target
   */
  void setTarget(double target);

  /**
   * Do one loop.
   */
  void loop();
};

#endif // NETSTEPPER_STEPPER_H
