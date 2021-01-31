#include <EEPROM.h>

#include <PID_v1.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <PID_v1.h>

#define RELAY_PIN 12

#include "max6675.h"

#define KiADRES 0
#define KpADRES sizeof(double)*4+KiADRES
#define KdADRES sizeof(double)*4+KpADRES

#define TempADRES sizeof(double)*4+KdADRES

#define Fq0ADRES sizeof(double)*4+TempADRES
#define Fq1ADRES sizeof(int)+Fq0ADRES

#define initByteADRES sizeof(int)+Fq1ADRES

String APSSID = "ESPap1";
String APPSK = "qweqweqweasd";

const char *ssid = "ESPap1";
const char *password = "qweqweqweasd";

double currentTemps[4] = {20, 20, 20, 20};

ESP8266WebServer server(80);

int fq0 = 0;
int fq1 = 0;
double Output[4];
double temps[4] = {20, 20, 20, 20};
double Kp[4] = {1, 1, 1, 1};
double Ki[4] = {1, 1, 1, 1};
double Kd[4] = {1, 1, 1, 1};

#include "epr.h"
#include "site.h"


PID myPID1(&currentTemps[0], &Output[0], &temps[0], Kp[0], Ki[0], Kd[0], DIRECT);
PID myPID2(&currentTemps[1], &Output[1], &temps[1], Kp[1], Ki[1], Kd[1], DIRECT);
PID myPID3(&currentTemps[2], &Output[2], &temps[2], Kp[2], Ki[2], Kd[2], DIRECT);
PID myPID4(&currentTemps[3], &Output[3], &temps[3], Kp[3], Ki[3], Kd[3], DIRECT); //switchkeys

int WindowSize = 5000;


void setup() {
  EEPROM.begin(512);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");


  if (EEPROM.read(initByteADRES) == 43) { //this magic num 42
    readEEprom();
  } else {
    EEPROM.write(initByteADRES, 43);
    if (EEPROM.commit()) {
      Serial.println("EEPROM successfully committed");
    } else {
      Serial.println("ERROR! EEPROM commit failed");
    }
    writeEEprom();
    Serial.println("EEPROM INIT");
  }

  WiFi.softAP(ssid, password);

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

void loop() {
  server.handleClient();

  if (timer1 + 1000 < millis()) {
    timer1 = millis();

    //for (int i = 0; i < 4; i++) {
    //currentTemp[i] = thermocouple.readCelsius();
    //Serial.println(Input);
    //}
  }
  myPID1.Compute();
  myPID2.Compute();
  myPID3.Compute();
  myPID4.Compute();
}
