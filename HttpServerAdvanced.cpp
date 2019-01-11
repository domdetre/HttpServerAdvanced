#include "HttpServerAdvanced.h"

HttpServerAdvanced::HttpServerAdvanced(const char* ssid, const char* sskey, int port, int ledPin)
{
  init(ssid, sskey, port, ledPin);
}

void HttpServerAdvanced::init(const char* ssid, const char* sskey, int port, int ledPin)
{
  this->ssid = ssid;
  this->sskey = sskey;
  this->port = port;
  this->ledPin = ledPin;

  this->server = new WiFiServer(port);
}

void HttpServerAdvanced::setup(const char* ssid, const char* sskey, int port, int ledPin)
{
  init(ssid, sskey, port, ledPin);
  setup();
}

void HttpServerAdvanced::setup()
{
  if (debug) {
    delay(500);
    Serial.println("Booting... ");
  }

  delay(10);

  if (ledPin >= 0) {
    pinMode(ledPin, OUTPUT);
    turnOnLed();
  }

  if (debug) {
    Serial.print("Connecting to " + String(ssid));
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, sskey);

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);

    if (ledPin >= 0) {
      turnOffLed();
    }

    delay(250);

    if (ledPin >= 0) {
      turnOnLed();
    }

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

  waitForClient(client);

  readClient(client);

  String response = processRequest();

  client.flush();

  response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n" + response;

  client.print(response);
  delay(1);
}

void HttpServerAdvanced::waitForClient(WiFiClient client)
{
  int counter = 0;
  while (!client.available()) {
    delay(1);
    counter++;

    if (counter > 250) {
      if (ledPin >= 0) {
        turnOffLed();
      }
    }

    if (counter > 500) {
      if (ledPin >= 0) {
        turnOnLed();
      }

      counter = 0;
    }
  }
}

void HttpServerAdvanced::readClient(WiFiClient client)
{
  String request = "";
  while (client.available()) {
    request += char(client.read());
  }

  request.toLowerCase();

  int delimIndex = request.indexOf(' ');
  method = request.substring(delimIndex);
  request.remove(0, delimIndex + 1);

  delimIndex = request.indexOf(' ');
  uri = request.substring(delimIndex);
  request.remove(0, delimIndex + 1);

  delimIndex = request.indexOf('\n');
  String protocoll = request.substring(delimIndex);
  request.remove(0, delimIndex + 1);

  delimIndex = request.indexOf("\r\n\r\n");
  String headers = request.substring(delimIndex);
  data = request.substring(delimIndex + 4);
}

String HttpServerAdvanced::sendToSerial()
{
  Serial.println(method + ' ' + uri);
  if (Serial.available()) {
    return Serial.readStringUntil('\n');
  }

  return String();
}

void HttpServerAdvanced::turnOnLed()
{
  digitalWrite(ledPin, LOW);
}

void HttpServerAdvanced::turnOffLed()
{
  digitalWrite(ledPin, HIGH);
}

String HttpServerAdvanced::processRequest()
{
  //serial
  if (uri == "/serial") {
    if (method == "get") {
      return readSerial();
    }
    if (method == "post") {
      writeSerial(data);
      return "";
    }

    return "";
  }

  //digital/{pinNumber}
  if (uri.indexOf("/digital/") == 0) {
    String strPinNumber = uri.substring(9);

    if (method == "get") {
      return getPinState(strPinNumber);
    }

    if (method == "post") {
      setPinState(strPinNumber, data);
      return "";
    }

    if (method == "put") {
      initPin(strPinNumber, data);
      return "";
    }

    return "";
  }
}

int HttpServerAdvanced::convertPin(String strPinNumber)
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

String HttpServerAdvanced::getPinState(String strPinNumber)
{
  int pinNumber = convertPin(strPinNumber);
  if (pinNumber < 0) {
    return "";
  }

  //TODO check the stored pinmode and pinstate if output

  return String(digitalRead(pinNumber));
}

void HttpServerAdvanced::setPinState(String strPinNumber, String strPinState)
{
  int pinNumber = convertPin(strPinNumber);
  if (pinNumber < 0) {
    return;
  }

  //TODO check the stored pinmode and pinstate if output

  int state = -1;
  if (strPinState == "0" || strPinState == "low") {
    state = LOW;
  }

  if (strPinState == "1" || strPinState == "high") {
    state = HIGH;
  }

  if (state < 0) {
    return;
  }

  digitalWrite(pinNumber, state);

  //TODO store pinstate
}

void HttpServerAdvanced::initPin(String strPinNumber, String strPinMode)
{
  int pinNumber = convertPin(strPinNumber);
  if (pinNumber < 0) {
    return;
  }

  //TODO check the stored pinmode and pinstate if output

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
    return;
  }

  pinMode(pinNumber, mode);

  //TODO store pinmode
}
