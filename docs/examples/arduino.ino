#include <HttpServerAdvanced.h>

HttpServerAdvanced httpServerAdvanced("YOUR WIFI SSID GOES HERE", "PASSWORD GOES HERE", 80, LED_BUILTIN);

void setup() {
  Serial.begin(115200);
  delay(1000);

  httpServerAdvanced.debug.enabled = true;
  httpServerAdvanced.debug.serial = true;

  httpServerAdvanced.setup();
}

void loop() {
  httpServerAdvanced.loop();
}