#include <naos.h>

#include "net_stepper.h"

naos_config_t config = {.device_type = "NetStepper",
                        .online_callback = net_stepper_setup,
                        .message_callback = net_stepper_handle,
                        .loop_callback = net_stepper_loop,
                        .loop_interval = 0,
                        .offline_callback = net_stepper_terminate,
                        .status_callback = net_stepper_notify};

/**
 * The applications entry point.
 */
void app_main() {
  // initialize net stepper
  net_stepper_init();

  // initialize naos
  naos_init(&config);
}
