#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <RCSwitch.h>
#include <EEPROM.h>

const int DEBUG_LED = 13;
const int TX_PIN = 4;
const int RX_PIN = 5;

// Handler defs from "handlers.ino"
void handleRoot();
void handleSetup();
void handleQuery();
void handleWriteDevices();
void handleReadDevices();
void handleCmd();
void handleCmd2();
void handleListen();
void handleNotFound();

WiFiManager wifiManager;
ESP8266WebServer server(80);
RCSwitch rcSwitch = RCSwitch();

String getDeviceName() {
  return "ESPRFSwitch:" + String(ESP.getChipId());
}

void wifi_auto() {
  wifiManager.autoConnect(getDeviceName().c_str());

  wifiManager.setAPCallback(configModeCallback);
}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());

  Serial.println(myWiFiManager->getConfigPortalSSID());
}

String getConfigJson() {
  uint8_t len = EEPROM.read(0);
  String buf = "";

  Serial.println(String("Reading EEPROM ") + len);

  for (size_t i = 0; i < len; i++) {
    char b = (char)EEPROM.read(i + 1);
    buf += b;
  }

  return buf;
}

void handshake() {
  if (!MDNS.begin(getDeviceName().c_str())) {
    Serial.println("Error setting up MDNS responder!");
  }

  Serial.println("mDNS responder started");
  MDNS.addService("esp", "tcp", 80); // Announce esp tcp service on port 80
}

void setup(void) {
  EEPROM.begin(512);

  Serial.begin(115200);
  pinMode(DEBUG_LED, OUTPUT);

  rcSwitch.enableTransmit(TX_PIN);
  rcSwitch.enableReceive(digitalPinToInterrupt(RX_PIN));

  wifi_auto();

  Serial.println("Configured");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/setup", handleSetup);
  server.on("/query", handleQuery);
  server.on("/cmd", handleCmd);
  server.on("/cmd2", handleCmd2);
  server.on("/listen", handleListen);

  server.on("/devices.json", HTTP_GET, handleReadDevices);
  server.on("/devices.json", HTTP_POST, handleWriteDevices);

  server.onNotFound(handleNotFound);

  server.begin();

  Serial.println("HTTP server started");

  handshake();
}

void loop(void){
  server.handleClient();
}
