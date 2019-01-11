#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <ESP8266WiFi.h>
#include <Arduino.h>

#include "StatusLed.h"

class HttpRequest
{
  public:
    StatusLed* statusLed;

    String method = "";
    String uri = "";
    String protocol = "";
    String headers = "";
    String data = "";

    HttpRequest(StatusLed* statusLed);
    void readClient(WiFiClient* client);
};

#endif
