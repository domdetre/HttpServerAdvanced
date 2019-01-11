#ifndef HTTP_SERVER_ADVANCED_H
#define HTTP_SERVER_ADVANCED_H

#include <ESP8266WiFi.h>
#include <Arduino.h>

#include "HttpRequest.h"
#include "StatusLed.h"

class HttpServerAdvanced
{
  public:
    const char* ssid;
    const char* sskey;
    bool debug = false;
    int port = 80;
    WiFiServer* server;

    StatusLed statusLed;

    HttpServerAdvanced(const char* ssid, const char* sskey, int port = 80, int ledPinNumber = -1);
    void init(const char* ssid, const char* sskey, int port = 80, int ledPinNumber = -1);
    void setup(const char* ssid, const char* sskey, int port = 80, int ledPinNumber = -1);

    void setup();
    void loop();

    void waitForClient(WiFiClient* client);
    String processRequest(HttpRequest* request);

    String readSerial();
    void writeSerial(String data);

    int convertPin(String strPinNumber);
    String getPinState(String strPinNumber);
    void setPinState(String strPinNumber, String strPinState);
    void initPin(String strPinNumber, String strPinMode);
};

#endif
