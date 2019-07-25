#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include "Wifi.h"
#include "TempSensor.h"
#include "Mqtt.h"

// Declare functions
void formPrint();
void handlePost();
void handleRoot();
void reconnect();
void setupWebserver();
void publishSensorInfo(float temp);

String name;

String ssid = "Hughes_House";
String password = "8172662079";

String web_user = "admin";
String web_pass = "esp8266";

String mqtt_host = "192.168.1.10";
String mqtt_user = "bhughes";
String mqtt_pass = "Debian06.";
String mqtt_topic = "sensors/temp/";


ESP8266WebServer server(80);
Wifi wifi(ssid, password);
TempSensor tempSensor(2);
Mqtt mqtt(mqtt_host, mqtt_user, mqtt_pass);

void setup() {
  Serial.begin(115200);
  wifi.connect();
  setupWebserver();
  ArduinoOTA.begin();

  // Set name to the ip until told otherwise
  name = WiFi.localIP().toString();
}

void loop() {
  ArduinoOTA.handle();
  mqtt.checkConnection();

  // Get temperature
  float temp = tempSensor.read();
  publishSensorInfo(temp);

  server.handleClient();
}

void publishSensorInfo(float temp) {
  char msg[250];
  sprintf(
    msg,
    "{\"name\": \"%s\", \"ip\": \"%s\", \"temp\": %.2f}",
    name.c_str(),
    WiFi.localIP().toString().c_str(),
    temp
  );
  mqtt.publish(String(mqtt_topic + name), msg);
}