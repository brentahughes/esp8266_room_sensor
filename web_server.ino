void setupWebserver() {
    // Start the server
    server.on("/", HTTP_GET, handleRoot);
    server.on("/", HTTP_POST, handlePost);
    server.on("/factory_reset", HTTP_GET, handleReset);
    server.begin();
    Serial.println("Server started");
}

bool isAuthorized() {
    if (!server.authenticate(web_user.c_str(), web_pass.c_str())) {
        return false;
    }

    return true;
}

void handleReset() {
    if (!isAuthorized()) {
        return server.requestAuthentication();
    }

    // Add configmration page with post that actually does the reset


    server.send(
        200,
        "text/html",
        "<h1>Attention</h1>"
        "Connect to wifi '" + wifiManager.getConfigPortalSSID() + "' to setup wifi"
    );

    wifiManager.resetSettings();
    ESP.reset();
}

void handleRoot() {
    if (!isAuthorized()) {
        return server.requestAuthentication();
    }

    formPrint();
}

void handlePost() {
    if (!isAuthorized()) {
        return server.requestAuthentication();
    }

    if (server.arg("device_name") != NULL) {
        device_name = server.arg("device_name");
    }
    if (server.arg("mqtt_host") != NULL) {
        mqtt_host = server.arg("mqtt_host");
    }
    if (server.arg("mqtt_port") != NULL) {
        mqtt_port = server.arg("mqtt_port");
    }
    if (server.arg("mqtt_user") != NULL) {
        mqtt_user = server.arg("mqtt_user");
    }
    if (server.arg("mqtt_pass") != NULL) {
        mqtt_pass = server.arg("mqtt_pass");
    }
    saveConfig();

    ESP.reset();
}

void formPrint() {
    server.send(
        200,
        "text/html",
        "<form action=\"/\" method=\"POST\">"
        "  <h2>Basic Settings</h2>"
        "  Name: <input type=\"text\" name=\"device_name\" value=\"" + device_name + "\">"
        "  <br><br><hr>"
        "  <h3>MQTT</h3>"
        "  Host: <input type=\"text\" name=\"mqtt_host\" value=\"" + mqtt_host + "\"><br>"
        "  Host: <input type=\"text\" name=\"mqtt_port\" value=\"" + mqtt_port + "\"><br>"
        "  User: <input type=\"text\" name=\"mqtt_user\" value=\"" + mqtt_user + "\"><br>"
        "  Pass: <input type=\"password\" name=\"mqtt_pass\" value=\"" + mqtt_pass + "\"><br><br>"
        "  <input type=\"submit\" value=\"save\">"
        "</form"
    );
}