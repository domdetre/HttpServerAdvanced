#include "Debug.h"
#include "HttpServerAdvanced.h"

void Debug::log(String data)
{
  if (!enabled) {
    return;
  }

  if (serial) {
    String trail = "";
    if (data.indexOf('\n') >= 0) {
      trail = "\n---------------------";
    }

    Serial.println(String(HTTP_SERVER_ADVANCED_NAME) + String(": ") + data + trail);
  }

  if (store) {
    this->data += data;
  }
  else {
    this->data = data;
  }
}

void Debug::info(String data)
{
  if (infoLogs) {
    log("[INFO] " + data);
  }
}

void Debug::warn(String data)
{
  if (warningLogs) {
    log("[WARNING] " + data);
  }
}

void Debug::error(String data)
{
  if (errorLogs) {
    log("[ERROR] " + data);
  }
}

String Debug::get()
{
  String data = this->data;
  this->data = "";
  return data;
}

void Debug::waiting()
{
  if (enabled) {
    Serial.print(".");
  }
}

void Debug::waitingFinished()
{
  if (enabled) {
    Serial.println("|");
  }
}
