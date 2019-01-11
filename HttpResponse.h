#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <ESP8266WiFi.h>
#include <Arduino.h>

class HttpResponse
{
  public:
    int code = 200;
    String status = "OK";
    String protocol = "HTTP/1.1";
    String contentType = "text/plain";
    String data;

    HttpResponse(int code, String status);
    HttpResponse(String data);
    HttpResponse();
    String toString();

    static HttpResponse BadRequest();
    static HttpResponse NotFound();
    static HttpResponse Unacceptable();
};

#endif
