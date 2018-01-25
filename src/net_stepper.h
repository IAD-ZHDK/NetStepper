#ifndef NET_STEPPER_H
#define NET_STEPPER_H

void net_stepper_init();

void net_stepper_setup();

void net_stepper_handle(const char* topic, uint8_t* payload, unsigned int len, naos_scope_t scope);

void net_stepper_notify(naos_status_t status);

void net_stepper_loop();

void net_stepper_terminate();

#endif  // NET_STEPPER_H
