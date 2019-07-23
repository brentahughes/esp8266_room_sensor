#ifndef Wifi_h
#define Wifi_h

#include <Arduino.h>

class Wifi {
    public:
        Wifi(String ssid, String pass);
        void connect();
    private:
        String _ssid;
        String _pass;
};

#endif