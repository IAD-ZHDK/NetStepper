#include <driver/adc.h>

#define SENSORS_CHANNEL_1 ADC1_CHANNEL_7
#define SENSORS_CHANNEL_2 ADC1_CHANNEL_6

#define SENSORS_HISTOGRAM_SIZE 10

typedef struct {
  int values[SENSORS_HISTOGRAM_SIZE];
  uint8_t index;
  long sum;
  uint8_t count;
} sensors_histogram_t;

sensors_histogram_t sensors_histogram_1 = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, 0, 0};
sensors_histogram_t sensors_histogram_2 = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, 0, 0};

void sensors_init() {
  // set adc resolution
  adc1_config_width(ADC_WIDTH_12Bit);
}

static int sensors_read(sensors_histogram_t *adc, adc1_channel_t ch) {
  // remove last value from sum
  adc->sum -= adc->values[adc->index];

  // read next value
  adc->values[adc->index] = adc1_get_voltage(ch);

  // add read value to sum
  adc->sum += adc->values[adc->index];

  // advance index
  adc->index++;

  // handle index overflow
  adc->index = (uint8_t)(adc->index % SENSORS_HISTOGRAM_SIZE);

  // increase count at beginning
  if (adc->count < SENSORS_HISTOGRAM_SIZE) {
    adc->count++;
  }

  // return mean value
  return (int)(adc->sum / adc->count);
}

int sensors_read_1() { return sensors_read(&sensors_histogram_1, SENSORS_CHANNEL_1); }

int sensors_read_2() { return sensors_read(&sensors_histogram_2, SENSORS_CHANNEL_2); }
