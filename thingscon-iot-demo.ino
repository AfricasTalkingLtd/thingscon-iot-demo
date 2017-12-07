/*
 Name:		ThingsConDemoWiFi101.ino
 Created:	12/5/2017 11:52:51 AM
 Author:	Africas Talking
 Simple IOT Demo for the ThingsCon
*/

#include <SPI.h>
#include <WiFi101.h>
#include <AfricasTalkingCloud.h>

#define sensorPin A0
const char ssid[] = "My Super Fast Internet";
const char password[] = "SuperSecretPassowrd";
const byte idleLight = 4;
const byte featureLight = 8;
char message[50];
long messageDuration = 0;
long reading = 0;
long powerOffTime = 0;
String topicOrigin = "";

void thingsConCallBack(char topic[], byte* payload, unsigned int length);
const char deviceID[] = "ThingsCon";
const char deviceUsername[] = "<username>";
const char devicePassword[] = "<password>";
WiFiClient thingsCon;
AfricasTalkingCloudClient client(thingsConCallBack, thingsCon);

int status = WL_IDLE_STATUS;

void setup() {
	WiFi.setPins(10, 3, 5);
	Serial.begin(9600);
	while (!Serial)
	{
		;
	}
	connectToWPA();
	pinMode(sensorPin, INPUT);
	pinMode(idleLight, OUTPUT);
	pinMode(featureLight, OUTPUT);
	digitalWrite(idleLight, HIGH);
	digitalWrite(featureLight, HIGH);
	delay(1500);
	digitalWrite(idleLight, LOW);
	digitalWrite(featureLight, LOW);
}


void loop() {
	if (!client.connected())
	{
		reconnectDevice();
	}
	if (!client.loop())
	{
		Serial.println(F("Device Disconnected"));
		if (client.connect(deviceID, deviceUsername, devicePassword))
		{
			Serial.println(F("Reconnected..."));
		}
		else
		{
			Serial.println(F("Failed"));
		}
	}
	reading = analogRead(sensorPin);
	long now = millis();
	if (now - messageDuration > 5000)
	{
		messageDuration = now;
		snprintf(message, 75, "%ld", reading);
		client.publish("<username>/demo/readings", message, 1, false);
		Serial.print(F("Just sent: "));
		Serial.println(message);
	}
}

void thingsConCallBack(char topic[], byte * payload, unsigned int length)
{
	digitalWrite(featureLight, LOW);
	digitalWrite(idleLight, LOW);
	String response = "";
	topicOrigin = (String)topic;
	Serial.print("Message arrived [");
	Serial.print(topic);
	Serial.print("] ");
	for (int i = 0; i < length; i++)
	{
		Serial.print((char)payload[i]);
		response += (char)payload[i];
	}
	//response = (String)response;
	Serial.println();
	Serial.println(response);
	if (topicOrigin == "<username>/demo/commands" && response == "OFF")
	{
		digitalWrite(idleLight, LOW);
	} else if (topicOrigin == "<username>/demo/commands" && response == "ON")
	{
		digitalWrite(idleLight, HIGH);
	}
	else if (topicOrigin == "<username>/demo/pay")
	{
		int onDuration = response.toInt();
		digitalWrite(featureLight, HIGH);
		delay(onDuration);
		digitalWrite(featureLight, LOW);
	}
}

void printCurrentNet() {
	Serial.print("SSID: ");
	Serial.println(WiFi.SSID());
	byte bssid[6];
	WiFi.BSSID(bssid);
	Serial.print("BSSID: ");
	Serial.print(bssid[5], HEX);
	Serial.print(":");
	Serial.print(bssid[4], HEX);
	Serial.print(":");
	Serial.print(bssid[3], HEX);
	Serial.print(":");
	Serial.print(bssid[2], HEX);
	Serial.print(":");
	Serial.print(bssid[1], HEX);
	Serial.print(":");
	Serial.println(bssid[0], HEX);
	long rssi = WiFi.RSSI();
	Serial.print("signal strength (RSSI):");
	Serial.println(rssi);
	byte encryption = WiFi.encryptionType();
	Serial.print("Encryption Type:");
	Serial.println(encryption, HEX);
	Serial.println();
}
void getAddress() {
	IPAddress ip = WiFi.localIP();
	Serial.print("IP Address: ");
	Serial.println(ip);
	Serial.println(ip);
	byte mac[6];
	WiFi.macAddress(mac);
	Serial.print("MAC address: ");
	Serial.print(mac[5], HEX);
	Serial.print(":");
	Serial.print(mac[4], HEX);
	Serial.print(":");
	Serial.print(mac[3], HEX);
	Serial.print(":");
	Serial.print(mac[2], HEX);
	Serial.print(":");
	Serial.print(mac[1], HEX);
	Serial.print(":");
	Serial.println(mac[0], HEX);
}
void connectToWPA()
{
	Serial.println(F("Setting up connection...."));
	if (WiFi.status() == WL_NO_SHIELD)
	{
		Serial.println("WiFi shield not present");
		while (true);
	}
	while (status != WL_CONNECTED)
	{
		Serial.print("Attempting to connect to WPA SSID: ");
		Serial.println(ssid);
		status = WiFi.begin(ssid, password);
		delay(10000);
	}
	Serial.print("You're connected to the network");
	printCurrentNet();
	getAddress();
}
void reconnectDevice() {

	while (!client.connected()) {
		Serial.print(F("Contacting Africa's Talking: "));
		Serial.println();
		if (client.connect(deviceID, deviceUsername, devicePassword))
		{
			Serial.println(F("Connected"));
			client.publish("<username>/demo/birth", "ThingsCon");
			client.subscribe("<username>/demo/commands");
			client.subscribe("<username>/demo/pay");
		}
		else {
			Serial.print(F("Failed, Code: "));
			Serial.print(client.state());
			Serial.println(F(" Retrying..."));
			delay(6000);
		}
	}
}