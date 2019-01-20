#include "Pins.h"

void Pins::setup(Settings* settings, Debug* debug)
{
  this->settings = settings;
  this->debug = debug;
}

byte Pins::digital2gpio(byte digitalPinNumber)
{
  switch (digitalPinNumber) {
    case 0:
      return D0;
    case 1:
      return D1;
    case 2:
      return D2;
    case 3:
      return D3;
    case 4:
      return D4;
    case 5:
      return D5;
    case 6:
      return D6;
    case 7:
      return D7;
    case 8:
      return D8;
    case 9:
      return D9;
    case 10:
      return D10;
    case 11:
      return D11;
    case 12:
      return D12;
    case 13:
      return D13;
    case 14:
      return D14;
    case 15:
      return D15;
  }

  debug->error("Pin is out of range. Range: 0-15");
  return 255;
}

byte Pins::getState(byte digitalPinNumber)
{
  debug->info("Getting the state of pin: " + String(digitalPinNumber));

  byte pinState = 0;
  byte gpioNumber = digital2gpio(digitalPinNumber);
  if (gpioNumber == 255) {
    return 255;
  }

  debug->info("GPIO pin number: " + String(gpioNumber));

  // if the pin is input mode, read the value of it
  if (isInput(digitalPinNumber)) {
    pinState = digitalRead(gpioNumber);
    debug->info("Pin is in input mode, read state: " + String(pinState));
    return pinState;
  }

  // otherwise return the stored value
  pinState = settings->getPinState(digitalPinNumber);
  debug->info("Pin is in output mode, stored state: " + String(pinState));
  return pinState;
}

bool Pins::setState(byte digitalPinNumber, String strPinState)
{
  debug->info("Setting pin state of " + String(digitalPinNumber) + " to " + strPinState);

  byte gpioNumber = digital2gpio(digitalPinNumber);
  if (gpioNumber == 255) {
    return false;
  }

  debug->info("GPIO pin number: " + String(gpioNumber));

  // if the pin is in input mode, bail
  if (isInput(digitalPinNumber)) {
    debug->warn("The pin is in input mode!");
    return false;
  }

  int state = -1;
  if (strPinState == "0" || strPinState == "low") {
    state = LOW;
  }

  if (strPinState == "1" || strPinState == "high") {
    state = HIGH;
  }

  debug->info("Requested pin state: " + String(state));

  if (state < 0) {
    debug->warn("The requested state is invalid! Accepted values: 0, 1, low, high.");
    return false;
  }

  digitalWrite(gpioNumber, state);
  settings->storePinState(digitalPinNumber, state);
  return true;
}

bool Pins::initPin(byte digitalPinNumber, String strPinMode)
{
  debug->info("Initializing pin " + String(digitalPinNumber) + " with mode " + strPinMode);

  if (settings->isPinInitalized(digitalPinNumber)) {
    debug->error("Pin is already initialized");
    return false;
  }

  if (settings->isPinLocked(digitalPinNumber)) {
    debug->error("Pin is locked");
    return false;
  }

  byte gpioNumber = digital2gpio(digitalPinNumber);
  if (gpioNumber == 255) {
    return false;
  }

  debug->info("GPIO pin number: " + String(gpioNumber));

  int mode = -1;
  if (strPinMode == "input") {
    mode = INPUT;
  }
  else if (strPinMode == "output") {
    mode = OUTPUT;
  }
  else if (strPinMode == "input_pullup") {
    mode = INPUT_PULLUP;
  }
  else {
    debug->error("Requested pin mode is invalid! Only input, output, input_pullup are accepted.");
    return false;
  }

  pinMode(gpioNumber, mode);
  settings->setPinInit(digitalPinNumber);
  settings->storePinMode(digitalPinNumber, mode);
  debug->info("Pin initialized with mode " + String(mode));
  return true;
}

bool Pins::isInput(byte digitalPinNumber)
{
  return !isOutput(digitalPinNumber);
}

bool Pins::isOutput(byte digitalPinNumber)
{
  return settings->getPinMode(digitalPinNumber) == OUTPUT;
}

void Pins::restorePinModesAndStates()
{
  debug->info("Restoring pin modes and states.");

  for (byte digitalPinNumber = 0; digitalPinNumber < 16; digitalPinNumber++) {
    if (
      !settings->isPinInitalized(digitalPinNumber) ||
      settings->isPinLocked(digitalPinNumber)
    ) {
      continue;
    }

    byte gpioNumber = digital2gpio(digitalPinNumber);

    // if it is an output pin, set the mode to output, get the stored state and write that out
    if (isOutput(digitalPinNumber)) {
      byte pinState = settings->getPinState(digitalPinNumber);
      pinMode(gpioNumber, OUTPUT);
      digitalWrite(gpioNumber, pinState);

      debug->info("Restored pin " + String(digitalPinNumber) + " with mode output and state " + String(pinState));
      continue;
    }

    // otherwise just set the pinmode to the stored mode
    pinMode(gpioNumber, settings->getPinMode(digitalPinNumber));

    debug->info("Restored pin " + String(digitalPinNumber) + " with mode input or input_pullup");
  }
}
