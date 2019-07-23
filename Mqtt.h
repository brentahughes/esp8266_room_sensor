#ifndef Mqtt_h
#define Mqtt_h

#include <Arduino.h>
#include <PubSubClient.h>

class Mqtt {
    public:
        Mqtt(String host, String user, String pass);
        void checkConnection();
        void publish(String topic, String msg);

    private:
        String _host;
        String _user;
        String _pass;
        PubSubClient _client;
};

#endif