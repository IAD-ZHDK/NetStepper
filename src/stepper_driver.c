#include <driver/gpio.h>
#include <driver/ledc.h>

#include "stepper_driver.h"

#define STEPPER_DRIVER_EN_BIT GPIO_SEL_12
#define STEPPER_DRIVER_MS1_BIT GPIO_SEL_14
#define STEPPER_DRIVER_MS2_BIT GPIO_SEL_27
#define STEPPER_DRIVER_MS3_BIT GPIO_SEL_26
#define STEPPER_DRIVER_DIR_BIT GPIO_SEL_33

#define STEPPER_DRIVER_EN_NUM GPIO_NUM_12
#define STEPPER_DRIVER_MS1_NUM GPIO_NUM_14
#define STEPPER_DRIVER_MS2_NUM GPIO_NUM_27
#define STEPPER_DRIVER_MS3_NUM GPIO_NUM_26
#define STEPPER_DRIVER_DIR_NUM GPIO_NUM_33

#define STEPPER_DRIVER_STEP_PIN 25

struct {
  bool on;
  bool turning;
  int resolution;
  int frequency;
  bool direction_cw;
} stepper_driver_data;

void stepper_driver_init() {
  // prepare config
  gpio_config_t config = {.pull_down_en = GPIO_PULLDOWN_DISABLE,
                          .pull_up_en = GPIO_PULLUP_DISABLE,
                          .intr_type = GPIO_INTR_DISABLE,
                          .mode = GPIO_MODE_OUTPUT,
                          .pin_bit_mask = STEPPER_DRIVER_EN_BIT | STEPPER_DRIVER_MS1_BIT | STEPPER_DRIVER_MS2_BIT |
                                          STEPPER_DRIVER_MS3_BIT | STEPPER_DRIVER_DIR_BIT};

  // set config
  ESP_ERROR_CHECK(gpio_config(&config));

  // prepare ledc timer config
  ledc_timer_config_t t = {
      .bit_num = LEDC_TIMER_10_BIT, .freq_hz = 1, .speed_mode = LEDC_HIGH_SPEED_MODE, .timer_num = LEDC_TIMER_0};

  // configure ledc timer
  ESP_ERROR_CHECK(ledc_timer_config(&t));

  // prepare ledc channel config
  ledc_channel_config_t c = {.duty = 0,
                             .intr_type = LEDC_INTR_DISABLE,
                             .speed_mode = LEDC_HIGH_SPEED_MODE,
                             .timer_sel = LEDC_TIMER_0,
                             .gpio_num = STEPPER_DRIVER_STEP_PIN,
                             .channel = LEDC_CHANNEL_0};

  // configure step
  ESP_ERROR_CHECK(ledc_channel_config(&c));

  // set default values
  stepper_driver_set_power(false);
  stepper_driver_set_direction_cw(true);
  stepper_driver_set_resolution(1);
  stepper_driver_set_motor(false);
  stepper_driver_set_frequency(0);

  // TODO: Enable Interrupt to count steps?
}

bool stepper_driver_is_on() { return stepper_driver_data.on; }

void stepper_driver_set_power(bool on) {
  stepper_driver_data.on = on;
  ESP_ERROR_CHECK(gpio_set_level(STEPPER_DRIVER_EN_NUM, (uint8_t)(on ? 0 : 1)))
}

bool stepper_driver_is_turning() { return stepper_driver_data.turning; }

void stepper_driver_set_motor(bool turning) {
  // save setting
  stepper_driver_data.turning = turning;

  // set pwm duty
  if (turning) {
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 255));
  } else {
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0));
  }

  // update ledc
  ESP_ERROR_CHECK(ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0));
}

bool stepper_driver_is_direction_cw() { return stepper_driver_data.direction_cw; }

void stepper_driver_set_direction_cw(bool cw) {
  stepper_driver_data.direction_cw = cw;
  ESP_ERROR_CHECK(gpio_set_level(STEPPER_DRIVER_DIR_NUM, (uint8_t)(cw ? 0 : 1)));
}

static void stepper_driver_set_resolution_bits(uint8_t ms1, uint8_t ms2, uint8_t ms3) {
  ESP_ERROR_CHECK(gpio_set_level(STEPPER_DRIVER_MS1_NUM, ms1));
  ESP_ERROR_CHECK(gpio_set_level(STEPPER_DRIVER_MS2_NUM, ms2));
  ESP_ERROR_CHECK(gpio_set_level(STEPPER_DRIVER_MS3_NUM, ms3));
}

int stepper_driver_get_resolution() { return stepper_driver_data.resolution; }

void stepper_driver_set_resolution(int res) {
  if (res >= 16) {
    stepper_driver_data.resolution = 16;
    stepper_driver_set_resolution_bits(1, 1, 1);
  } else if (res >= 8) {
    stepper_driver_data.resolution = 8;
    stepper_driver_set_resolution_bits(1, 1, 0);
  } else if (res >= 4) {
    stepper_driver_data.resolution = 4;
    stepper_driver_set_resolution_bits(0, 1, 0);
  } else if (res >= 2) {
    stepper_driver_data.resolution = 2;
    stepper_driver_set_resolution_bits(1, 0, 0);
  } else {
    stepper_driver_data.resolution = 1;
    stepper_driver_set_resolution_bits(0, 0, 0);
  }
}

int stepper_driver_get_frequency() { return stepper_driver_data.frequency; }

void stepper_driver_set_frequency(int freq) {
  // check upper bound
  if (freq > 900) {
    freq = 900;
  }

  // check lower bound
  if (freq < 1) {
    freq = 1;
  }

  // save frequency
  stepper_driver_data.frequency = freq;

  // set frequency
  ESP_ERROR_CHECK(ledc_set_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0, (unsigned int)freq));
}
