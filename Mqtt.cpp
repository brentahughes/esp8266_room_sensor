#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include "Mqtt.h"

Mqtt::Mqtt(String host, String user, String pass) {
    _host = host;
    _user = user;
    _pass = pass;

    WiFiClient http_client;
    PubSubClient client(http_client);
    _client = client;

    _client.setServer(_host.c_str(), 1883);
}

void Mqtt::checkConnection() {
    while (!_client.connected()) {
        String clientId = "esp" + String(WiFi.localIP()[0]) +  String(WiFi.localIP()[1]) + String(WiFi.localIP()[2]) + String(WiFi.localIP()[3]);
        Serial.println(String("connecting to mqtt as ") + clientId);

        if (_client.connect(clientId.c_str(), _user.c_str(), _pass.c_str())) {
            Serial.println("connected to mqtt");
        } else {
            Serial.print("mqtt failed: ");
            Serial.println(_client.state());
            delay(5000);
        }
    }
}

void Mqtt::publish(String topic, String msg) {
    _client.publish(topic.c_str(), msg.c_str());
}