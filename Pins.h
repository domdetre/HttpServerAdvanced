#ifndef PINS_H
#define PINS_H

#include <Arduino.h>
#include "Settings.h"
#include "Debug.h"

class Pins
{
  public:
    Settings* settings;
    Debug* debug;

    void setup(Settings* settings, Debug* debug);

    /**
     * Converts the String pin to byte pin and validates it.
     * It accepts numeric value where the string is going to be converted to integer meaning the GPIO pin is going to be used.
     * Accepts D# format where the digital pin number will be translated to a GPIO pin and that will be returned.
     * If the string doesn't confomr any of the above rules, it will return 255, indicating an error.
     * @param  digitalPinNumber
     * @return byte The GPIO pin number or 255 on error
     */
    byte digital2gpio(byte digitalPinNumber);

    /**
     * Gets the state of the pin.
     * If the pin is in input mode, then it will return result of digitalRead method.
     * If the pin is in output mode, then it will return the last set state stored.
     * If the pinnumber is not correct will return 255.
     * @param  String digitalPinNumber Numeric or D# format
     * @return byte                    0 or 1 or 255
     */
    byte getState(byte digitalPinNumber);

    bool setState(byte digitalPinNumber, String strPinState);

    bool initPin(byte digitalPinNumber, String strPinMode);

    bool isInput(byte digitalPinNumber);

    bool isOutput(byte digitalPinNumber);

    void restorePinModesAndStates();
};

#endif
