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

#define STEPPER_SMOOTHING 10

enum DriveMode { Unset, Idle, Absolute, Continuous };

enum Direction { CW, CCW };

class Stepper {
 private:
  boolean powered = false;
  DriveMode mode = Unset;
  int resolution = 1;
  Direction direction = CW;
  int speed = 5000;
  int threshold = 0;
  double target = 0.0;
  double position = 0.0;

  unsigned long lastStep = 0;
  boolean stepping = false;
  int lastReading = 0;
  double lastPosition = 0.0;
  DriveMode lastStatus = Idle;
  MQTTClient *client;

  String powerTopic;
  String stopTopic;
  String resolutionTopic;
  String directionTopic;
  String speedTopic;
  String searchTopic;
  String targetTopic;
  String statusTopic;
  String sensorTopic;
  String positionTopic;

  void writePower(boolean on);
  void writeResolution(int resolution);
  void writeResolutionBits(uint8_t ms1, uint8_t ms2, uint8_t ms3);
  void writeDirection(Direction direction);
  void writeStep(boolean on);
  int readSensor();

 public:
  Stepper();
  void setup(MQTTClient *);
  void handle(String topic, String payload);
  void loop();
};

#endif  // NETSTEPPER_STEPPER_H
