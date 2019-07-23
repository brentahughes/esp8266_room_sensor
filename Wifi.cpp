#include "Wifi.h";
#include <Arduino.h>
#include <ESP8266WiFi.h>

Wifi::Wifi(String ssid, String pass) {
    _ssid = ssid;
    _pass = pass;
}

void Wifi::connect() {
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(_ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid, _pass);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("WiFi connected");
    Serial.println(WiFi.localIP());
}