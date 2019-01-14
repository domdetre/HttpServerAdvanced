#include "Settings.h"

void Settings::setup()
{
  if (!this->eepromEnabled) {
    return;
  }

  EEPROM.begin(EEPROM_LENGTH);

  if (!isEepromIdPresent()) {
    initEeprom();
  }

  pinStates = EEPROM.read(EEPROM_INDEX_PINSTATES);
  pinModes = EEPROM.read(EEPROM_INDEX_PINMODES);
  pinPullups = EEPROM.read(EEPROM_INDEX_PINPULLUPS);

  dataRestored = true;
}

bool Settings::isEepromIdPresent()
{
  return
    EEPROM.read(0) == id[0] &&
    EEPROM.read(1) == id[1] &&
    EEPROM.read(2) == id[2];
}

void Settings::initEeprom()
{
  EEPROM.write(0, id[0]);
  EEPROM.write(1, id[1]);
  EEPROM.write(2, id[2]);

  for (int index = 3; index < EEPROM_LENGTH; index++) {
    EEPROM.write(index, 0);
  }

  EEPROM.commit();
}

/**
  *  INPUT = 0
  *  OUTPUT = 1
  */
void Settings::storePinMode(byte pinNumber, byte mode)
{
  if (mode == OUTPUT) {
    bitWrite(pinModes, pinNumber, 1);
    bitWrite(pinPullups, pinNumber, 0);
  }
  else {
    bitWrite(pinModes, pinNumber, 0);
    bitWrite(pinPullups, pinNumber, (mode == INPUT_PULLUP));
  }

  if (this->eepromEnabled) {
    EEPROM.write(EEPROM_INDEX_PINMODES, pinModes);
    EEPROM.write(EEPROM_INDEX_PINPULLUPS, pinPullups);
    EEPROM.commit();
  }
}

void Settings::storePinState(byte pinNumber, bool isHigh)
{
  bitWrite(pinStates, pinNumber, isHigh);

  if (this->eepromEnabled) {
    EEPROM.write(EEPROM_INDEX_PINSTATES, pinStates);
    EEPROM.commit();
  }
}

byte Settings::getPinMode(byte pinNumber)
{

  if (bitRead(pinModes, pinNumber) == OUTPUT) {
    return OUTPUT;
  }

  if (bitRead(pinPullups, pinNumber) == 1) {
    return INPUT_PULLUP;
  }

  return INPUT;
}

byte Settings::getPinState(byte pinNumber)
{
  return bitRead(pinStates, pinNumber);
}

bool Settings::hasDataRestored()
{
  return dataRestored;
}
