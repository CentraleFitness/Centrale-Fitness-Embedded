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
const int	host_port = 10000;

ESP8266WiFiClass	g_wifi;
wl_status_t			g_status;
WiFiClient			g_client;
Adafruit_INA219		ina219;

double	shuntvoltage = 0;
double	busvoltage = 0;
double	current_mA = 0;
double	loadvoltage = 0;
double	energy = 0;

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
	epd_screen_rotation(EPD_INVERSION);
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
	epd_disp_string((String("Local IP is ") + g_wifi.localIP().toString()).c_str(), 0, 30);
	epd_update();
	delay(1000);
	Serial.println("Local IP	: " + g_wifi.localIP().toString());
	Serial.println("Subnet mask	: " + g_wifi.subnetMask().toString());
	Serial.println("gateway IP	: " + g_wifi.gatewayIP().toString());
	Serial.println("DNS IP		: " + g_wifi.dnsIP().toString());

	// distant host init

	epd_disp_string((String("Trying to reach the host on ") + host_ip).c_str(), 0, 60);
	epd_update();
	delay(1000);
	Serial.print(String("Trying to reach the host on ") + host_ip);
	while (!g_client.connect(host_ip, host_port)) {
		Serial.print('.');
		delay(500);
	}
	Serial.println(" Done !");
//	Serial.println("Local IP	: " + g_client.localIP().toString());
	Serial.println("Host IP		: " + g_client.remoteIP().toString());
	epd_disp_string("Connected to the host ", 0, 90);
	epd_update();
	delay(100);

	g_client.println("The Quick Brown Fox jumps over the Lazy Dog");
	Serial.println("Setup finished");
}

void loop() {
//	g_client.println("The quick brown fox jumps over the lazy dog");
	ina219values();
	//Serial.print(loadvoltage * current_mA);
	//Serial.print("mW	");
	//Serial.print(energy);
	//Serial.println("mWh");

	g_client.print(String("{\"V\": ") + loadvoltage + ", \"A\": " + (current_mA / 1000) + ", \"W\": " + ((loadvoltage * current_mA) / 1000) + "}");
//	g_client.println(String(loadvoltage * current_mA) + "mW, " + energy + "mWh");
	delay(1000);
}
