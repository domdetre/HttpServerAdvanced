#ifndef HTTP_SERVER_ADVANCED_H
#define HTTP_SERVER_ADVANCED_H

#include <ESP8266WiFi.h>
#include <Arduino.h>

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "StatusLed.h"
#include "Settings.h"

class HttpServerAdvanced
{
  public:
    const char* ssid;
    const char* sskey;
    bool debug = false;
    int port = 80;
    WiFiServer* server;
    StatusLed statusLed;
    Settings settings;

    HttpServerAdvanced(const char* ssid, const char* sskey, int port = 80, int ledPinNumber = -1);
    void init(const char* ssid, const char* sskey, int port = 80, int ledPinNumber = -1);
    void setup(const char* ssid, const char* sskey, int port = 80, int ledPinNumber = -1);

    void setup();
    void loop();

    void waitForClient(WiFiClient* client);
    HttpResponse processRequest(HttpRequest* request);

    String readSerial();
    void writeSerial(String data);

    byte convertPin(String strPinNumber);
    byte getPinState(String strPinNumber);
    bool setPinState(String strPinNumber, String strPinState);
    bool initPin(String strPinNumber, String strPinMode);
    bool isPinInput(byte pinNumber);
    bool isPinOutput(byte pinNumber);
    void restorePinModesAndStates();

    void disableEeprom();
};

#endif
