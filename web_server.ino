void setupWebserver() {
    // Start the server
    server.on("/", HTTP_GET, handleRoot);
    server.on("/", HTTP_POST, handlePost);
    server.begin();
    Serial.println("Server started");
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