#include <Arduino.h>
#include <ArduinoJson.h>
#include <stdio.h>
#include "wifi_stuff.h"
#include "oled.h"


DNSServer dnsServer;

String responseHTML = ""
  "<!DOCTYPE html><html><head><title>HoneyPot's Captive Portal</title></head><body>"
  "<h1>Hello Visitor!</h1><p>This is a totally legit captive portal. No data is being"
  " stollen. Promise!</p><ul>$ssid_list</ul></body></html>"
  "<form action=\"\" method=\"post\">"
  "<div>"
  "<label for=\"ssid\">SSID: </label>"
  "<input type=\"text\" name=\"ssid\" id=\"ssid\" required>"
  "</div>"
  "<div>"
  "<label for=\"password\">Password: </label>"
  "<input type=\"password\" name=\"password\" id=\"password\">"
  "</div>"
  "<div><input type=\"submit\" value=\"Save!\"></div>";

void setup()
{
  Serial.begin(115200);
  setupDisplay();
  setupWiFiStationMode(responseHTML);
  setupWiFiAPMode(dnsServer);
  webServerSetup(responseHTML);
  delay(100);
}

void loop() {
  dnsServer.processNextRequest();
}
