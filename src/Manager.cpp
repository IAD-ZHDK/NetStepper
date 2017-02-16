#include "Manager.h"

void Manager::connect() {
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  while (
      !client.connect(MANAGER_CLIENT_ID, MANAGER_USERNAME, MANAGER_PASSWORD)) {
    delay(1000);
  }

  client.subscribe("/direction");
  client.subscribe("/speed");
  client.subscribe("/resolution");
  client.subscribe("/enabled");
  client.subscribe("/search");
}

void Manager::setup() {
  WiFi.begin(MANAGER_SSID, MANAGER_PASS);

  client.begin(net);
  client.setHost(MANAGER_BROKER);

  stepper.setup();

  connect();
}

void Manager::loop() {
  client.loop();

  if (!client.connected()) {
    connect();
  }

  stepper.loop();
}

void Manager::messageReceived(String topic, String payload, char *bytes,
                              unsigned int length) {
  if (topic.equals("/direction")) {
    stepper.changeDirection((int)payload.toInt());
  } else if (topic.equals("/speed")) {
    stepper.changeSpeed((int)payload.toInt());
  } else if (topic.equals("/resolution")) {
    stepper.changeResolution((int)payload.toInt());
  } else if (topic.equals("/enabled")) {
    stepper.enable(payload.toInt() == 1);
  } else if (topic.equals("/search")) {
    stepper.enableSearch(payload.toInt() == 1);
  }
}
