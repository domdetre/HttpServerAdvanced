#ifndef HTTPSERVERADVANCED_H
#define HTTPSERVERADVANCED_H

#include <ESP8266WiFi.h>
#include <Arduino.h>

class HttpServerAdvanced
{
  public:
    const char* ssid;
    const char* sskey;
    bool debug = false;
    int port = 80;
    WiFiServer* server;
    int ledPin = -1;
    String method = "";
    String uri = "";
    String protocol = "";
    String data = "";

    HttpServerAdvanced(const char* ssid, const char* sskey, int port = 80, int ledPin = -1);
    void init(const char* ssid, const char* sskey, int port = 80, int ledPin = -1);

    void setup(const char* ssid, const char* sskey, int port = 80, int ledPin = -1);
    void setup();
    void loop();

    void waitForClient(WiFiClient client);
    void readClient(WiFiClient client);
    String sendToSerial();

    void turnOnLed();
    void turnOffLed();

    String processRequest();
    int convertPin(String strPinNumber);

    String readSerial();
    void writeSerial(String data);

    String getPinState(String strPinNumber);
    void setPinState(String strPinNumber, String strPinState);
    void initPin(String strPinNumber, String strPinMode);
};

#endif
