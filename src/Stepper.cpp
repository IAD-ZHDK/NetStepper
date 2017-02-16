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

void Stepper::enable(boolean ok) {
  if (ok) {
    digitalWrite(STEPPER_EN, LOW);
  } else {
    digitalWrite(STEPPER_EN, HIGH);
  }
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

// should be -1, 0 or 1
void Stepper::changeDirection(int dir) {
  // set on off
  on = dir != 0;

  // set direction
  if (dir < 0) {
    digitalWrite(STEPPER_DIR, LOW);
  } else {
    digitalWrite(STEPPER_DIR, HIGH);
  }
}

// should be between 10 and 10000
void Stepper::changeSpeed(int _speed) { speed = constrain(_speed, 10, 10000); }

// should be 1 or 0
void Stepper::enableSearch(boolean ok) { search = ok; }

void Stepper::loop() {
  // check run time
  if (lastMicros + speed < micros()) {
    // save current time
    lastMicros = micros();

    // complete step if stepping
    if (stepping) {
      digitalWrite(STEPPER_STEP, LOW);
      stepping = false;
      return;
    }

    // check if stepper is on
    if (on) {
      // begin step
      digitalWrite(STEPPER_STEP, HIGH);
      stepping = true;

      // read sensor
      int pos = analogRead(STEPPER_POS);

      // check if sensor has changed
      if (pos != lastPos) {
        // send current sensor value
        client->publish("/sensor", String(pos));

        // check if zero has been reached
        if (search && pos > 300) {
          // TODO: Make threshold configurable.

          search = false;
          on = false;
        }
      }
    }
  }
}
