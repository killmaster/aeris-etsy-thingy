#include "wifi_stuff.h"

IPAddress apIP(192,168,1,1);
AsyncWebServer server(80);

String currentSSID, currentPassword;

void changeToStationAndConnect(String ssid, String password){
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  const int ssid_len = ssid.length();
  char* new_ssid = new char[ssid_len + 1];
  strcpy(new_ssid, ssid.c_str());

  const int password_len = password.length();
  char* new_password = new char[password_len + 1];
  strcpy(new_password, password.c_str());

  WiFi.begin(ssid.c_str(), password.c_str());
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    showMessage("Connection to " + ssid);
  }
  showMessage("Connected to: " + ssid + "\nIP: " + WiFi.localIP().toString());
  Serial.println(WiFi.localIP().toString());
}

void saveWifiSettings(AsyncWebServerRequest *request){
  String ssid = String(request->arg("ssid"));
  String password = String(request->arg("password"));
  Serial.println("SSID: " + ssid + "\tPassword: " + password);
  request->send(200);
  changeToStationAndConnect(ssid, password);
}

void webServerSetup(String responseHTML) {
  // POST
  server.on("/", HTTP_POST, saveWifiSettings);

  server.on("/", HTTP_GET, [responseHTML](AsyncWebServerRequest *request){
    request->send(200, "text/html", responseHTML);
    Serial.println("requested \"/\"");
  });

  server.on("/generate_204", HTTP_GET, [responseHTML](AsyncWebServerRequest *request){
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

void setupWiFiStationMode(String& responseHTML){
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  showMessage("Scanning for networks...");

  int n = WiFi.scanNetworks();
  String* ssids = new String[MAX_SSIDS];
  int stop = min(n, MAX_SSIDS);
  for (int i = 0; i < stop; i++){
    ssids[i] = WiFi.SSID(i);
  }

  String ssid_list = "";
  for (int i = 0; i < stop; i++){
    ssid_list += "<li>" + ssids[i] + "</li>";
  }
  responseHTML.replace("$ssid_list", ssid_list);
}

void setupWiFiAPMode(DNSServer &dnsServer){
  showMessage("Starting AP mode.");
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(AP_NAME);
  dnsServer.start(DNS_PORT, "*", apIP);
  showMessage("AP mode on.");
}
