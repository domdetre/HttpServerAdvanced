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

  debug.log("Booting... ");

  delay(10);

  statusLed.setup();

  settings.setup();
  if (settings.hasDataRestored()) {
    restorePinModesAndStates();
  }

  debug.log("Connecting to " + String(ssid));

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
  debug.log("WiFi connected");

  server->begin();

  debug.log("Server started" + WiFi.localIP().toString());
}

void HttpServerAdvanced::loop()
{
  // Check if we have a new client
  WiFiClient client = server->available();
  if (!client) {
    return;
  }

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
  debug.log("Waiting for the client to send data");

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
}

HttpResponse HttpServerAdvanced::processRequest(HttpRequest* request)
{
  debug.log("Processesing request");
  debug.log("request uri: " + request->uri);
  debug.log("request method: " + request->method);
  debug.log("request data: " + request->data);

  //serial
  if (request->uri == "/serial") {
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

  //digital/{pinNumber}
  if (request->uri.indexOf("/digital/") == 0) {
    String strPinNumber = request->uri.substring(9);

    if (request->method == "get") {
      byte state = getPinState(strPinNumber);
      if (state == 255) {
        return HttpResponse::BadRequest();
      }

      return HttpResponse(
        String(state, DEC)
      );
    }

    if (request->method == "post") {
      if (setPinState(strPinNumber, request->data)) {
        return HttpResponse();
      }

      return HttpResponse::Unacceptable();
    }

    if (request->method == "put") {
      if (initPin(strPinNumber, request->data)) {
        return HttpResponse();
      }

      return HttpResponse::Unacceptable();
    }

    return HttpResponse::BadRequest();
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

byte HttpServerAdvanced::convertPin(String strPinNumber)
{
  bool digital = false;
  if (strPinNumber.indexOf("d") == 0) {
    digital = true;
    strPinNumber.remove(0, 1);
  }

  String numbers = "0123456789";

  // The length of the number can only be 1 or 2 digits
  if (strPinNumber.length() > 2 || strPinNumber.length() == 0) {
    return 255;
  }

  if (numbers.indexOf(strPinNumber[0]) < 0 || numbers.indexOf(strPinNumber[1]) < 0) {
    return 255;
  }

  int pinNumber = strPinNumber.toInt();

  if (!digital) {
    return pinNumber;
  }

  switch (pinNumber) {
    case 0:
      return D0;
    case 1:
      return D1;
    case 2:
      return D2;
    case 3:
      return D3;
    case 4:
      return D4;
    case 5:
      return D5;
    case 6:
      return D6;
    case 7:
      return D7;
    case 8:
      return D8;
    case 9:
      return D9;
    case 10:
      return D10;
    case 11:
      return D11;
    case 12:
      return D12;
    case 13:
      return D13;
    case 14:
      return D14;
    case 15:
      return D15;
  }

  return 255;
}

String HttpServerAdvanced::readSerial()
{
  String serialData;
  while (Serial.available()) {
    serialData += char(Serial.read());
  }

  return serialData;
}

void HttpServerAdvanced::writeSerial(String data)
{
  Serial.println(data);
}

byte HttpServerAdvanced::getPinState(String strPinNumber)
{
  byte pinNumber = convertPin(strPinNumber);
  if (pinNumber == 255) {
    return 255;
  }

  // if the pin is input mode, read the value of it
  if (isPinInput(pinNumber)) {
    return digitalRead(pinNumber);
  }

  // otherwise return the stored value
  return settings.getPinState(pinNumber);
}

bool HttpServerAdvanced::setPinState(String strPinNumber, String strPinState)
{
  debug.log("Setting pin state");

  byte pinNumber = convertPin(strPinNumber);
  if (pinNumber == 255) {
    return false;
  }

  debug.log("pin number: " + String(pinNumber));

  // if the pin is in input mode, bail
  if (isPinInput(pinNumber)) {
    debug.log("pin is input");
    return false;
  }

  int state = -1;
  if (strPinState == "0" || strPinState == "low") {
    state = LOW;
  }

  if (strPinState == "1" || strPinState == "high") {
    state = HIGH;
  }

  debug.log("pin state: " + String(state));

  if (state < 0) {
    return false;
  }

  digitalWrite(pinNumber, state);
  settings.storePinState(pinNumber, state);
  return true;
}

bool HttpServerAdvanced::initPin(String strPinNumber, String strPinMode)
{
  debug.log("Init pin ...");

  int pinNumber = convertPin(strPinNumber);
  if (pinNumber == 255) {
    return false;
  }

  debug.log("pin number: " + String(pinNumber));

  int mode = -1;
  if (strPinMode == "input") {
    mode = INPUT;
  }
  else if (strPinMode == "output") {
    mode = OUTPUT;
  }
  else if (strPinMode == "input_pullup") {
    mode = INPUT_PULLUP;
  }
  else {
    debug.log("pin mode is invalid");
    return false;
  }

  debug.log("pin mode: " + String(mode));

  pinMode(pinNumber, mode);
  settings.storePinMode(pinNumber, mode);
  return true;
}

bool HttpServerAdvanced::isPinInput(byte pinNumber)
{
  return !isPinOutput(pinNumber);
}

bool HttpServerAdvanced::isPinOutput(byte pinNumber)
{
  return settings.getPinMode(pinNumber) == OUTPUT;
}

void HttpServerAdvanced::restorePinModesAndStates()
{
  for (byte pinNumber = 0; pinNumber < 255; pinNumber++) {
    // if it is an output pin, set the mode to output, get the stored state and write that out
    if (isPinOutput(pinNumber)) {
      pinMode(pinNumber, OUTPUT);
      digitalWrite(pinNumber, settings.getPinState(pinNumber));
      continue;
    }

    // otherwise just set the pinmode to the stored mode
    pinMode(pinNumber, settings.getPinMode(pinNumber));
  }
}

void HttpServerAdvanced::disableEeprom()
{
  this->settings.eepromEnabled = false;
}

void HttpServerAdvanced::enableDebug(bool serial, bool store)
{
  debug.enabled = true;
  debug.serial = serial;
  debug.store = store;
}
