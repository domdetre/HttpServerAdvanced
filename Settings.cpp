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

  for (byte bitSet = 0; bitSet < 2; bitSet++) {
    pinStates[bitSet] = EEPROM.read(EEPROM_INDEX_PINSTATES + bitSet);
    pinModes[bitSet] = EEPROM.read(EEPROM_INDEX_PINMODES + bitSet);
    pinPullups[bitSet] = EEPROM.read(EEPROM_INDEX_PINPULLUPS + bitSet);
  }

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

void Settings::storePinMode(byte pinNumber, byte mode)
{
  byte bitSet = getByteSetIndex(pinNumber);

  if (mode == OUTPUT) {
    bitWrite(pinModes[bitSet], getBitIndex(pinNumber), OUTPUT);
    bitWrite(pinPullups[bitSet], getBitIndex(pinNumber), 0);
  }
  else {
    bitWrite(pinModes[bitSet], getBitIndex(pinNumber), INPUT);
    bitWrite(pinPullups[bitSet], getBitIndex(pinNumber), (mode == INPUT_PULLUP));
  }

  if (this->eepromEnabled) {
    EEPROM.write(EEPROM_INDEX_PINMODES + bitSet, pinModes[bitSet]);
    EEPROM.write(EEPROM_INDEX_PINPULLUPS + bitSet, pinPullups[bitSet]);
    EEPROM.commit();
  }
}

void Settings::storePinState(byte pinNumber, bool isHigh)
{
  byte bitSet = getByteSetIndex(pinNumber);

  bitWrite(pinStates[bitSet], getBitIndex(pinNumber), isHigh);

  if (this->eepromEnabled) {
    EEPROM.write(EEPROM_INDEX_PINSTATES + bitSet, pinStates[bitSet]);
    EEPROM.commit();
  }
}

byte Settings::getPinMode(byte pinNumber)
{
  byte bitSet = getByteSetIndex(pinNumber);

  if (bitRead(pinModes[bitSet], getBitIndex(pinNumber)) == OUTPUT) {
    return OUTPUT;
  }

  if (bitRead(pinPullups[bitSet], getBitIndex(pinNumber)) == 1) {
    return INPUT_PULLUP;
  }

  return INPUT;
}

byte Settings::getPinState(byte pinNumber)
{
  byte bitSet = getByteSetIndex(pinNumber);

  return bitRead(pinStates[bitSet], getBitIndex(pinNumber));
}

bool Settings::hasDataRestored()
{
  return dataRestored;
}

byte Settings::getByteSetIndex(byte bytePosition)
{
  return (byte)(bytePosition / 8);
}

byte Settings::getBitIndex(byte byteIndex)
{
  return byteIndex - 8 * getByteSetIndex(byteIndex);
}

