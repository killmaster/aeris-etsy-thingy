#ifndef WIFI_STUFF_H 
#define WIFI_STUFF_H

#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <DNSServer.h>

#define AP_NAME "Totally not an honeypot"
const byte DNS_PORT = 53;
const String localIPURL = "http://192.168.1.1";
const int MAX_SSIDS = 30;

void webServerSetup(String responseHTML); 
void setupWiFiStationMode(String &responseHTML);
void setupWiFiAPMode(DNSServer &dnsServer);

#endif