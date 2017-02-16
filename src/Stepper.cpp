#include "Stepper.h"

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
  setPower(false);
  setResolution(1);
  setDirection(CW);
  setSpeed(5000);

  // enforce idle mode
  mode = Idle;

  // set default
  digitalWrite(STEPPER_STEP, LOW);

  // subscribe to topics
  client->subscribe("/power");
  client->subscribe("/stop");
  client->subscribe("/resolution");
  client->subscribe("/direction");
  client->subscribe("/speed");
  client->subscribe("/search");
  client->subscribe("/target");
}

void Stepper::handle(String topic, String payload) {
  if (topic.equals("/power")) {
    if(payload.equals("on")) {
      setPower(true);
    } else if(payload.equals("off")) {
      setPower(false);
    }
  } else if (topic.equals("/stop")) {
    mode = Idle;
  } else if (topic.equals("/resolution")) {
    setResolution((int)payload.toInt());
  } else if (topic.equals("/direction")) {
    if (payload.equals("cw")) {
      setDirection(CW);
    } else if (payload.equals("ccw")) {
      setDirection(CCW);
    }
  } else if (topic.equals("/speed")) {
    setSpeed((int)payload.toInt());
  } else if (topic.equals("/search")) {
    setSearch((int)payload.toInt());
  } else if(topic.equals("/target")) {
    setTarget(payload.toDouble());
  }
}

void Stepper::setPower(boolean on) {
  powered = on;

  digitalWrite(STEPPER_EN, (uint8_t)(powered ? LOW : HIGH));
}

void Stepper::_setResolution(uint8_t ms1, uint8_t ms2, uint8_t ms3) {
  digitalWrite(STEPPER_MS1, ms1);
  digitalWrite(STEPPER_MS2, ms2);
  digitalWrite(STEPPER_MS3, ms3);
}

void Stepper::setResolution(int res) {
  if (res >= 16) {
    resolution = 16;
    _setResolution(HIGH, HIGH, HIGH);
  } else if (res >= 8) {
    resolution = 8;
    _setResolution(HIGH, HIGH, LOW);
  } else if (res >= 4) {
    resolution = 4;
    _setResolution(LOW, HIGH, LOW);
  } else if (res >= 2) {
    resolution = 2;
    _setResolution(HIGH, LOW, LOW);
  } else {
    resolution = 1;
    _setResolution(LOW, LOW, LOW);
  }
}

void Stepper::setDirection(Direction dir) {
  direction = dir;
  mode = Continuous;

  digitalWrite(STEPPER_DIR, (uint8_t)(direction == CW ? LOW : HIGH));
}

void Stepper::setSpeed(int _speed) { speed = constrain(_speed, 10, 10000); }

void Stepper::setSearch(int _threshold) { threshold = _threshold; }

void Stepper::setTarget(double _target) {
  target = _target;
  mode = Absolute;
}

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

    // publish status if changed
    if(mode != lastStatus) {
      lastStatus = mode;

      if (mode == Idle) {
        client->publish("/status", "idle");
      } else if (mode == Continuous) {
        client->publish("/status", "continuous");
      } else if (mode == Absolute) {
        client->publish("/status", "absolute");
      }
    }

    // return immediately if not powered or in Idle mode
    if (!powered || mode == Idle) {
      return;
    }

    // return immediately if position has already been reached
    if (mode == Absolute && position == target) {
      return;
    }

    // calculate next move
    double move = 1.0 / 200.0 / resolution;

    // handle continuous mode
    if(mode == Continuous) {
      if(direction == CW) {
        position -= move;
      } else if(direction == CCW) {
        position += move;
      }
    }

    // handle absolute mode
    if (mode == Absolute) {
      // update direction
      setDirection(position < target ? CW : CCW);

      // update position
      if(position > target + move) {
        position -= move;
      } else if(position < target - move) {
        position += move;
      } else {
        position = target;
      }
    }

    // begin step
    digitalWrite(STEPPER_STEP, HIGH);
    stepping = true;

    // read sensor
    int sensor = analogRead(STEPPER_POS);

    // check if sensor has changed
    if (sensor != lastReading) {
      // save reading
      lastReading = sensor;

      // publish sensor value
      client->publish("/sensor", String(sensor));

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

    // publish position
    client->publish("/position", String(position));
  }
}
