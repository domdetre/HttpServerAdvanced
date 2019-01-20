#include "HttpServerAdvanced.h"

HttpServerAdvanced::HttpServerAdvanced(const char* ssid, const char* sskey, int port, int ledPinNumber)
{
  init(ssid, sskey, port, ledPinNumber);
}

void HttpServerAdvanced::init(const char* ssid, const char* sskey, int port, int ledPinNumber)
{
  this->ssid = ssid;
  this->sskey = sskey;
  this->port = port;
  this->statusLed.ledPinNumber = ledPinNumber;

  this->server = new WiFiServer(port);
}

void HttpServerAdvanced::setup(const char* ssid, const char* sskey, int port, int ledPinNumber)
{
  init(ssid, sskey, port, ledPinNumber);
  setup();
}

void HttpServerAdvanced::setup()
{
  if (debug.enabled) {
    delay(500);
  }

  debug.info("Version " + String(HTTP_SERVER_ADVANCED_VERSION));

  delay(10);

  pins.setup(&settings, &debug);

  statusLed.setup();

  settings.setup();
  if (settings.hasDataRestored()) {
    pins.restorePinModesAndStates();
  }

  debug.info("Connecting to " + String(ssid));

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, sskey);

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);

    statusLed.turnOff();

    delay(250);

    statusLed.turnOn();

    debug.waiting();
  }

  debug.waitingFinished();
  debug.info("WiFi connected.");

  server->begin();

  debug.info("Server started, local IP: " + WiFi.localIP().toString());

  debug.info("Setup complete.");
}

void HttpServerAdvanced::loop()
{
  // Check if we have a new client
  WiFiClient client = server->available();
  if (!client) {
    return;
  }

  debug.info("Got request.");

  waitForClient(&client);

  HttpRequest request(&statusLed);
  request.readClient(&client);

  HttpResponse response = processRequest(&request);

  client.flush();

  client.print(response.toString());
  delay(1);
}

void HttpServerAdvanced::waitForClient(WiFiClient* client)
{
  debug.info("Waiting for the client to send data");

  int counter = 0;
  while (!client->available()) {
    delay(1);
    counter++;

    if (counter > 250) {
      statusLed.turnOff();
    }

    if (counter > 500) {
      statusLed.turnOn();
      counter = 0;
      debug.waiting();
    }
  }

  debug.waitingFinished();
  debug.info("All data received from the client.");
}

HttpResponse HttpServerAdvanced::processRequest(HttpRequest* request)
{
  debug.info("Processesing request");
  debug.info("request method: " + request->method);
  debug.info("request uri: " + request->uri);
  debug.info("request protocol: " + request->protocol);
  debug.info("request headers: " + request->headers);
  debug.info("request data: " + request->data);

  //serial
  if (request->uri == "/serial") {
    return processRequestOfSerial(request);
  }

  //digital/{pinNumber}
  if (request->uri.indexOf("/digital") == 0) {
    return processRequestOfDigital(request);
  }

  //debug
  if (request->uri == "/debug") {
    if (request->method == "get") {
      return HttpResponse(
        debug.get()
      );
    }

    return HttpResponse::BadRequest();
  }

  return HttpResponse::NotFound();
}

HttpResponse HttpServerAdvanced::processRequestOfSerial(HttpRequest* request)
{
  if (request->method == "get") {
    return HttpResponse(
      readSerial()
    );
  }

  if (request->method == "post") {
    writeSerial(request->data);
    return HttpResponse();
  }

  return HttpResponse::BadRequest();
}

HttpResponse HttpServerAdvanced::processRequestOfDigital(HttpRequest* request)
{
  String strPinNumber = request->uri.substring(9);

  byte pinNumber = strPinNumber.toInt();
  if (String(pinNumber) != strPinNumber) {
    return HttpResponse::BadRequest(
      "The pin number contains non-digit characters."
    );
  }

  if (pinNumber < 0 || pinNumber > 15) {
    return HttpResponse::BadRequest(
      "The pin number is out of range. Range: 0-15"
    );
  }

  // GET
  if (request->method == "get") {
    return HttpResponse(
      getPinData(pinNumber)
    );
  }

  // If the pin is locked, only get is allowed
  if (settings.isPinLocked(pinNumber)) {
    return HttpResponse::Unacceptable(
      "The pin is locked, can't set state.\r\n" +
      getPinData(pinNumber)
    );
  }

  // POST
  if (request->method == "post") {
    if (settings.getPinMode(pinNumber) != OUTPUT) {
      return HttpResponse::Unacceptable(
        "The pin is not in output mode, can't set state.\r\n" +
        getPinData(pinNumber)
      );
    }

    if (!pins.setState(pinNumber, request->data)) {
      return HttpResponse::InternalError();
    }

    return HttpResponse(
      getPinData(pinNumber)
    );
  }

  // PUT
  if (request->method == "put") {
    if (settings.isPinInitalized(pinNumber)) {
      return HttpResponse::Unacceptable(
        "The pin is already initialized.\r\n" +
        getPinData(pinNumber)
      );
    }

    if (
      request->data != "input" ||
      request->data != "output" ||
      request->data != "input_pullup"
    ) {
      return HttpResponse::BadRequest(
        "Bad mode requested. Following is accepted: input, output, input_pullup."
      );
    }

    if (!pins.initPin(pinNumber, request->data)) {
      return HttpResponse::InternalError();
    }

    return HttpResponse(
      getPinData(pinNumber)
    );
  }

  // DELETE
  if (request->method == "delete") {
    settings.unsetPinInit(pinNumber);

    return HttpResponse(
      getPinData(pinNumber)
    );
  }

  return HttpResponse::BadRequest();
}

String HttpServerAdvanced::readSerial()
{
  debug.info("Reading serial data.");

  String serialData;
  while (Serial.available()) {
    serialData += char(Serial.read());
  }

  return serialData;
}

void HttpServerAdvanced::writeSerial(String data)
{
  debug.info("Writing serial data.");

  Serial.println(data);
}

void HttpServerAdvanced::disableEeprom()
{
  this->settings.eepromEnabled = false;
}

void HttpServerAdvanced::enableDebug(bool serial, bool store, bool infoLogs, bool warningLogs, bool errorLogs)
{
  debug.enabled = true;
  debug.serial = serial;
  debug.store = store;

  debug.infoLogs = infoLogs;
  debug.warningLogs = warningLogs;
  debug.errorLogs = errorLogs;
}

String HttpServerAdvanced::getPinData(byte digitalPinNumber)
{
  if (!settings.isPinInitalized(digitalPinNumber)) {
    return "Not initialized";
  }

  return
    "initialized: " +
      String(settings.isPinInitalized(digitalPinNumber), DEC) +
      "\r\n" +
    "state: " +
      String(pins.getState(digitalPinNumber), DEC) +
      "\r\n" +
    "mode: " +
      String(settings.getPinMode(digitalPinNumber), DEC) +
      "\r\n" +
    "locked: " +
      String(settings.isPinLocked(digitalPinNumber), DEC) +
      "\r\n";
}
