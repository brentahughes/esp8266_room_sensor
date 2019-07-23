#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <EEPROM.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>

#include "HttpServer.h"
#include "MqttClient.h"

const int TEMP_TIMEOUT = 10000;
const String DEFAULT_MQTT_TOPIC = "sensors/temp/";

String ssid;
String password;
String mqtt_host;
String mqtt_topic;
String mqtt_user;
String mqtt_pass;
String name;

// Create esp web server
MqttClient mClient;
HttpServer server(80, mClient, name);

// Create connection to temp sensor
const int oneWireBus = 4;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

// Create MQTT client
WiFiClient http_client;
PubSubClient mqtt_client(http_client);

void setup() {
  Serial.begin(115200);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to " + ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println(F("WiFi connected"));

  // Start the server
  server.setupRoutes();
  server.begin();
  Serial.println(F("Server started"));

  // Print the IP address
  Serial.println(WiFi.localIP());

  sensors.begin();

  // Setup mqtt client
  mqtt_client.setServer(mqtt_host.c_str(), 1883);
}

void reconnect() {
  while (!mqtt_client.connected()) {
    String clientId = "esp" + String(WiFi.localIP()[0]) +  String(WiFi.localIP()[1]) + String(WiFi.localIP()[2]) + String(WiFi.localIP()[3]);
    Serial.println(String("connecting to mqtt as") + clientId);

    if (mqtt_client.connect(clientId.c_str(), mqtt_user.c_str(), mqtt_pass.c_str())) {
      Serial.println("connected to mqtt");
    } else {
      Serial.print("mqtt failed: ");
      Serial.println(mqtt_client.state());
      delay(5000);
    }
  }
}

float readTemp() {
  if (millis() % TEMP_TIMEOUT == 0) {
    sensors.requestTemperatures();
    float temp = sensors.getTempFByIndex(0);

    Serial.print("Temp: ");
    Serial.println(temp);

    char msg[100];
    sprintf(msg, "{\"name\": \"%s\", \"ip\": \"%s\", \"temp\": %.2f}", name.c_str(), WiFi.localIP().toString().c_str(), temp);
    mqtt_client.publish(String(mqtt_topic + name).c_str(), msg);
  }
}

void loop() {
  if (!mqtt_client.connected()) {
    reconnect();
  }
  mqtt_client.loop();

  readTemp();
  server.handleClient();
}
