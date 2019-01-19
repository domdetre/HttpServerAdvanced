#ifndef SETTINGS_H
#define SETTINGS_H

#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <EEPROM.h>

#define EEPROM_LENGTH 128
#define EEPROM_INDEX_PINMODES 3
#define EEPROM_INDEX_PINPULLUPS 5
#define EEPROM_INDEX_PINSTATES 7

class Settings
{
  public:
    byte id[3] = {193, 193, 193};
    byte pinStates[2] = {0,0};
    byte pinModes[2] = {0,0};
    byte pinPullups[2] = {0,0};

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

    byte getByteSetIndex(byte byteIndex);
    byte getBitIndex(byte byteIndex);

    void writeByteSet(byte* byteSet, byte byteIndex, byte value);
    byte readByteSet(byte* byteSet, byte byteIndex);
};

#endif
