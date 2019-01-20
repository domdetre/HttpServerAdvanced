#include "Settings.h"

void Settings::setup()
{
  if (!this->eepromEnabled) {
    return;
  }

  EEPROM.begin(EEPROM_LENGTH);

  if (!isEepromIdPresent()) {
    initEeprom();
    return;
  }

  for (byte byteSetIndex = 0; byteSetIndex < 2; byteSetIndex++) {
    pinStates[byteSetIndex] = EEPROM.read(EEPROM_INDEX_PINSTATES + byteSetIndex);
    pinModes[byteSetIndex] = EEPROM.read(EEPROM_INDEX_PINMODES + byteSetIndex);
    pinPullups[byteSetIndex] = EEPROM.read(EEPROM_INDEX_PINPULLUPS + byteSetIndex);
    pinInits[byteSetIndex] = EEPROM.read(EEPROM_INDEX_PININITS + byteSetIndex);
    pinLocks[byteSetIndex] = EEPROM.read(EEPROM_INDEX_PINLOCKS + byteSetIndex);
  }

  dataRestored = true;
}

bool Settings::isEepromIdPresent()
{
  return
    EEPROM.read(0) == EEPROM_ID &&
    EEPROM.read(1) == EEPROM_ID &&
    EEPROM.read(2) == EEPROM_ID;
}

void Settings::initEeprom()
{
  EEPROM.write(0, EEPROM_ID);
  EEPROM.write(1, EEPROM_ID);
  EEPROM.write(2, EEPROM_ID);

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

  writeEeprom(EEPROM_INDEX_PINMODES, pinModes);
  writeEeprom(EEPROM_INDEX_PINPULLUPS, pinPullups);
}

void Settings::storePinState(byte pinNumber, bool isHigh)
{
  writeByteSet(pinStates, pinNumber, isHigh);
  writeEeprom(EEPROM_INDEX_PINSTATES, pinStates);
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

bool Settings::isPinInitalized(byte pinNumber)
{
  return readByteSet(pinInits, pinNumber) == 1;
}

bool Settings::isPinLocked(byte pinNumber)
{
  return readByteSet(pinLocks, pinNumber) == 1;
}

void Settings::setPinInit(byte pinNumber)
{
  writeByteSet(pinInits, pinNumber, 1);
  writeEeprom(EEPROM_INDEX_PININITS, pinInits);
}

void Settings::setPinLock(byte pinNumber)
{
  writeByteSet(pinLocks, pinNumber, 1);
  writeEeprom(EEPROM_INDEX_PINLOCKS, pinLocks);
}

void Settings::unsetPinInit(byte pinNumber)
{
  writeByteSet(pinInits, pinNumber, 0);
  writeEeprom(EEPROM_INDEX_PININITS, pinInits);
}

void Settings::unsetPinLock(byte pinNumber)
{
  writeByteSet(pinLocks, pinNumber, 0);
  writeEeprom(EEPROM_INDEX_PINLOCKS, pinLocks);
}

void Settings::writeEeprom(byte eepromIndex, byte* byteSet)
{
  if (this->eepromEnabled) {
    EEPROM.write(eepromIndex, byteSet[0]);
    EEPROM.write(eepromIndex + 1, byteSet[1]);
    EEPROM.commit();
  }
}
