#ifndef HttpServer_h
#define HttpServer_h

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "MqttClient.h"

class HttpServer {
public:
    HttpServer(int port, MqttClient mqttClient, String name);
    void setupRoutes();
    void handleClient();
    void begin();

private:
    ESP8266WebServer server;
    MqttClient mClient;
    String name;

    void formPrint();
    void handleRoot();
    void handlePost();
};

#endif