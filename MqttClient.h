#ifndef MqttClient_h
#define MqttClient_h

#include <Arduino.h>

class MqttClient {
public:
    String host;
    String user;
    String pass;
    String topic;
};

#endif