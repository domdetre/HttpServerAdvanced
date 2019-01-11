#include "StatusLed.h"

void StatusLed::setup()
{
  if (ledPinNumber >= 0) {
    pinMode(ledPinNumber, OUTPUT);
    turnOn();
  }
}

void StatusLed::turnOn()
{
  if (ledPinNumber >= 0) {
    digitalWrite(ledPinNumber, LOW);
  }
}

void StatusLed::turnOff()
{
  if (ledPinNumber >= 0) {
    digitalWrite(ledPinNumber, HIGH);
  }
}
