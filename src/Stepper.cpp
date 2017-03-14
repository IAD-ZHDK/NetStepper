#include "Stepper.h"
#include "Manager.h"

#define POWER_TOPIC MANAGER_ID "/power"
#define STOP_TOPIC MANAGER_ID "/stop"
#define RESOLUTION_TOPIC MANAGER_ID "/resolution"
#define DIRECTION_TOPIC MANAGER_ID "/direction"
#define SPEED_TOPIC MANAGER_ID "/speed"
#define SEARCH_TOPIC MANAGER_ID "/search"
#define TARGET_TOPIC MANAGER_ID "/target"
#define STATUS_TOPIC MANAGER_ID "/status"
#define SENSOR_TOPIC MANAGER_ID "/sensor"
#define POSITION_TOPIC MANAGER_ID "/position"

void Stepper::writePower(boolean on) {
  powered = on;
  digitalWrite(STEPPER_EN, (uint8_t)(powered ? LOW : HIGH));
}

void Stepper::writeResolution(int res) {
  if (res >= 16) {
    resolution = 16;
    writeResolutionBits(HIGH, HIGH, HIGH);
  } else if (res >= 8) {
    resolution = 8;
    writeResolutionBits(HIGH, HIGH, LOW);
  } else if (res >= 4) {
    resolution = 4;
    writeResolutionBits(LOW, HIGH, LOW);
  } else if (res >= 2) {
    resolution = 2;
    writeResolutionBits(HIGH, LOW, LOW);
  } else {
    resolution = 1;
    writeResolutionBits(LOW, LOW, LOW);
  }
}

void Stepper::writeResolutionBits(uint8_t ms1, uint8_t ms2, uint8_t ms3) {
  digitalWrite(STEPPER_MS1, ms1);
  digitalWrite(STEPPER_MS2, ms2);
  digitalWrite(STEPPER_MS3, ms3);
}

void Stepper::writeDirection(Direction dir) {
  direction = dir;
  digitalWrite(STEPPER_DIR, (uint8_t)(direction == CW ? LOW : HIGH));
}

void Stepper::writeStep(boolean on) {
  stepping = on;
  digitalWrite(STEPPER_STEP, (uint8_t)(on ? HIGH : LOW));
}

int Stepper::readSensor() {
  static int LM[STEPPER_SMOOTHING];
  static uint8_t index = 0;
  static long sum = 0;
  static uint8_t count = 0;

  sum -= LM[index];
  LM[index] = analogRead(STEPPER_POS);
  sum += LM[index];
  index++;
  index = (uint8_t)(index % STEPPER_SMOOTHING);
  if (count < STEPPER_SMOOTHING) count++;

  return (int)(sum / count);
}

void Stepper::setup(MQTTClient *_client) {
  // save client reference
  client = _client;

  // set pin modes
  pinMode(STEPPER_EN, OUTPUT);
  pinMode(STEPPER_MS1, OUTPUT);
  pinMode(STEPPER_MS2, OUTPUT);
  pinMode(STEPPER_MS3, OUTPUT);
  pinMode(STEPPER_STEP, OUTPUT);
  pinMode(STEPPER_DIR, OUTPUT);

  // enforce defaults
  writePower(false);
  writeResolution(1);
  writeDirection(CW);
  writeStep(false);

  // subscribe to topics
  client->subscribe(POWER_TOPIC);
  client->subscribe(STOP_TOPIC);
  client->subscribe(RESOLUTION_TOPIC);
  client->subscribe(DIRECTION_TOPIC);
  client->subscribe(SPEED_TOPIC);
  client->subscribe(SEARCH_TOPIC);
  client->subscribe(TARGET_TOPIC);
}

void Stepper::handle(String topic, String payload) {
  if (topic.equals(POWER_TOPIC)) {
    if (payload.equals("on")) {
      writePower(true);
    } else if (payload.equals("off")) {
      writePower(false);
    }
  } else if (topic.equals(STOP_TOPIC)) {
    mode = Idle;
  } else if (topic.equals(RESOLUTION_TOPIC)) {
    writeResolution((int)payload.toInt());
  } else if (topic.equals(DIRECTION_TOPIC)) {
    mode = Continuous;

    if (payload.equals("cw")) {
      writeDirection(CW);
    } else if (payload.equals("ccw")) {
      writeDirection(CCW);
    }
  } else if (topic.equals(SPEED_TOPIC)) {
    speed = constrain((int)payload.toInt(), 10, 10000);
  } else if (topic.equals(SEARCH_TOPIC)) {
    mode = Absolute;
    threshold = (int)payload.toInt();
  } else if (topic.equals(TARGET_TOPIC)) {
    mode = Absolute;
    target = payload.toDouble();
  }
}

void Stepper::loop() {
  // check last step
  if (lastStep + speed < micros()) {
    // save current time
    lastStep = micros();

    // complete step if stepping
    if (stepping) {
      writeStep(false);
      return;
    }

    // publish status if changed
    if (mode != lastStatus) {
      lastStatus = mode;

      if (mode == Idle) {
        client->publish(STATUS_TOPIC, "idle");
      } else if (mode == Continuous) {
        client->publish(STATUS_TOPIC, "continuous");
      } else if (mode == Absolute) {
        client->publish(STATUS_TOPIC, "absolute");
      }
    }

    // return immediately if not powered or in Idle mode
    if (!powered || mode == Unset || mode == Idle) {
      return;
    }

    // return immediately if position has already been reached
    if (mode == Absolute && position == target) {
      return;
    }

    // calculate next move
    double move = 1.0 / 200.0 / resolution;

    // handle continuous mode
    if (mode == Continuous) {
      if (direction == CW) {
        position -= move;
      } else if (direction == CCW) {
        position += move;
      }
    }

    // handle absolute mode
    if (mode == Absolute) {
      // update direction
      writeDirection(position < target ? CW : CCW);

      // update position
      if (position > target + move) {
        position -= move;
      } else if (position < target - move) {
        position += move;
      } else {
        position = target;
        mode = Idle;
      }
    }

    // begin step
    writeStep(true);

    // read sensor
    int sensor = this->readSensor();

    // check if sensor has changed
    if (sensor != lastReading) {
      // save reading
      lastReading = sensor;

      // publish sensor value
      client->publish(SENSOR_TOPIC, String(sensor));

      // check if zero has been reached
      if (threshold > 0 && sensor > threshold) {
        // reset position
        position = 0;

        // set drive mode to idle
        mode = Idle;

        // finish search
        threshold = 0;
      }
    }

    // publish position if changed
    if (position != lastPosition) {
      lastPosition = position;

      client->publish(POSITION_TOPIC, String(position));
    }
  }
}
