#ifndef STATUS_LED_H
#define STATUS_LED_H

#include <ESP8266WiFi.h>
#include <Arduino.h>

class StatusLed
{
  public:
    int ledPinNumber;

    void setup();
    void turnOn();
    void turnOff();
};

#endif
