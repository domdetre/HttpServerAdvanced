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
  if (debug) {
    delay(500);
    Serial.println("Booting... ");
  }

  delay(10);

  statusLed.setup();

  eeData.setup();
  if (!eeData.isNew) {
    restorePinModesAndStates();
  }

  if (debug) {
    Serial.print("Connecting to " + String(ssid));
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, sskey);

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);

    statusLed.turnOff();

    delay(250);

    statusLed.turnOn();


    if (debug) {
      Serial.print(".");
    }
  }

  if (debug) {
    Serial.println();
    Serial.println("WiFi connected");
  }

  server->begin();

  if (debug) {
    Serial.println("Server started");
    Serial.println(WiFi.localIP());
  }
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
    }
  }
}

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
HttpResponse HttpServerAdvanced::processRequest(HttpRequest* request)
{
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
      return HttpResponse(
        String(
          getPinState(strPinNumber)
        )
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

  return HttpResponse::NotFound();
}

byte HttpServerAdvanced::convertPin(String strPinNumber)
{
  String numbers = "0123456789";

  if (strPinNumber.length() > 2 || strPinNumber.length() == 0) {
    return -1;
  }

  if (numbers.indexOf(strPinNumber[0]) < 0 || numbers.indexOf(strPinNumber[1]) < 0) {
    return -1;
  }

  return numbers.toInt();
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
  if (pinNumber < 0) {
    return 0;
  }

  // if the pin is input read the value of it
  if (isPinInput(pinNumber)) {
    return digitalRead(pinNumber);
  }

  // otherwise return the stored value
  return eeData.getPinState(pinNumber);
}

bool HttpServerAdvanced::setPinState(String strPinNumber, String strPinState)
{
  byte pinNumber = convertPin(strPinNumber);
  if (pinNumber < 0) {
    return false;
  }

  // if the pin is in input mode, bail
  if (isPinInput(pinNumber)) {
    return false;
  }

  int state = -1;
  if (strPinState == "0" || strPinState == "low") {
    state = LOW;
  }

  if (strPinState == "1" || strPinState == "high") {
    state = HIGH;
  }

  if (state < 0) {
    return false;
  }

  digitalWrite(pinNumber, state);
  eeData.storePinState(pinNumber, state);
  return true;
}

bool HttpServerAdvanced::initPin(String strPinNumber, String strPinMode)
{
  int pinNumber = convertPin(strPinNumber);
  if (pinNumber < 0) {
    return false;
  }

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
    return false;
  }

  pinMode(pinNumber, mode);
  eeData.storePinMode(pinNumber, mode);
  return true;
}

bool HttpServerAdvanced::isPinInput(byte pinNumber)
{
  return !isPinOutput(pinNumber);
}

bool HttpServerAdvanced::isPinOutput(byte pinNumber)
{
  return eeData.getPinMode(pinNumber) == OUTPUT;
}

void HttpServerAdvanced::restorePinModesAndStates()
{
  for (byte pinNumber = 0; pinNumber < 255; pinNumber++) {
    // if it is an output pin, set the mode to output, get the stored state and write that out
    if (isPinOutput(pinNumber)) {
      pinMode(pinNumber, OUTPUT);
      digitalWrite(pinNumber, eeData.getPinState(pinNumber));
      continue;
    }

    // otherwise just set the pinmode to the stored mode
    pinMode(pinNumber, eeData.getPinMode(pinNumber));
  }
}
