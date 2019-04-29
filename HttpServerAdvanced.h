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

#include "version.h"

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "StatusLed.h"
#include "Settings.h"
#include "Debug.h"
#include "Pins.h"

struct AccessPoint {
  char ssid[32];
  char psk[64];
  byte priority = 255;
  int signalStrength = -1000;
  bool found = false;
};

class HttpServerAdvanced
{
  public:
    int port = 80;
    WiFiServer* server;
    StatusLed statusLed;
    Settings settings;
    Debug debug;
    Pins pins;

    AccessPoint* accessPointList;
    int accessPointCounter = 0;

    bool setupComplete = false;

    HttpServerAdvanced(const char* ssid = nullptr, const char* sskey = nullptr, int port = 80, int ledPinNumber = LED_BUILTIN);

    /**
     * Adds a new accessPoint to the existing list of access points.
     * @param  String   ssid     SSID of the AP.
     * @param  String   psk      WPA-PSK for the AP.
     * @param  byte     priority The lower the number, higher the priority. Range is 0 to 255, default value is 0.
     * @return bool              False on error.
     */
    bool addAccessPoint(String ssid, String psk, byte priority = 255);

    /**
     * Sets up the pin to be used as status indicator.
     * @param ledpin GPIO pin number
     */
    void setStatusLedPin(byte ledpin);

    /**
     * Sets the port of the webserver.
     * @param port
     */
    void setServerPort(int port);

    /**
     * Disaples reading and writing the eeprom.
     */
    void disableEeprom();

    /**
     * Sets up the Advanced Http Server
     */
    void setup();

    /**
     * Scans for the available wifi networks, gets their signal
     * strength, and selects the one with highest priority and highest
     * signal strength.
     * Priority has higher precedence than signal strength.
     */
    bool setupWifi();

    /**
     * The loop;
     */
    void loop();

    /**
     * Waits for the http client to send the request data
     * @param client WiFiClient
     */
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
