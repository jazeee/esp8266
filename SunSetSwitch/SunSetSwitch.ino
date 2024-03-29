#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <time.h>
#include "./time-utils.h"
#include "./index.h"
#include "/home/jsingh/private/wireless.h"

// #define IP_ADDRESS_SUFFIX 204

const char* ssid = PRIVATE_SSID;
const char* password = PRIVATE_PSK;

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", INDEX_HTML);
}

const int relay = 0;
const int invertRelayLogic = 1;
bool relayState = false;
const String setRelayState(bool newRelayState) {
	bool oldRelayState = relayState;
  relayState = newRelayState;
	const String output = relayState ? "Relay is on" : "Relay is off";
	if (oldRelayState != relayState) {
		digitalWrite(relay, invertRelayLogic - relayState ? 1 : 0);
  	Serial.println(output);
	}
	return output;
}
void toggleRelay() {
	const String output = setRelayState(!relayState);
  server.send(200, "text/plain", output);
}
void turnRelayOn() {
	const String output = setRelayState(true);
  server.send(200, "text/plain", output);
}
void turnRelayOff() {
	const String output = setRelayState(false);
  server.send(200, "text/plain", output);
}

void getTime() {
  time_t now = time(nullptr);
  server.send(200, "text/plain", ctime(&now));
}

void getSunSetTime() {
  server.send(200, "text/plain", String(getSunSetMinutesFromMidnight(), 5) + " " + String(getSunSetMinutesFromNow(), 5));
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {
  pinMode(relay, OUTPUT);
  setRelayState(false);

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  #ifdef IP_ADDRESS_SUFFIX
	  IPAddress ip(10, 2, 1, IP_ADDRESS_SUFFIX);
		IPAddress gateway(10, 2, 1, 1);
		IPAddress subnet(255, 255, 255, 0);
		IPAddress dns(10, 2, 1, 1);
		WiFi.config(ip, dns, gateway, subnet);
	#endif
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/time", getTime);
  server.on("/time/sun-set", getSunSetTime);
  server.on("/toggle-relay", toggleRelay);
  server.on("/relay/on", turnRelayOn);
  server.on("/relay/off", turnRelayOff);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  configTime(-8 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  setRelayState(true);
  delay(2000);
  setRelayState(false);
}

bool lastSunSetTransition = false;
void processSunSet() {
  time_t now = time(nullptr);
  struct tm* tmInfo = localtime(&now);
  bool nextSunSetTransition = false;
	double minutesFromMidnight = (tmInfo -> tm_hour * 60.0 + tmInfo -> tm_min);
  double sunSetMinutesFromNow = getSunSetMinutesFromNow();
	if (sunSetMinutesFromNow < 30 && minutesFromMidnight < 22 * 60) {
		nextSunSetTransition = true;
	}
	if (lastSunSetTransition != nextSunSetTransition) {
		setRelayState(nextSunSetTransition);
		lastSunSetTransition = nextSunSetTransition;
	}
}

void loop(void) {
  server.handleClient();
  MDNS.update();
  processSunSet();
}
