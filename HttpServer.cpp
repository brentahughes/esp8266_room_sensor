#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "HttpServer.h"

HttpServer::HttpServer(ESP8266WebServer webServer, MqttClient mqttClient, String name) {
  server = webServer;
  name = name;
  mClient = MqttClient;
}

void HttpServer::formPrint() {
  server.send(
      200,
      "text/html",
      "<form action=\"/\" method=\"POST\">"
      "  Name: <input type=\"text\" name=\"name\" value=\"" + name + "\"><br><br>"
      "  <h3>MQTT</h3>"
      "  Host: <input type=\"text\" name=\"mqtt_host\" value=\"" + mClient.host + "\"><br>"
      "  User: <input type=\"text\" name=\"mqtt_user\" value=\"" + mClient.user + "\"><br>"
      "  Pass: <input type=\"text\" name=\"mqtt_pass\" value=\"" + mClient.pass + "\"><br><br>"
      "  <input type=\"submit\" value=\"save\">"
      "</form"
    );
}

void HttpServer::handleRoot() {
    formPrint();
}

void HttpServer::handlePost() {
    if (server.arg("name") != NULL) {
      name = server.arg("name");
    }
    formPrint();
}
