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

  statusLed.setup();

  settings.setup();
  if (settings.hasDataRestored()) {
    restorePinModesAndStates();
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
  debug.info("Converting pin: " + strPinNumber);

  bool digital = false;
  if (strPinNumber.indexOf("d") == 0) {
    debug.info("Pin format is D#");
    digital = true;
    strPinNumber.remove(0, 1);
  }

  String numbers = "0123456789";

  // The length of the number can only be 1 or 2 digits
  if (strPinNumber.length() > 2 || strPinNumber.length() == 0) {
    debug.error("Too many or too few digits: " + String(strPinNumber.length()));
    return 255;
  }

  if (numbers.indexOf(strPinNumber[0]) < 0 || numbers.indexOf(strPinNumber[1]) < 0) {
    debug.error("Digits contain non-numeric character");
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

  debug.error("D# pin format is out of range. Range: 0-15");
  return 255;
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

byte HttpServerAdvanced::getPinState(String strPinNumber)
{
  debug.info("Getting the state of pin: " + strPinNumber);

  byte pinState = 0;
  byte pinNumber = convertPin(strPinNumber);
  if (pinNumber == 255) {
    return 255;
  }

  debug.info("GPIO pin number: " + String(pinNumber));

  // if the pin is input mode, read the value of it
  if (isPinInput(pinNumber)) {
    pinState = digitalRead(pinNumber);
    debug.info("Pin is in input mode, read state: " + String(pinState));
    return pinState;
  }

  // otherwise return the stored value
  pinState = settings.getPinState(pinNumber);
  debug.info("Pin is in output mode, stored state: " + String(pinState));
  return pinState;
}

bool HttpServerAdvanced::setPinState(String strPinNumber, String strPinState)
{
  debug.info("Setting pin state of " + strPinNumber + " to " + strPinState);

  byte pinNumber = convertPin(strPinNumber);
  if (pinNumber == 255) {
    return false;
  }

  debug.info("GPIO pin number: " + String(pinNumber));

  // if the pin is in input mode, bail
  if (isPinInput(pinNumber)) {
    debug.warn("The pin is in input mode!");
    return false;
  }

  int state = -1;
  if (strPinState == "0" || strPinState == "low") {
    state = LOW;
  }

  if (strPinState == "1" || strPinState == "high") {
    state = HIGH;
  }

  debug.info("Requested pin state: " + String(state));

  if (state < 0) {
    debug.warn("The requested state is invalid! Accepted values: 0, 1, low, high.");
    return false;
  }

  digitalWrite(pinNumber, state);
  settings.storePinState(pinNumber, state);
  return true;
}

bool HttpServerAdvanced::initPin(String strPinNumber, String strPinMode)
{
  debug.info("Initializing pin " + strPinNumber + " with mode " + strPinMode);

  int pinNumber = convertPin(strPinNumber);
  if (pinNumber == 255) {
    return false;
  }

  debug.info("GPIO pin number: " + String(pinNumber));

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
    debug.error("Requested pin mode is invalid! Only input, output, input_pullup are accepted.");
    return false;
  }

  pinMode(pinNumber, mode);
  settings.storePinMode(pinNumber, mode);
  debug.info("Pin initialized with mode " + String(mode));
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
  debug.info("Restoring pin modes and states.");

  for (byte pinNumber = 0; pinNumber < 255; pinNumber++) {
    // if it is an output pin, set the mode to output, get the stored state and write that out
    if (isPinOutput(pinNumber)) {
      byte pinState = settings.getPinState(pinNumber);
      pinMode(pinNumber, OUTPUT);
      digitalWrite(pinNumber, pinState);
      debug.info("Restored pin " + String(pinNumber) + " with mode output and state " + String(pinState));
      continue;
    }

    // otherwise just set the pinmode to the stored mode
    pinMode(pinNumber, settings.getPinMode(pinNumber));
    debug.info("Restored pin " + String(pinNumber) + " with mode input or input_pullup");
  }
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
