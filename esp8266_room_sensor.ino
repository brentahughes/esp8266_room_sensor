#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <EEPROM.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>
#include <ESP8266WebServer.h>

#define STASSID "Hughes_House"
#define STAPSK  "8172662079"

#define TEMP_TIMEOUT 10000

const char* ssid = STASSID;
const char* password = STAPSK;
String mqtt_host = "192.168.1.10";
String mqtt_user = "bhughes";
String mqtt_pass = "Debian06.";
String mqtt_topic = "sensors/temp/";
String name = "undefined";

ESP8266WebServer server(80);

const int oneWireBus = 2;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

WiFiClient http_client;
PubSubClient mqtt_client(http_client);
void setup() {
  Serial.begin(115200);

  // prepare LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println(F("WiFi connected"));

  // Start the server
  server.on("/", HTTP_GET, handleRoot);
  server.on("/", HTTP_POST, handlePost);
  server.begin();
  Serial.println(F("Server started"));

  // Print the IP address
  Serial.println(WiFi.localIP());

  sensors.begin();

  // Setup mqtt client
  mqtt_client.setServer(mqtt_host.c_str(), 1883);
}

void handleRoot() {
    formPrint();
}

void handlePost() {
    if (server.arg("name") != NULL) {
      name = server.arg("name");
    }

    formPrint();
}

void formPrint() {
  server.send(
      200,
      "text/html",
      "<form action=\"/\" method=\"POST\">"
      "  Name: <input type=\"text\" name=\"name\" value=\"" + name + "\"><br><br>"
      "  <h3>MQTT</h3>"
      "  Host: <input type=\"text\" name=\"mqtt_host\" value=\"" + mqtt_host + "\"><br>"
      "  User: <input type=\"text\" name=\"mqtt_user\" value=\"" + mqtt_user + "\"><br>"
      "  Pass: <input type=\"password\" name=\"mqtt_pass\" value=\"" + mqtt_pass + "\"><br><br>"
      "  <input type=\"submit\" value=\"save\">"
      "</form"
    );
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
