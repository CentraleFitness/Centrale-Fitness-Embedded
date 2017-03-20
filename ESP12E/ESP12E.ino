/*
 Name:		ESP12E.ino
 Created:	3/20/2017 12:08:04 PM
 Author:	Kyalma
*/


#include <WiFiUdp.h>
#include <WiFiServer.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ESP8266WiFiType.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFi.h>

// Network hosted on the WRT54GS Router

#define SSID	"centralefitness"
#define	PASS	"epitechforward42A"

ESP8266WiFiClass	g_wifi;
wl_status_t			g_status;

void setup() {
	Serial.begin(9600);
	Serial.println("Boot");

	g_wifi.mode(WIFI_STA);
	g_wifi.begin(SSID, PASS);
	while (g_wifi.status() != WL_CONNECTED) {
		delay(100);
	}
	g_status = g_wifi.status();
	Serial.println("IP Adress: " + g_wifi.localIP().toString());
	g_wifi.setAutoConnect(true);
	Serial.println("Setup finihed");
}

void loop() {
	delay(1000);
}
