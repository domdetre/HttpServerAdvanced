#include "Debug.h"

void Debug::log(String data)
{
  if (!enabled) {
    return;
  }

  if (serial) {
    Serial.println(data);
  }

  if (store) {
    this->data += data;
  }
  else {
    this->data = data;
  }
}

String Debug::get()
{
  return data;
}

void Debug::waiting()
{
  Serial.print(".");
}

void Debug::waitingFinished()
{
  Serial.println("|");
}
