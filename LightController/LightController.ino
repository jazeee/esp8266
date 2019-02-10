#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <time.h>
#include "./index.h"
#include "/home/jsingh/private/wireless.h"

const char* ssid = PRIVATE_SSID;
const char* password = PRIVATE_PSK;

const int LED = 15;
const int STROBE_ON_DO = 5;
const int STROBE_OFF_DO = 16;
const int SWITCH_1_DI = 4;
const int SWITCH_2_DI = 0;
const int SWITCH_3_DI = 2;
const int SWITCH_4_DI = 14;
const int SWITCH_5_DI = 12;
const int SWITCH_6_DI = 13;
const int SWITCH_INPUTS[] = {
	SWITCH_1_DI,
	SWITCH_2_DI,
	SWITCH_3_DI,
	SWITCH_4_DI,
	SWITCH_5_DI,
	SWITCH_6_DI,
};
const int SWITCH_COUNT = sizeof(SWITCH_INPUTS) / sizeof(int);
const int URL_SUFFICES[SWITCH_COUNT] = {
	200,
	208,
	202,
	203,
	204,
	205,
};
int SWITCH_REQUEST_IN_MS[SWITCH_COUNT] = {
	0,
	0,
	0,
	0,
	0,
	0,
};

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", INDEX_HTML);
}

void getTime() {
  time_t now = time(nullptr);
  server.send(200, "text/plain", ctime(&now));
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
  pinMode(LED, OUTPUT);
  digitalWrite(LED, 1);
  pinMode(STROBE_ON_DO, OUTPUT);
  pinMode(STROBE_OFF_DO, OUTPUT);
  digitalWrite(STROBE_ON_DO, 1);
  digitalWrite(STROBE_OFF_DO, 1);
  for ( int i = 0; i < SWITCH_COUNT ; i++ ){
  	pinMode(SWITCH_INPUTS[i], INPUT_PULLUP);
  }

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

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  configTime(-8 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  digitalWrite(LED, 0);
}

unsigned long  previousMillis = 0;
#define DURATION 512
int start = 0;
int end = DURATION;
boolean isRising = true;
void heartBeat() {
  unsigned long currentMillis = millis();
  // map it to the range of the analog out:
  int outputValue = map(currentMillis - previousMillis, start, end, 0, 255);
  // change the analog out value:
  analogWrite(LED, outputValue);
//  Serial.print("\t output = ");
//  Serial.println(outputValue);
	int delayTime = isRising ? DURATION : 8 * DURATION;
  if(currentMillis - previousMillis > delayTime){
    previousMillis = currentMillis;
    int temp = end;
    end = start;
    start = temp;
    isRising = !isRising;
  }
}

void postToUrl(String& url) {
  WiFiClient client;

	HTTPClient http;
  if (http.begin(client, url)) {
  	Serial.println("Posting to " + url);
  	int httpCode = http.POST("");
  	if (httpCode > 0) {
  		Serial.println("Post Result: " + String(httpCode));
  	} else {
  		Serial.println("Failed: " + String(httpCode));
  	}
  	http.end();
  } else {
  	Serial.println("Unable to connect");
  }
}

void probeForSwitches(bool state) {
	int pin = state ? STROBE_ON_DO : STROBE_OFF_DO;
  digitalWrite(pin, 0);
  delay(1);
  unsigned long currentMillis = millis();
  for ( int i = 0; i < SWITCH_COUNT ; i++ ){
  	bool isPressed = digitalRead(SWITCH_INPUTS[i]) == 0;
  	if (isPressed) {
  		if (currentMillis - SWITCH_REQUEST_IN_MS[i] < 1000) {
  			continue;
  		}
  		SWITCH_REQUEST_IN_MS[i] = currentMillis;
//  		String url = "http://10.2.1." + String(URL_SUFFICES[i]) + "/relay/";
//  		url += state ? "on" : "off";
  		String url = "http://10.2.1." + String(URL_SUFFICES[i]) + "/toggle-relay";
  		Serial.println("Hitting " + url);
  		digitalWrite(LED, 1);
  		postToUrl(url);
  		digitalWrite(LED, 0);
  	}
  }
  digitalWrite(pin, 1);
}

void loop(void) {
  server.handleClient();
  MDNS.update();
  heartBeat();
  probeForSwitches(true);
  probeForSwitches(false);
  delay(1);
}
