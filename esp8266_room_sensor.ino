#include <FS.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Declare functions
void formPrint();
void handlePost();
void handleRoot();
void reconnect();
void setupWebserver();
void publishSensorInfo(float temp);
void saveConfigCallback();
void checkMqttConnection();

#define VERSION "1.0"
#define TEMP_TIMEOUT 10000

String web_user = "admin";
String web_pass = "esp8266";

String device_name = "esp";
String mqtt_host;
String mqtt_port = "1883";
String mqtt_user;
String mqtt_pass;
String mqtt_topic = "sensors/room/";

long lastSensorCheck = 0;

ESP8266WebServer server(80);
WiFiManager wifiManager;


OneWire oneWire(D2);
DallasTemperature sensors(&oneWire);

WiFiClient espClient;
PubSubClient mqttClient(espClient);

bool shouldSaveConfig = false;

void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void setup() {
  Serial.begin(115200);
  sensors.begin();

  if (SPIFFS.begin()) {
    Serial.println("Mounted filesystem");
    readConfig();
  } else {
    Serial.println("failed to mound filesystem");
  }

  WiFiManagerParameter custom_device_name("device_name", "device name", device_name.c_str(), 40);
  WiFiManagerParameter custom_mqtt_host("mqtt_host", "mqtt host", mqtt_host.c_str(), 40);
  WiFiManagerParameter custom_mqtt_port("mqtt_port", "mqtt port", mqtt_port.c_str(), 6);
  WiFiManagerParameter custom_mqtt_user("mqtt_user", "mqtt username", mqtt_user.c_str(), 32);
  WiFiManagerParameter custom_mqtt_pass("mqtt_pass", "mqtt password", mqtt_pass.c_str(), 40);

  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.addParameter(&custom_device_name);
  wifiManager.addParameter(&custom_mqtt_host);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_user);
  wifiManager.addParameter(&custom_mqtt_pass);

  wifiManager.autoConnect();

  device_name = custom_device_name.getValue();
  mqtt_host = custom_mqtt_host.getValue();
  mqtt_port = custom_mqtt_port.getValue();
  mqtt_user = custom_mqtt_user.getValue();
  mqtt_pass = custom_mqtt_pass.getValue();

  mqttClient.setServer(mqtt_host.c_str(), mqtt_port.toInt());

  if (shouldSaveConfig) {
    saveConfig();
  }

  setupWebserver();
  ArduinoOTA.begin();

  // Set name to the ip until told otherwise
  if (!device_name) {
    Serial.println("setting default device name");
    device_name = WiFi.localIP().toString();
  }
}

void loop() {
  ArduinoOTA.handle();
  checkMqttConnection();

  // Get temperature every N seconds
  long now = millis();
  if (now - lastSensorCheck > TEMP_TIMEOUT) {
    lastSensorCheck = now;
    sensors.requestTemperatures();
    float temp = sensors.getTempFByIndex(0);
    Serial.print("temp: ");
    Serial.println(temp);
    publishSensorInfo(temp);
  }

  server.handleClient();
}

void publishSensorInfo(float temp) {
  char msg[250];
  sprintf(
    msg,
    "{\"version\": \"%s\", \"device_name\": \"%s\", \"ip\": \"%s\", \"temp\": %.2f}",
    VERSION,
    device_name.c_str(),
    WiFi.localIP().toString().c_str(),
    temp
  );

  mqttClient.publish(String(mqtt_topic + device_name).c_str(), msg);
}

void checkMqttConnection() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (!mqttClient.connect(clientId.c_str(), mqtt_user.c_str(), mqtt_pass.c_str())) {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}