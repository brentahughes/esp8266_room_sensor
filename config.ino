void readConfig() {
  if (!SPIFFS.exists("/config.json")) {
    Serial.println("/config.json does not exist");
    return;
  }

  Serial.println("Reading config at /config.json");
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("failed ot read /config.json");
    return;
  }

  Serial.println("Opened config file");

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);

  StaticJsonDocument<1024> json;
  DeserializationError error = deserializeJson(json, buf.get());
  if (error) {
    Serial.println("Failed to read config file");
  }

  device_name = json["device_name"].as<String>();
  mqtt_host = json["mqtt_host"].as<String>();
  mqtt_port = json["mqtt_port"].as<String>();
  mqtt_user = json["mqtt_user"].as<String>();
  mqtt_pass = json["mqtt_pass"].as<String>();
}

void saveConfig() {
  Serial.println("Saving config");

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("failed to open config file for writing");
  }

  StaticJsonDocument<1024> json;
  json["device_name"] = device_name;
  json["mqtt_host"] = mqtt_host;
  json["mqtt_port"] = mqtt_port;
  json["mqtt_user"] = mqtt_user;
  json["mqtt_pass"] = mqtt_pass;

  if (serializeJson(json, configFile) == 0) {
    Serial.println("Failed to write config file");
  }

  configFile.close();
}