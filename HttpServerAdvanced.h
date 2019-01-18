#ifndef HTTP_SERVER_ADVANCED_H
#define HTTP_SERVER_ADVANCED_H

#include <ESP8266WiFi.h>
#include <Arduino.h>

#define HTTP_SERVER_ADVANCED_VERSION "1.0.0"
#define HTTP_SERVER_ADVANCED_NAME "HTTP Server Advanced"

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "StatusLed.h"
#include "Settings.h"
#include "Debug.h"


class HttpServerAdvanced
{
  public:
    const char* ssid;
    const char* sskey;
    int port = 80;
    WiFiServer* server;
    StatusLed statusLed;
    Settings settings;
    Debug debug;

    HttpServerAdvanced(const char* ssid, const char* sskey, int port = 80, int ledPinNumber = -1);

    void init(const char* ssid, const char* sskey, int port = 80, int ledPinNumber = -1);

    void setup(const char* ssid, const char* sskey, int port = 80, int ledPinNumber = -1);

    void setup();

    void loop();

    void waitForClient(WiFiClient* client);
    /**
     * Processes the request and returns accodringly
     * If the endpoint cannot be found, returns 404
     * If the endpoint can be found, but the parameters
     * or method is not supported, returns 400
     * If the endpoint can be found, and the parameters are good,
     * but the operation failed the validation, returns 406
     * @param  request The request to process
     * @return         HttpResponse
     */
    HttpResponse processRequest(HttpRequest* request);

    String readSerial();

    void writeSerial(String data);

    /**
     * Converts the String pin to byte pin and validates it.
     * It accepts numeric value where the string is going to be converted to integer meaning the GPIO pin is going to be used.
     * Accepts D# format where the digital pin number will be translated to a GPIO pin and that will be returned.
     * If the string doesn't confomr any of the above rules, it will return 255, indicating an error.
     * @param  String strPinNumber Numeric or D# format
     * @return byte                The GPIO pin number or 255 on error
     */
    byte convertPin(String strPinNumber);

    /**
     * Gets the state of the pin.
     * If the pin is in input mode, then it will return result of digitalRead method.
     * If the pin is in output mode, then it will return the last set state stored.
     * If the pinnumber is not correct will return 255.
     * @param  String strPinNumber Numeric or D# format
     * @return byte                0 or 1 or 255
     */
    byte getPinState(String strPinNumber);

    bool setPinState(String strPinNumber, String strPinState);

    bool initPin(String strPinNumber, String strPinMode);

    bool isPinInput(byte pinNumber);

    bool isPinOutput(byte pinNumber);

    void restorePinModesAndStates();

    void disableEeprom();

    /**
     * Enables logging either on serial or on the http interface.
     * @param serial      enables logs to be outputted on the serial
     * @param store       enables logs to be stored until it can be retrieved by http interface. Warning this can easily fill the RAM.
     * @param infoLogs    whether to include info logs
     * @param warningLogs whether to include warning logs
     * @param errorLogs   whether to include error logs
     */
    void enableDebug(bool serial = true, bool store = false, bool infoLogs = true, bool warningLogs = true, bool errorLogs = true);
};

#endif
