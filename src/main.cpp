#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <stdio.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "wifi_stuff.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

DNSServer dnsServer;

String responseHTML = ""
  "<!DOCTYPE html><html><head><title>HoneyPot's Captive Portal</title></head><body>"
  "<h1>Hello Visitor!</h1><p>This is a totally legit captive portal. No data is being"
  " stollen. Promise!</p><ul>$ssid_list</ul></body></html>";

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void displaySetup() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  delay(2000);
  display.clearDisplay();
  display.dim(0x47);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Starting honeypot.");
  display.display();
  delay(2000);
}


void setup()
{
  Serial.begin(115200);
  setupWiFiStationMode(responseHTML);
  setupWiFiAPMode(dnsServer);
  webServerSetup(responseHTML);  
  delay(100);
  displaySetup();
}

void loop() {
  Serial.println(WiFi.softAPgetStationNum());

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Honeypot is on.");
  display.print(WiFi.softAPgetStationNum());
  display.println(" targets connected.");
  display.display();

  dnsServer.processNextRequest();
}
