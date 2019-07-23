#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include "TempSensor.h"

TempSensor::TempSensor(int pin) {
    _pin = pin;
    OneWire oneWire(pin);
    DallasTemperature sensors(&oneWire);
    _sensors = sensors;
    _sensors.begin();
}

float TempSensor::read() {
    _sensors.requestTemperatures();
    float temp = _sensors.getTempFByIndex(0);
    Serial.print("temp: ");
    Serial.println(temp);
    return temp;
}