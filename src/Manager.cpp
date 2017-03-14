#include "Manager.h"
#include "Utils.h"

void Manager::connect() {
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  String id = suffix("NetStepper/", MANAGER_ID);

  while (!client.connect(id.c_str(), MANAGER_USERNAME, MANAGER_PASSWORD)) {
    delay(1000);
  }

  stepper.setup(&client);
}

void Manager::setup() {
  WiFi.begin(MANAGER_SSID, MANAGER_PASS);

  client.begin(MANAGER_BROKER, net);

  connect();
}

void Manager::loop() {
  client.loop();

  if (!client.connected()) {
    connect();
  }

  stepper.loop();
}

void Manager::handle(String topic, String payload) { stepper.handle(topic, payload); }
