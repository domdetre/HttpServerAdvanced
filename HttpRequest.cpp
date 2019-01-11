#include "HttpRequest.h"

HttpRequest::HttpRequest(StatusLed* statusLed)
{
  this->statusLed = statusLed;
}

void HttpRequest::readClient(WiFiClient* client)
{
  String request = "";
  int counter = 0;
  while (client->available()) {
    request += char(client->read());

    counter ++;
    if (counter > 10) {
      statusLed->turnOn();
    }
    if (counter > 20) {
      statusLed->turnOff();
      counter = 0;
    }
  }

  statusLed->turnOn();

  request.toLowerCase();

  int delimIndex = request.indexOf(' ');
  method = request.substring(delimIndex);
  request.remove(0, delimIndex + 1);

  delimIndex = request.indexOf(' ');
  uri = request.substring(delimIndex);
  request.remove(0, delimIndex + 1);

  delimIndex = request.indexOf('\n');
  protocol = request.substring(delimIndex);
  request.remove(0, delimIndex + 1);

  delimIndex = request.indexOf("\r\n\r\n");
  headers = request.substring(delimIndex);

  data = request.substring(delimIndex + 4);
}
