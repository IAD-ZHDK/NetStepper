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
  client.subscribe("/mode");
  client.subscribe("/speed");
  client.subscribe("/resolution");
  client.subscribe("/enabled");
  client.subscribe("/search");
  client.subscribe("/steps");
}

void Manager::setup() {
  WiFi.begin(MANAGER_SSID, MANAGER_PASS);

  client.begin(net);
  client.setHost(MANAGER_BROKER);

  stepper.setup(&client);

  connect();
}

void Manager::loop() {
  client.loop();

  if (!client.connected()) {
    connect();
  }

  stepper.loop();
}

void Manager::handle(String topic, String payload) {
  if (topic.equals("/direction")) {
    if (payload.equals("left")) {
      stepper.setDirection(Left);
    } else if (payload.equals("right")) {
      stepper.setDirection(Right);
    }
  } else if (topic.equals("/mode")) {
    if (payload.equals("idle")) {
      stepper.setDriveMode(Idle);
    } else if (payload.equals("single")) {
      stepper.setDriveMode(Single);
    } else if (payload.equals("continuous")) {
      stepper.setDriveMode(Continuous);
    }
  } else if (topic.equals("/speed")) {
    stepper.setSpeed((int)payload.toInt());
  } else if (topic.equals("/resolution")) {
    stepper.setResolution((int)payload.toInt());
  } else if (topic.equals("/enabled")) {
    stepper.setEnabled(payload.toInt() == 1);
  } else if (topic.equals("/search")) {
    stepper.setSearch(payload.toInt() == 1);
  } else if (topic.equals("/steps")) {
    stepper.setSteps((int)payload.toInt());
  }
}
