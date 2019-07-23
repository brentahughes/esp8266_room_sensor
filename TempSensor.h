#ifndef TempSensor_h
#define TempSensor_h

#include <Arduino.h>
#include <DallasTemperature.h>

class TempSensor {
    public:
        TempSensor(int pin);
        float read();

    private:
        int _pin;
        DallasTemperature _sensors;
};

#endif