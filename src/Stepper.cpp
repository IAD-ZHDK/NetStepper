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
  setDriveMode(Idle);
  setResolution(1);
  setDirection(CW);
  setSpeed(5000);

  // set default
  digitalWrite(STEPPER_STEP, LOW);

  // subscribe to topics
  client->subscribe("/powered");
  client->subscribe("/mode");
  client->subscribe("/resolution");
  client->subscribe("/direction");
  client->subscribe("/speed");
  client->subscribe("/search");
  client->subscribe("/target");
}

void Stepper::handle(String topic, String payload) {
  if (topic.equals("/powered")) {
    if(payload.equals("on")) {
      setPower(true);
    } else if(payload.equals("off")) {
      setPower(false);
    }
  } else if (topic.equals("/mode")) {
    if (payload.equals("idle")) {
      setDriveMode(Idle);
    } else if(payload.equals("absolute")) {
      setDriveMode(Absolute);
    } else if (payload.equals("continuous")) {
      setDriveMode(Continuous);
    }
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

void Stepper::setDriveMode(DriveMode _mode) { mode = _mode; }

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

  digitalWrite(STEPPER_DIR, (uint8_t)(direction == CW ? LOW : HIGH));
}

void Stepper::setSpeed(int _speed) { speed = constrain(_speed, 10, 10000); }

void Stepper::setSearch(int _threshold) { threshold = _threshold; }

void Stepper::setTarget(double _target) {
  target = _target;
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

    // return immediately if not powered or in Idle mode
    if (!powered || mode == Idle) {
      return;
    }

    // make one step if stepper is in continuous mode
    if (mode == Continuous) {
      // begin step
      digitalWrite(STEPPER_STEP, HIGH);
      stepping = true;
    }

    // check if in absolute mode
    if (mode == Absolute) {
      // calculate current mode
      double move = 1.0 / 200.0 / resolution;

      // check if we are still away from the target
      if(position < target - move || position > target + move) {
        // update direction
        setDirection(position < target ? CW : CCW);

        // update position
        if(position > target) {
          position -= move;
        } else {
          position += move;
        }

        // begin step
        digitalWrite(STEPPER_STEP, HIGH);
        stepping = true;

        // update position
        client->publish("/position", String(position));
      }
    }

    // check if a step has been made
    if (stepping) {
      // read sensor
      int sensor = analogRead(STEPPER_POS);

      // check if sensor has changed
      if (sensor != lastReading) {
        // save reading
        lastReading = sensor;

        // send current sensor value
        client->publish("/sensor", String(sensor));

        // check if zero has been reached
        if (threshold > 0 && sensor > threshold) {
          // reset position
          position = 0;

          // set drive mode to idle
          setDriveMode(Idle);

          // finish search
          threshold = 0;
        }
      }
    }
  }
}
