#ifndef HttpServer_h
#define HttpServer_h

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "MqttClient.h"

class HttpServer {
public:
    HttpServer(ESP8266WebServer webServer, MqttClient mqttClient, String name);
    void formPrint();
    void handleRoot();
    void handlePost();
private:
    ESP8266WebServer server;
    MqttClient mClient;
    String name;
};

#endif