#ifndef EEPROM_DATA_H
#define EEPROM_DATA_H

#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <EEPROM.h>

#define EEPROM_LENGTH 128
#define EEPROM_INDEX_PINMODES 3
#define EEPROM_INDEX_PINPULLUPS 4
#define EEPROM_INDEX_PINSTATES 5

class EepromData
{
  public:
    byte id[3] = {193, 193, 193};
    byte pinStates;
    byte pinModes;
    byte pinPullups;

    bool isNew = false;

    void setup();

    void storePinMode(byte pinNumber, byte pinMode);
    void storePinState(byte pinNumber, bool isHigh);

    byte getPinMode(byte pinNumber);
    byte getPinState(byte pinNumber);
};

#endif
