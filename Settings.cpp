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

  for (byte byteSetIndex = 0; byteSetIndex < 2; byteSetIndex++) {
    pinStates[byteSetIndex] = EEPROM.read(EEPROM_INDEX_PINSTATES + byteSetIndex);
    pinModes[byteSetIndex] = EEPROM.read(EEPROM_INDEX_PINMODES + byteSetIndex);
    pinPullups[byteSetIndex] = EEPROM.read(EEPROM_INDEX_PINPULLUPS + byteSetIndex);
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
  if (mode == OUTPUT) {
    writeByteSet(pinModes, pinNumber, OUTPUT);
    writeByteSet(pinPullups, pinNumber, 0);
  }
  else {
    writeByteSet(pinModes, pinNumber, INPUT);
    writeByteSet(pinPullups, pinNumber, (mode == INPUT_PULLUP));
  }

  if (this->eepromEnabled) {
    byte byteSetIndex = getByteSetIndex(pinNumber);
    EEPROM.write(EEPROM_INDEX_PINMODES + byteSetIndex, pinModes[byteSetIndex]);
    EEPROM.write(EEPROM_INDEX_PINPULLUPS + byteSetIndex, pinPullups[byteSetIndex]);
    EEPROM.commit();
  }
}

void Settings::storePinState(byte pinNumber, bool isHigh)
{
  writeByteSet(pinStates, pinNumber, isHigh);

  if (this->eepromEnabled) {
    byte byteSetIndex = getByteSetIndex(pinNumber);
    EEPROM.write(EEPROM_INDEX_PINSTATES + byteSetIndex, pinStates[byteSetIndex]);
    EEPROM.commit();
  }
}

byte Settings::getPinMode(byte pinNumber)
{
  if (readByteSet(pinModes, pinNumber) == OUTPUT) {
    return OUTPUT;
  }

  if (readByteSet(pinPullups, pinNumber) == 1) {
    return INPUT_PULLUP;
  }

  return INPUT;
}

byte Settings::getPinState(byte pinNumber)
{
  return readByteSet(pinStates, pinNumber);
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

void Settings::writeByteSet(byte* byteSet, byte byteIndex, byte value)
{
  byte byteSetIndex = getByteSetIndex(byteIndex);
  byte bitIndex = getBitIndex(byteIndex);
  bitWrite(byteSet[byteSetIndex], bitIndex, value);
}

byte Settings::readByteSet(byte* byteSet, byte byteIndex)
{
  byte byteSetIndex = getByteSetIndex(byteIndex);
  byte bitIndex = getBitIndex(byteIndex);
  return bitRead(byteSet[byteSetIndex], bitIndex);
}
