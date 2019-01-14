#ifndef SETTINGS_H
#define SETTINGS_H

#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <EEPROM.h>

#define EEPROM_LENGTH 128
#define EEPROM_INDEX_PINMODES 3
#define EEPROM_INDEX_PINPULLUPS 4
#define EEPROM_INDEX_PINSTATES 5

class Settings
{
  public:
    byte id[3] = {193, 193, 193};
    byte pinStates = 0;
    byte pinModes = 0;
    byte pinPullups = 0;

    bool eepromEnabled = true;
    bool dataRestored = false;

    void setup();

    bool isEepromIdPresent();
    void initEeprom();

    void storePinMode(byte pinNumber, byte pinMode);
    void storePinState(byte pinNumber, bool isHigh);

    byte getPinMode(byte pinNumber);
    byte getPinState(byte pinNumber);

    bool hasDataRestored();
};

#endif
