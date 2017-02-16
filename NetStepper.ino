#include "src/Manager.h"

Manager manager;

void setup() {
  manager.setup();
}

void loop() {
  manager.loop();
}

void messageReceived(String topic, String payload, char * bytes, unsigned int length) {
  manager.handle(topic, payload);
}
