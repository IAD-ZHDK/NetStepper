#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <naos.h>

#include "led.h"
#include "sensors.h"
#include "stepper_driver.h"

typedef enum { NET_STEPPER_IDLE, NET_STEPPER_ABSOLUTE, NET_STEPPER_CONTINUOUS } net_stepper_mode_t;

static struct {
  bool powered;
  int resolution;
  int frequency;
  net_stepper_mode_t mode;
  bool direction_cw;
  int threshold;
  double target;
  double position;
  bool verbose;
  bool send_sensor;
  uint32_t last_loop;
} net_stepper_state = {.powered = false,
                       .resolution = 1,
                       .frequency = 1,
                       .mode = NET_STEPPER_IDLE,
                       .threshold = 0,
                       .target = 0.0,
                       .position = 0.0,
                       .send_sensor = false,
                       .verbose = false,
                       .last_loop = 0};

void net_stepper_init() {
  // initialize stepper drive
  stepper_driver_init();

  // initialize sensors
  sensors_init();

  // initialize led
  led_init();
}

void net_stepper_setup() {
  // subscribe to topics
  naos_subscribe("power", 0, NAOS_LOCAL);
  naos_subscribe("resolution", 0, NAOS_LOCAL);
  naos_subscribe("frequency", 0, NAOS_LOCAL);
  naos_subscribe("threshold", 0, NAOS_LOCAL);
  naos_subscribe("drive", 0, NAOS_LOCAL);
  naos_subscribe("move", 0, NAOS_LOCAL);
  naos_subscribe("move-quiet", 0, NAOS_LOCAL);
  naos_subscribe("stop", 0, NAOS_LOCAL);
  naos_subscribe("reset", 0, NAOS_LOCAL);
  naos_subscribe("read", 0, NAOS_LOCAL);

  // always start in a powered off and idle state
  net_stepper_state.powered = false;
  net_stepper_state.mode = NET_STEPPER_IDLE;
}

void net_stepper_handle(const char* topic, uint8_t* payload, unsigned int len, naos_scope_t scope) {
  char* str = (char*)payload;

  // handle "power" command
  if (strcmp(topic, "power") == 0) {
    if (strcmp(str, "on") == 0) {
      net_stepper_state.powered = true;
      net_stepper_state.mode = NET_STEPPER_IDLE;
    } else if (strcmp(str, "off") == 0) {
      net_stepper_state.powered = false;
      net_stepper_state.mode = NET_STEPPER_IDLE;
    }

    return;
  }

  // handle "resolution" command
  if (strcmp(topic, "resolution") == 0) {
    net_stepper_state.resolution = atoi(str);

    return;
  }

  // handle "frequency" command
  if (strcmp(topic, "frequency") == 0) {
    net_stepper_state.frequency = (int)atol(str);

    return;
  }

  // handle "threshold" command
  if (strcmp(topic, "threshold") == 0) {
    net_stepper_state.threshold = atoi(str);

    return;
  }

  // handle "drive" command
  if (strcmp(topic, "drive") == 0) {
    if (strcmp(str, "cw") == 0) {
      net_stepper_state.direction_cw = true;
      net_stepper_state.mode = NET_STEPPER_CONTINUOUS;
    } else if (strcmp(str, "ccw") == 0) {
      net_stepper_state.direction_cw = false;
      net_stepper_state.mode = NET_STEPPER_CONTINUOUS;
    }

    return;
  }

  // handle "move" command
  if (strcmp(topic, "move") == 0) {
    net_stepper_state.target = atof(str);
    net_stepper_state.verbose = true;
    net_stepper_state.mode = NET_STEPPER_ABSOLUTE;

    return;
  }

  // handle "move-quiet" command
  if (strcmp(topic, "move-quiet") == 0) {
    net_stepper_state.target = atof(str);
    net_stepper_state.verbose = false;
    net_stepper_state.mode = NET_STEPPER_ABSOLUTE;

    return;
  }

  // handle "stop" command
  if (strcmp(topic, "stop") == 0) {
    net_stepper_state.mode = NET_STEPPER_IDLE;

    return;
  }

  // handle "reset" command
  if (strcmp(topic, "reset") == 0) {
    net_stepper_state.position = 0;
    net_stepper_state.mode = NET_STEPPER_IDLE;

    return;
  }

  // handle "read" command
  if (strcmp(topic, "read") == 0) {
    net_stepper_state.send_sensor = true;

    return;
  }
}

void net_stepper_notify(naos_status_t status) {
  switch (status) {
    case NAOS_DISCONNECTED:
      led_set(false, false);
      break;
    case NAOS_CONNECTED:
      led_set(true, false);
      break;
    case NAOS_NETWORKED:
      led_set(false, true);
      break;
  }
}

void net_stepper_loop() {
  // read sensor
  int sensor = sensors_read_1();

  // check for read requests
  if (net_stepper_state.send_sensor) {
    // publish last read sensor value
    char sensor_string[10];
    snprintf(sensor_string, 10, "%d", sensor);
    naos_publish("sensor", sensor_string, 0, false, NAOS_LOCAL);

    // reset flag
    net_stepper_state.send_sensor = false;
  }

  // check if zero has been reached
  if (net_stepper_state.mode == NET_STEPPER_CONTINUOUS &&
      ((net_stepper_state.threshold > 0 && sensor > net_stepper_state.threshold) ||
       (net_stepper_state.threshold < 0 && sensor < abs(net_stepper_state.threshold)))) {
    // turn off and go to idle state
    net_stepper_state.mode = NET_STEPPER_IDLE;

    // reset position & threshold
    net_stepper_state.position = 0;
    net_stepper_state.threshold = 0;

    // publish reached event
    naos_publish("reached", "0.000", 0, false, NAOS_LOCAL);
  }

  // get current time
  uint32_t now = naos_millis();

  // calculate time difference and set last check
  uint32_t diff = now - net_stepper_state.last_loop;
  net_stepper_state.last_loop = now;

  // calculate steps and move
  double step = 1 / 200.0 / stepper_driver_get_resolution();
  double steps = diff / 1000.0 * stepper_driver_get_frequency();
  double move = steps * step;

  // update position if not idle and powered
  if (net_stepper_state.mode != NET_STEPPER_IDLE && stepper_driver_is_on()) {
    if (stepper_driver_is_direction_cw()) {
      net_stepper_state.position += move;
    } else {
      net_stepper_state.position -= move;
    }
  }

  // handle absolute mode
  if (net_stepper_state.mode == NET_STEPPER_ABSOLUTE) {
    // check if we have reached our target position
    if (net_stepper_state.position > net_stepper_state.target - step &&
        net_stepper_state.position < net_stepper_state.target + step) {
      // set idle state
      net_stepper_state.mode = NET_STEPPER_IDLE;

      // set precise position
      net_stepper_state.position = net_stepper_state.target;

      // send reached event if verbose
      if (net_stepper_state.verbose) {
        char position_str[10];
        snprintf(position_str, 10, "%.3f", net_stepper_state.position);
        naos_publish("reached", position_str, 0, false, NAOS_LOCAL);
      }
    } else {
      // otherwise adjust direction
      net_stepper_state.direction_cw = net_stepper_state.position < net_stepper_state.target;
    }
  }

  // set power if changed
  if (net_stepper_state.powered != stepper_driver_is_on()) {
    stepper_driver_set_power(net_stepper_state.powered);
  }

  // set resolution if changed
  if (net_stepper_state.resolution != stepper_driver_get_resolution()) {
    stepper_driver_set_resolution(net_stepper_state.resolution);
  }

  // set frequency if changed
  if (net_stepper_state.frequency != stepper_driver_get_frequency()) {
    stepper_driver_set_frequency(net_stepper_state.frequency);
  }

  // set direction if changed
  if (net_stepper_state.direction_cw != stepper_driver_is_direction_cw()) {
    stepper_driver_set_direction_cw(net_stepper_state.direction_cw);
  }

  // turn motor on or off depending on state
  if (net_stepper_state.mode == NET_STEPPER_IDLE && stepper_driver_is_turning()) {
    stepper_driver_set_motor(false);
  } else if (net_stepper_state.mode != NET_STEPPER_IDLE && !stepper_driver_is_turning()) {
    stepper_driver_set_motor(true);
  }
}

void net_stepper_terminate() {
  // power of stepper driver
  stepper_driver_set_power(false);
}
