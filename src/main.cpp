#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <stdio.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32// OLED display height, in pixels

const byte DNS_PORT = 53;
IPAddress apIP(192,168,1,1);
DNSServer dnsServer;
WiFiServer server(80);

String responseHTML = ""
  "<!DOCTYPE html><html><head><title>HoneyPot's Captive Portal</title></head><body>"
  "<h1>Hello Visitor!</h1><p>This is a totally legit captive portal. No data is being"
  " stollen. Promise!</p></body></html>";

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/*
void newDisplayMessage(char text[]){
  display.clearDisplay();
  display.println(text);
  display.display();
}
*/

void setup()
{
  Serial.begin(115200);

  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("Totally not an honeypot");

  dnsServer.start(DNS_PORT, "*", apIP);

  server.begin();
  delay(100);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Starting honeypot.");
  display.display();
  delay(2000);
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
  WiFiClient client = server.available();

  if (client) {
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("");
            client.println(responseHTML);
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
  }

}
