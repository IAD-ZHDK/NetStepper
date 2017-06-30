#ifndef STEPPER_DRIVER_H
#define STEPPER_DRIVER_H

#include <stdbool.h>

void stepper_driver_init();

bool stepper_driver_is_on();

void stepper_driver_set_power(bool on);

bool stepper_driver_is_turning();

void stepper_driver_set_motor(bool turning);

int stepper_driver_get_resolution();

void stepper_driver_set_resolution(int res);

bool stepper_driver_is_direction_cw();

void stepper_driver_set_direction_cw(bool cw);

int stepper_driver_get_frequency();

void stepper_driver_set_frequency(int freq);

#endif  // STEPPER_DRIVER_H
