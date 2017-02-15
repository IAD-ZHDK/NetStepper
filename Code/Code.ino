#include <SPI.h>
#include <WiFi101.h>
#include <MQTTClient.h>

char *ssid = "BRIDGE";
char *pass = "internet";

WiFiClient net;
MQTTClient client;

void setup() {
  stepperSetup();
  
  Serial.begin(9600);
  WiFi.begin(ssid, pass);
  client.begin(net);
  client.setHost("broker.shiftr.io");

  connect();
}

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("arduino", "puppentanz", "puppentanz")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("/direction");
  client.subscribe("/speed");
  client.subscribe("/resolution");
  client.subscribe("/enabled");
  client.subscribe("/search");
}

void loop() {
  client.loop();

  if(!client.connected()) {
    connect();
  }

  stepperLoop();
}

void messageReceived(String topic, String payload, char * bytes, unsigned int length) {
  if(topic.equals("/direction")) {
    stepperSetDir(payload.toInt());
  } else if(topic.equals("/speed")) {
    stepperSetSpeed(payload.toInt());
  } else if(topic.equals("/resolution")) {
    stepperSetResolution(payload.toInt());
  } else if(topic.equals("/enabled")) {
    stepperSetEnabled(payload.toInt());
  } else if(topic.equals("/search")) {
    stepperSetSearch(payload.toInt());
  }
}

