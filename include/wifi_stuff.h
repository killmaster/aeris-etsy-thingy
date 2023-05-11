#ifndef WIFI_STUFF_H 
#define WIFI_STUFF_H

#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <DNSServer.h>
#include <Arduino.h>
#include "oled.h"

#define AP_NAME "Dropship"
const byte DNS_PORT = 53;
const String localIPURL = "http://192.168.1.1";
const int MAX_SSIDS = 30;
extern bool captivePortal;

void webServerSetup(String responseHTML); 
void setupWiFiStationMode(String &responseHTML);
void setupWiFiAPMode(DNSServer &dnsServer);
void setupWiFiAPStationMode(DNSServer &dnsServer, String& responseHTML);

#endif
