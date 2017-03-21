/*
 Name:		ESP12E.ino
 Created:	3/20/2017 12:08:04 PM
 Author:	Kyalma
*/

#include <Adafruit_INA219.h>
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

#include "inc/epd.h"

// Network hosted on the WRT54GS Router
#define SSID	"centralefitness"
#define	PASS	"epitechforward42A"

// Server
const char*	host_ip = "192.168.1.101";
const int host_port = 5555;

ESP8266WiFiClass	g_wifi;
wl_status_t			g_status;
WiFiClient			g_client;
Adafruit_INA219		ina219;

double	shuntvoltage;
double	busvoltage;
double	current_mA;
double	loadvoltage;
double	energy;

void ina219values() {
	shuntvoltage = ina219.getShuntVoltage_mV();
	busvoltage = ina219.getBusVoltage_V();
	current_mA = ina219.getCurrent_mA();
	loadvoltage = busvoltage + (shuntvoltage / 1000);
	energy += loadvoltage * current_mA / 3600;
}

void setup() {
	Serial.begin(9600);
	Serial.println("<-- Power ON -->");

	// Power meter
	ina219.begin();
	Serial.println("Power meter started");

	// e-Paper init
	epd_init();
	epd_wakeup();
	epd_set_memory(MEM_NAND);
	Serial.println("E-Paper initialized");

	// wifi connection init
	g_wifi.mode(WIFI_STA);
	g_wifi.begin(SSID, PASS);
	Serial.print(String("Connecting to ") + SSID);
	while ((g_status = g_wifi.status()) != WL_CONNECTED) {
		delay(500);
		Serial.print('.');
	}
	Serial.println("Done !");
	g_wifi.setAutoReconnect(true);
	epd_disp_string(("Connected to the network SSID " + String(SSID)).c_str(), 0, 0);
	epd_update();
	delay(100);
	Serial.println("Local IP	: " + g_wifi.localIP().toString());
	Serial.println("Subnet mask	: " + g_wifi.subnetMask().toString());
	Serial.println("gateway IP	: " + g_wifi.gatewayIP().toString());
	Serial.println("DNS IP		: " + g_wifi.dnsIP().toString());

	// distant host init
	while (!g_client.connect(host_ip, host_port)) {
		Serial.println(String("Server connection failed at ") + host_ip + ':' + host_port);
		delay(500);
	}
	Serial.println("Server connection successful");
//	Serial.println("Local IP	: " + g_client.localIP().toString());
	Serial.println("Host IP		: " + g_client.remoteIP().toString());
	epd_disp_string((String("Connected to the host ") + host_ip).c_str(), 0, 30);
	epd_update();
	delay(100);


	Serial.println("Setup finished");
}

void loop() {
//	g_client.println("The quick brown fox jumps over the lazy dog");
	ina219values();
	Serial.print(loadvoltage * current_mA);
	Serial.print("mW	");
	Serial.print(energy);
	Serial.println("mWh");
	g_client.println(String(loadvoltage * current_mA) + "mW, " + energy + "mWh");
	delay(1000);
}
