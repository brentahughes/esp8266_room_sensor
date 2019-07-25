void setupWebserver() {
    // Start the server
    server.on("/", HTTP_GET, handleRoot);
    server.on("/", HTTP_POST, handlePost);
    server.begin();
    Serial.println("Server started");
}

bool isAuthorized() {
    if (!server.authenticate(web_user.c_str(), web_pass.c_str())) {
        return false;
    }

    return true;
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
        "  <h2>Basic Settings</h2>"
        "  Name: <input type=\"text\" name=\"name\" value=\"" + name + "\">"
        "  <br><br><hr>"
        "  <h3>MQTT</h3>"
        "  Host: <input type=\"text\" name=\"mqtt_host\" value=\"" + mqtt_host + "\"><br>"
        "  User: <input type=\"text\" name=\"mqtt_user\" value=\"" + mqtt_user + "\"><br>"
        "  Pass: <input type=\"password\" name=\"mqtt_pass\" value=\"" + mqtt_pass + "\"><br><br>"
        "  <input type=\"submit\" value=\"save\">"
        "</form"
    );
}