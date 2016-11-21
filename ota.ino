/**
 * httpUpdate.ino
 *
 *  Created on: 27.11.2015
 *
 */

#include <Arduino.h>
#include "credentials.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <EEPROM.h>
#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;
#define firmware "iotupdater"
#define firmware_version firmware"_001"


#define update_server "82.77.158.27"
#define update_uri "/index.php"

#define SSIDBASE 200
#define PASSWORDBASE 220

char ssid[20];
char password[20];

String initSSID = "Brom01";
String initPassword = "Alanis..";

bool readCredentials() {
  EEPROM.begin(512);
  if (EEPROM.read(SSIDBASE - 1) != 0x5)  {
    Serial.println(EEPROM.read(SSIDBASE - 1), HEX);
    initSSID.toCharArray(ssid, initSSID.length() + 1);
    for (int ii = 0; ii <= initSSID.length(); ii++) EEPROM.write(SSIDBASE + ii, ssid[ii]);

    initPassword.toCharArray(password, initPassword.length() + 1);
    for (int ii = 0; ii <= initPassword.length(); ii++) EEPROM.write(PASSWORDBASE + ii, password[ii]);
    EEPROM.write(SSIDBASE - 1, 0x35);
  }
  int i = 0;
  do {
    ssid[i] = EEPROM.read(SSIDBASE + i);
    i++;
  } while (ssid[i - 1] > 0 && i < 20);

  if (i == 20) Serial.println("ssid loaded");
  i = 0;
  do {
    password[i] = EEPROM.read(PASSWORDBASE + i);
    i++;
  } while (password[i - 1] != 0 && i < 20);
  if (i == 20) Serial.println("Pass loaded");
  EEPROM.end();
}


void printMacAddress() {
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  for (int i = 0; i < 5; i++) {
    Serial.print(mac[i], HEX);
    Serial.print(":");
  }
  Serial.println(mac[5], HEX);
}

void iotUpdater(bool debug) {
  if (debug) {
    printMacAddress();
    Serial.println("start flashing......");
    Serial.println(update_server);
    Serial.println(update_uri);
    Serial.println(firmware_version);
  }

  t_httpUpdate_return ret = ESPhttpUpdate.update(update_server, 8888, update_uri, firmware_version);
  switch (ret) {
    case HTTP_UPDATE_FAILED:
      if (debug) Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      if (debug) Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;

    case HTTP_UPDATE_OK:
      if (debug) Serial.println("HTTP_UPDATE_OK");
      break;
  }
}

void setup() {

  Serial.begin(115200);
  Serial.println("");
  Serial.println("Start");
  pinMode(BUILTIN_LED, OUTPUT);  // initialize onboard LED as output
  readCredentials();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("connected");

  iotUpdater(true);
}




void loop() {


//////////////////////////////////////////////////////////////////////
  digitalWrite(BUILTIN_LED, HIGH);  // turn on LED with voltage HIGH
  delay(100);                      // wait one second
  digitalWrite(BUILTIN_LED, LOW);   // turn off LED with voltage LOW
  delay(100);                      // wait one second
}
