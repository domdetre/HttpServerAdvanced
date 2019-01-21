#ifndef HTTP_SERVER_ADVANCED_H
#define HTTP_SERVER_ADVANCED_H

// check for the supported boards
#if \
  !defined(ARDUINO_ESP8266_WEMOS_D1MINI) && \
  !defined(ARDUINO_ESP8266_WEMOS_D1MINIPRO) && \
  !defined(ARDUINO_ESP8266_WEMOS_D1MINIPRO) && \
  !defined(ARDUINO_ESP8266_WEMOS_D1R1) && \
  !defined(ARDUINO_ESP8266_NODEMCU)
    #error Unsupported board!
#endif

#include <ESP8266WiFi.h>
#include <Arduino.h>

#define HTTP_SERVER_ADVANCED_VERSION "1.0.0"
#define HTTP_SERVER_ADVANCED_NAME "HTTP Server Advanced"

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "StatusLed.h"
#include "Settings.h"
#include "Debug.h"
#include "Pins.h"

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
    Pins pins;

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

    HttpResponse processRequestOfSerial(HttpRequest* request);
    HttpResponse processRequestOfDigital(HttpRequest* request);

    String readSerial();

    void writeSerial(String data);

    String getPinData(byte digitalPinNumber);

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
