#include "Manager.h"

void Manager::connect() {
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  while (!client.connect("NetStepper/" MANAGER_ID, MANAGER_USERNAME, MANAGER_PASSWORD)) {
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
