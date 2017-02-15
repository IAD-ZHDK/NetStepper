#define STEPPER_EN 5
#define STEPPER_MS1 4
#define STEPPER_MS2 3
#define STEPPER_MS3 2
#define STEPPER_STEP 1
#define STEPPER_DIR 0
#define STEPPER_POS A0

unsigned long stepperLastMicros = 0;
boolean stepperStepping = false;
boolean stepperOn = false;
int stepperSpeed = 5000;
boolean stepperSearch = false;

void stepperSetup() {
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

void stepperSetEnabled(boolean ok) {
  if(ok) {
    digitalWrite(STEPPER_EN, LOW);
  } else {
    digitalWrite(STEPPER_EN, HIGH);
  }
}

void stepperSetResolution(int res) {
  if(res >= 16) {
    _stepperSetResolution(HIGH, HIGH, HIGH);
  } else if(res >= 8) {
    _stepperSetResolution(HIGH, HIGH, LOW);
  } else if(res >= 4) {
    _stepperSetResolution(LOW, HIGH, LOW);
  } else if(res >= 2) {
    _stepperSetResolution(HIGH, LOW, LOW);
  } else {
    _stepperSetResolution(LOW, LOW, LOW);
  }
}

void _stepperSetResolution(int ms1, int ms2, int ms3) {
  digitalWrite(STEPPER_MS1, ms1);
  digitalWrite(STEPPER_MS2, ms2);
  digitalWrite(STEPPER_MS3, ms3);
}

// should be -1, 0 or 1
void stepperSetDir(int dir) {
  // set on off
  stepperOn = dir != 0;

  // set direction
  if (dir < 0) {
    digitalWrite(STEPPER_DIR, LOW);
  } else {
    digitalWrite(STEPPER_DIR, HIGH);
  }
}

// should be between 10 and 10000
void stepperSetSpeed(int speed) {
  stepperSpeed = constrain(speed, 10, 10000);
}

// should be 1 or 0
void stepperSetSearch(int srch) {
  stepperSearch = srch == 1;
}

void stepperLoop() {
  // check run time
  if(stepperLastMicros + stepperSpeed < micros()) {
    // save current time
    stepperLastMicros = micros();

    // complete step if stepping
    if(stepperStepping) {
      digitalWrite(STEPPER_STEP, LOW);
      stepperStepping = false;
      return;
    }

    // begin step
    if(stepperOn) {
      digitalWrite(STEPPER_STEP, HIGH);
      stepperStepping = true;

      // check if zero has been reached
      if(stepperSearch && analogRead(A1) > 300) {
        stepperSearch = false;
        stepperOn = false;   
      }
    }
  }
}

