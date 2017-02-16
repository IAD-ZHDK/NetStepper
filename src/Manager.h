#include <SPI.h>
#include <WiFi101.h>
#include <MQTTClient.h>

#include "Stepper.h"

#ifndef NETSTEPPER_MANAGER_H
#define NETSTEPPER_MANAGER_H

#define MANAGER_SSID "BRIDGE"
#define MANAGER_PASS "internet"

#define MANAGER_BROKER "broker.shiftr.io"
#define MANAER_CLIENT_ID "arduino"
#define MANAGER_USERNAME "puppentanz"
#define MANAGER_PASSWORD "puppentanz"

class Manager {
  WiFiClient net;
  MQTTClient client;
  Stepper stepper;
  void connect();
public:
  void setup();
  void loop();
  void messageReceived(String, String, char *, unsigned int);
};

#endif //NETSTEPPER_MANAGER_H
