#include "HttpServerAdvanced.h"

HttpServerAdvanced::HttpServerAdvanced(const char* ssid, const char* sskey, int port, int ledPinNumber)
{
  if (ssid) {
    addAccessPoint(ssid, sskey);
  }

  if (port > 0) {
    setServerPort(port);
  }

  if (ledPinNumber >= 0) {
    setStatusLedPin(ledPinNumber);
  }
}

void HttpServerAdvanced::setStatusLedPin(byte ledpin)
{
  statusLed.ledPinNumber = ledpin;
}

void HttpServerAdvanced::setServerPort(int port)
{
  this->port = port;
}

bool HttpServerAdvanced::addAccessPoint(String ssid, String psk, byte priority)
{
  if (ssid.length() > 32 || psk.length() > 64) {
    debug.error("Too long ssid or psk!");
    return false;
  }

  AccessPoint accessPoint;
  accessPoint.priority = priority;
  ssid.toCharArray(accessPoint.ssid, 32);
  psk.toCharArray(accessPoint.psk, 64);

  AccessPoint* accessPointListNew = new AccessPoint[accessPointCounter + 1];
  for (size_t accessPointIndex = 0; accessPointIndex < accessPointCounter; accessPointIndex++) {
    accessPointListNew[accessPointIndex] = accessPointList[accessPointIndex];
  }

  accessPointListNew[accessPointCounter] = accessPoint;

  delete accessPointList;
  accessPointList = accessPointListNew;
  accessPointCounter++;
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

  if (!setupWifi()) {
    debug.error("Setup incomplete.");
    statusLed.turnOff();
    return;
  }

  server = new WiFiServer(port);
  server->begin();

  setupComplete = true;
  debug.info("Setup complete.");
}

bool HttpServerAdvanced::setupWifi()
{
  int numberOfNetworks = WiFi.scanNetworks();
  if (numberOfNetworks == 0) {
    debug.error("No networks found!");
    return false;
  }

  for (int scanIndex = 0; scanIndex < numberOfNetworks; scanIndex++) {
    debug.info(
      "Found AP: '" + WiFi.SSID(scanIndex) + "' rssi: " + WiFi.RSSI(scanIndex)
    );

    for (size_t accessPointIndex = 0; accessPointIndex < accessPointCounter; accessPointIndex++) {
      if (
        String(
          accessPointList[accessPointIndex].ssid
        ).compareTo(
          WiFi.SSID(scanIndex)
        ) == 0
      ) {
        accessPointList[accessPointIndex].found = true;

        // same ssid can be used by multiple APs, and we wan't to store the strongest signal
        int rssi = WiFi.RSSI(scanIndex);
        if (rssi > accessPointList[accessPointIndex].signalStrength) {
          accessPointList[accessPointIndex].signalStrength = rssi;
        }
      }
    }
  }

  AccessPoint selectedAccessPoint;
  for (size_t accessPointIndex = 0; accessPointIndex < accessPointCounter; accessPointIndex++) {
    // If the stored network cannot be found, drop it.
    if (!accessPointList[accessPointIndex].found) {
      continue;
    }

    // If the priority is higher than the selected one's, use that.
    if (accessPointList[accessPointIndex].priority < selectedAccessPoint.priority) {
      selectedAccessPoint = accessPointList[accessPointIndex];
    }

    // If the priority is the same, but the signalStrength is stronger than the selected one's, use that.
    if (
      accessPointList[accessPointIndex].priority == selectedAccessPoint.priority &&
      accessPointList[accessPointIndex].signalStrength > selectedAccessPoint.signalStrength
    ) {
      selectedAccessPoint = accessPointList[accessPointIndex];
    }
  }

  if (!selectedAccessPoint.found) {
    debug.error("None of the stored networks are visible!");
    return false;
  }

  debug.info("Connecting to " + String(selectedAccessPoint.ssid));

  String nodeName = settings.getNodeName();
  if (nodeName.length() == 0) {
    settings.setNodeName("DomNode");
  }

  nodeName = settings.getNodeName();
  debug.info("nodeName: " + nodeName);

  WiFi.mode(WIFI_STA);
  WiFi.begin(selectedAccessPoint.ssid, selectedAccessPoint.psk);

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    statusLed.turnOff();
    delay(250);
    statusLed.turnOn();
    debug.waiting();
  }

  debug.waitingFinished();
  debug.info("WiFi connected, local IP: " + WiFi.localIP().toString());
  return true;
}

void HttpServerAdvanced::loop()
{
  if (!setupComplete) {
    statusLed.turnOff();
    return;
  }

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
  unsigned long startTime = millis();
  int timeOutMillis = 30 * 1000;
  unsigned long timeOutTime = startTime + timeOutMillis;
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

    if (timeOutTime < millis()) {
      debug.waitingFinished();
      debug.warn("No data received within the timeout period.");
      return;
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

  if (request->method == "options") {
    return HttpResponse();
  }

  if (request->uri == "/") {
    if (request->method == "get") {
      return HttpResponse(
        "name: " + settings.getNodeName() + "\r\n" +
        "hsaVersion: " + String(HTTP_SERVER_ADVANCED_VERSION) + "\r\n"
      );
    }

    if (request->method == "post") {
      settings.setNodeName(request->data);
      return HttpResponse();
    }

    return HttpResponse::BadRequest();
  }

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
      request->data != "input" &&
      request->data != "output" &&
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

  // // PATCH
  // if (request->method == "patch") {
  //   if (settings.getPinMode(pinNumber) != OUTPUT) {
  //     return HttpResponse::Unacceptable(
  //       "The pin is not in output mode, can't set state.\r\n" +
  //       getPinData(pinNumber)
  //     );
  //   }

  //   if (!pins.setState(pinNumber, request->data)) {
  //     return HttpResponse::InternalError();
  //   }

  //   return HttpResponse(
  //     getPinData(pinNumber)
  //   );
  // }

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
    return
      "initialized: " +
        String(settings.isPinInitalized(digitalPinNumber), DEC) +
        "\r\n" +
      "locked: " +
        String(settings.isPinLocked(digitalPinNumber), DEC) +
        "\r\n" +
      "state: ?" +
        "\r\n" +
      "mode: ?" +
        "\r\n"
    ;
  }

  return
    "initialized: " +
      String(settings.isPinInitalized(digitalPinNumber), DEC) +
      "\r\n" +
    "locked: " +
      String(settings.isPinLocked(digitalPinNumber), DEC) +
      "\r\n" +
    "state: " +
      String(pins.getState(digitalPinNumber), DEC) +
      "\r\n" +
    "mode: " +
      String(settings.getPinMode(digitalPinNumber), DEC) +
      "\r\n"
  ;
}
