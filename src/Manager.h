#include <MQTTClient.h>
#include <SPI.h>
#include <WiFi101.h>

#include "Stepper.h"

#ifndef NETSTEPPER_MANAGER_H
#define NETSTEPPER_MANAGER_H

#define MANAGER_SSID "BRIDGE"
#define MANAGER_PASS "internet"

#define MANAGER_BROKER "broker.shiftr.io"
#define MANAGER_USERNAME "try"
#define MANAGER_PASSWORD "try"

#define MANAGER_ID "1"

class Manager {
  WiFiClient net;
  MQTTClient client;
  Stepper stepper;
  void connect();

 public:
  void setup();
  void loop();
  void handle(String, String);
};

#endif  // NETSTEPPER_MANAGER_H
