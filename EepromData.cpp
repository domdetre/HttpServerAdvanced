#include "EepromData.h"

void EepromData::setup()
{
  EEPROM.begin(EEPROM_LENGTH);

  if (
    EEPROM.read(0) != id[0] ||
    EEPROM.read(1) != id[1] ||
    EEPROM.read(2) != id[2]
  ) {
    EEPROM.write(0, id[0]);
    EEPROM.write(1, id[1]);
    EEPROM.write(2, id[2]);

    for (int index = 3; index < EEPROM_LENGTH; index++) {
      EEPROM.write(index, 0);
    }

    EEPROM.commit();

    isNew = true;
  }

  pinStates = EEPROM.read(EEPROM_INDEX_PINSTATES);
  pinModes = EEPROM.read(EEPROM_INDEX_PINMODES);
  pinPullups = EEPROM.read(EEPROM_INDEX_PINPULLUPS);
}

/**
  *  INPUT = 0
  *  OUTPUT = 1
  */
void EepromData::storePinMode(byte pinNumber, byte mode)
{
  if (mode == OUTPUT) {
    bitWrite(pinModes, pinNumber, 1);
    bitWrite(pinPullups, pinNumber, 0);
  }
  else {
    bitWrite(pinModes, pinNumber, 0);
    bitWrite(pinPullups, pinNumber, (mode == INPUT_PULLUP));
  }

  EEPROM.write(EEPROM_INDEX_PINMODES, pinModes);
  EEPROM.write(EEPROM_INDEX_PINPULLUPS, pinPullups);
  EEPROM.commit();
}

void EepromData::storePinState(byte pinNumber, bool isHigh)
{
  bitWrite(pinStates, pinNumber, isHigh);
  EEPROM.write(EEPROM_INDEX_PINSTATES, pinStates);
  EEPROM.commit();
}

byte EepromData::getPinMode(byte pinNumber)
{
  if (bitRead(pinModes, pinNumber) == OUTPUT) {
    return OUTPUT;
  }

  if (bitRead(pinPullups, pinNumber) == 1) {
    return INPUT_PULLUP;
  }

  return INPUT;
}

byte EepromData::getPinState(byte pinNumber)
{
  return bitRead(pinStates, pinNumber);
}
