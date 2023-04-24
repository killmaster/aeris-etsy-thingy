#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <DNSServer.h>
#include <stdio.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32// OLED display height, in pixels

const byte DNS_PORT = 53;
const String localIPURL = "http://192.168.1.1";
IPAddress apIP(192,168,1,1);
DNSServer dnsServer;
AsyncWebServer server(80);

String responseHTML = ""
  "<!DOCTYPE html><html><head><title>HoneyPot's Captive Portal</title></head><body>"
  "<h1>Hello Visitor!</h1><p>This is a totally legit captive portal. No data is being"
  " stollen. Promise!</p></body></html>";

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void webServerSetup() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", responseHTML);
    Serial.println("requested \"/\"");
  });

  server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", responseHTML);
    Serial.println("requested \"/generate_204\"");
  });

  server.onNotFound([](AsyncWebServerRequest *request){
    request->redirect("/");
    Serial.println("server.notfound triggered");
    Serial.println(request->url());
  });


  //Required
	server.on("/connecttest.txt",[](AsyncWebServerRequest *request){request->redirect("http://logout.net");}); //windows 11 captive portal workaround
	server.on("/wpad.dat",[](AsyncWebServerRequest *request){request->send(404);}); //Honestly don't understand what this is but a 404 stops win 10 keep calling this repeatedly and panicking the esp32 :)

	//Background responses: Probably not all are Required, but some are. Others might speed things up?
	//A Tier (commonly used by modern systems)
	server.on("/generate_204",[](AsyncWebServerRequest *request){request->redirect(localIPURL);}); // android captive portal redirect
	server.on("/redirect",[](AsyncWebServerRequest *request){request->redirect(localIPURL);}); //microsoft redirect
	server.on("/hotspot-detect.html",[](AsyncWebServerRequest *request){request->redirect(localIPURL);}); //apple call home
	server.on("/canonical.html",[](AsyncWebServerRequest *request){request->redirect(localIPURL);}); //firefox captive portal call home
	server.on("/success.txt",[](AsyncWebServerRequest *request){request->send(200);}); //firefox captive portal call home
	server.on("/ncsi.txt",[](AsyncWebServerRequest *request){request->redirect(localIPURL);}); //windows call home

	//B Tier (uncommon)
	// server.on("/chrome-variations/seed",[](AsyncWebServerRequest *request){request->send(200);}); //chrome captive portal call home
	// server.on("/service/update2/json",[](AsyncWebServerRequest *request){request->send(200);}); //firefox?
	// server.on("/chat",[](AsyncWebServerRequest *request){request->send(404);}); //No stop asking Whatsapp, there is no internet connection
	// server.on("/startpage",[](AsyncWebServerRequest *request){request->redirect(localIPURL);});


  server.begin();
  Serial.println("Web server started");
}

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

  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("Totally not an honeypot");
  dnsServer.start(DNS_PORT, "*", apIP);
  webServerSetup();  
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
