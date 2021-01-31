#include <EEPROM.h>

#include <PID_v1.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <PID_v1.h>
#include "max6675.h"

#define RELAY_PIN 12
#define START_ADR 1
#define initByteADRES 0
#define STRING_MAXLENGTH 20

String APSSID = "MyWiFi";
String APPSK = "qweqweqweasd";

const char *ssid = "ESPap1";
const char *password = "qweqweqweasd";

ESP8266WebServer server(80);

struct Data {
  int fq0;
  int fq1;
  double currentTemps[4];
  double Output[4];
  double temps[4];
  double Kp[4];
  double Ki[4];
  double Kd[4];
};

union Dat {
  Data d;
  byte b[sizeof(Data)];
};

Dat data;

#include "epr.h"
#include "site.h"

PID myPID1(&data.d.currentTemps[0], &data.d.Output[0], &data.d.temps[0], data.d.Kp[0], data.d.Ki[0], data.d.Kd[0], DIRECT);
PID myPID2(&data.d.currentTemps[1], &data.d.Output[1], &data.d.temps[1], data.d.Kp[1], data.d.Ki[1], data.d.Kd[1], DIRECT);
PID myPID3(&data.d.currentTemps[2], &data.d.Output[2], &data.d.temps[2], data.d.Kp[2], data.d.Ki[2], data.d.Kd[2], DIRECT);
PID myPID4(&data.d.currentTemps[3], &data.d.Output[3], &data.d.temps[3], data.d.Kp[3], data.d.Ki[3], data.d.Kd[3], DIRECT); //switchkeys

int WindowSize = 5000;

void setup() {
  EEPROM.begin(512);
  Serial.begin(115200);
  if (EEPROM.read(initByteADRES) == 42) {
    readSetting();
    readWiFiSetting();

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(APSSID);

    WiFi.mode(WIFI_STA);
    WiFi.begin(APSSID, APPSK);

    for (int i = 0; (i < 50) && (WiFi.status() != WL_CONNECTED); i++) {
      delay(500);
      Serial.print(".");
      if (i == 49) {
        WiFi.mode(WIFI_AP_STA);
        WiFi.softAP(ssid, password);
        Serial.print("Autonomic mode");
      }
    }
  }
  else {
    data.d.fq0 = 10;
    data.d.fq1 = 10;
    for (int i = 0; i < 4; i++) {
      data.d.currentTemps[i] = 0;
      data.d.Kp[i] = 0;
      data.d.Ki[i] = 0;
      data.d.Kd[i] = 0;
    }
    EEPROM.write(initByteADRES, 42);
    writeSetting();
    writeWiFiSetting();
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(ssid, password);

  }

  Serial.println();
  Serial.println("Configuring access point...");
  IPAddress myIP = WiFi.softAPIP();

  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/get_temp", get_temp);
  server.on("/get_fq", get_fq);
  server.on("/get_PID", get_PID);
  server.on("/set_temp", set_temp);
  server.on("/set_fq", set_fq);
  server.on("/set_PID", set_PID);
  server.on("/set_WiFi", set_WiFi);

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");

  myPID1.SetOutputLimits(0, WindowSize);
  myPID2.SetOutputLimits(0, WindowSize);
  myPID3.SetOutputLimits(0, WindowSize);
  myPID4.SetOutputLimits(0, WindowSize);

  myPID1.SetMode(AUTOMATIC);
  myPID2.SetMode(AUTOMATIC);
  myPID3.SetMode(AUTOMATIC);
  myPID4.SetMode(AUTOMATIC);
}

long timer1 = 0;

int wifiStatus = WL_CONNECTED;

void loop() {
  if (wifiStatus != WiFi.status()) {
    wifiStatus = WiFi.status();

    if (wifiStatus == WL_CONNECTED) {
      Serial.println();
      Serial.print("Connecting to ");
      Serial.println(APSSID);
      WiFi.mode(WIFI_STA);
    }
    else {
      WiFi.mode(WIFI_AP_STA);
      WiFi.softAP(ssid, password);
      Serial.println("\nAutonomic mode");
    }
  }

  server.handleClient();

  if (timer1 + 1000 < millis()) {
    timer1 = millis();
  }
  myPID1.Compute();
  myPID2.Compute();
  myPID3.Compute();
  myPID4.Compute();
}
