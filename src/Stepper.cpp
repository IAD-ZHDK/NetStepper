#include "Stepper.h"

void Stepper::setup(MQTTClient * _client) {
  client = _client;

  pinMode(STEPPER_EN, OUTPUT);
  pinMode(STEPPER_MS1, OUTPUT);
  pinMode(STEPPER_MS2, OUTPUT);
  pinMode(STEPPER_MS3, OUTPUT);
  pinMode(STEPPER_STEP, OUTPUT);
  pinMode(STEPPER_DIR, OUTPUT);

  digitalWrite(STEPPER_EN, LOW);
  digitalWrite(STEPPER_MS1, LOW);
  digitalWrite(STEPPER_MS2, LOW);
  digitalWrite(STEPPER_MS3, LOW);
  digitalWrite(STEPPER_STEP, LOW);
  digitalWrite(STEPPER_DIR, LOW);
}

void Stepper::enable(boolean yes) {
  enabled = yes;
  digitalWrite(STEPPER_EN, (uint8_t )(enabled ? LOW : HIGH));
}

void Stepper::setResolution(uint8_t ms1, uint8_t ms2, uint8_t ms3) {
  digitalWrite(STEPPER_MS1, ms1);
  digitalWrite(STEPPER_MS2, ms2);
  digitalWrite(STEPPER_MS3, ms3);
}

void Stepper::changeResolution(int res) {
  if (res >= 16) {
    setResolution(HIGH, HIGH, HIGH);
  } else if (res >= 8) {
    setResolution(HIGH, HIGH, LOW);
  } else if (res >= 4) {
    setResolution(LOW, HIGH, LOW);
  } else if (res >= 2) {
    setResolution(HIGH, LOW, LOW);
  } else {
    setResolution(LOW, LOW, LOW);
  }
}

void Stepper::changeDirection(int dir) {
  direction = dir;
  digitalWrite(STEPPER_DIR, (uint8_t)(direction < 0 ? LOW : HIGH));
}

void Stepper::changeSpeed(int _speed) { speed = constrain(_speed, 10, 10000); }

void Stepper::enableSearch(boolean ok) { search = ok; }

void Stepper::loop() {
  // check last step
  if (lastStep + speed < micros()) {
    // save current time
    lastStep = micros();

    // complete step if stepping
    if (stepping) {
      digitalWrite(STEPPER_STEP, LOW);
      stepping = false;
      return;
    }

    // check if stepper is enabled and direction is not zero
    if (enabled && direction != 0) {
      // begin step
      digitalWrite(STEPPER_STEP, HIGH);
      stepping = true;

      // read sensor
      int sensor = analogRead(STEPPER_POS);

      // check if sensor has changed
      if (sensor != lastReading) {
        // save reading
        lastReading = sensor;

        // send current sensor value
        client->publish("/sensor", String(sensor));

        // check if zero has been reached
        if (search && sensor > 300) {
          // TODO: Make threshold configurable.

          // stop stepper
          changeDirection(0);

          // finish search
          search = false;
        }
      }
    }
  }
}
