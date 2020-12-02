/* Create a WiFi access point and provide a web server on it. */
#include <PID_v1.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#ifndef APSSID
#define APSSID "ESPap"
#define APPSK  "qweqweqweasd"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

int fq1 = 0;
int fq2 = 0;

int temps[4] = {20, 20, 20, 20};

struct Chenl {
  double Ki, Kp, Kd,
         Setpoint, Input, Output;
  int csInp, outPin;
};

Chenl chenls[4];

ESP8266WebServer server(80);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/

//PID
#include <PID_v1.h>

#define RELAY_PIN 12

#include "max6675.h"

int thermoDO = 14;
int thermoCS = 5;
int thermoCLK = 16;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

double Setpoint, Input, Output;

double Kp[4] = {2, 2, 2, 2}, Ki[4] = {5, 5, 5, 5}, Kd[4] = {1, 1, 1, 1};

PID myPID1(&Input, &Output, &Setpoint, Kp[0], Ki[0], Kd[0], DIRECT);
PID myPID2(&Input, &Output, &Setpoint, Kp[1], Ki[1], Kd[1], DIRECT);
PID myPID3(&Input, &Output, &Setpoint, Kp[2], Ki[2], Kd[2], DIRECT);
PID myPID4(&Input, &Output, &Setpoint, Kp[3], Ki[3], Kd[3], DIRECT);

int WindowSize = 5000;
unsigned long windowStartTime;
//ENDPID

void handleRoot() {
  server.send(200, "text/html", "<h1>You are connected</h1>");
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

bool isKeyValid(String key) {
  return true;
}

void set_temp() {
  float temp;
  int chenl;
  String key;

  int cs = 0;
  for (uint8_t i = 0; i < server.args(); i++) {

    if (server.argName(i) == "chenl") {
      chenl = server.arg(i).toInt();
      cs += 1;
    }
    else if (server.argName(i) == "temp") {
      temp = server.arg(i).toInt(); //TEMP!!!
      cs += 2;
    }
    else if (server.argName(i) == "key") {
      key = server.arg(i);
      cs += 4;
    }
  }

  if ((cs == 7) && isKeyValid(key)) {
    server.send(200, "text/plain", "ok");
    if ((chenl <= 4) && (chenl > 0)) {
      temps[chenl - 1] = temp;
      Serial.println("TEMP" + String(chenl) + " = " + String(temp));
    }
  }
  else
    server.send(400, "text/plain", "Bad Request");
}

void set_fq() {
  int fq;
  int chenl;
  String key;

  int cs = 0;
  for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "chenl") {
      chenl = server.arg(i).toInt();
      cs += 1;
    }
    else if (server.argName(i) == "frequency") {
      fq = server.arg(i).toInt();
      cs += 2;
    }
    else if (server.argName(i) == "key") {
      key = server.arg(i);
      cs += 4;
    }
  }

  if ((cs == 7) && isKeyValid(key)) {
    server.send(200, "text/plain", "ok");
    if (chenl == 1) {
      fq1 = fq;
      Serial.println("FQ1=" + String(fq1));
    }
    else if (chenl == 2) {
      fq2 = fq;
      Serial.println("FQ2=" + String(fq2));
    }
  }
  else
    server.send(400, "text/plain", "Bad Request");
}

void set_PID() {
  double kp_;
  double ki_;
  double kd_;
  int chenl;
  String key;

  int cs = 0;
  for (uint8_t i = 0; i < server.args(); i++) {

    if (server.argName(i) == "chenl") {
      chenl = server.arg(i).toInt();
      cs += 1;
    }
    else if (server.argName(i) == "kp") {
      kp_ = server.arg(i).toInt();
      cs += 2;
    }
    else if (server.argName(i) == "ki") {
      ki_ = server.arg(i).toInt();
      cs += 4;
    }
    else if (server.argName(i) == "kd") {
      kd_ = server.arg(i).toInt();
      cs += 8;
    }
    else if (server.argName(i) == "key") {
      key = server.arg(i);
      cs += 16;
    }
  }

  if ((cs == 31) && isKeyValid(key)) {
    server.send(200, "text/plain", "ok");
      Kp[chenl] = kp_;
      Ki[chenl] = ki_;
      Kd[chenl] = kd_;
      Serial.println("PID touch");
  }
  else
    server.send(400, "text/plain", "Bad Request");
}

void get_temp() {
  String msg = "{";
  for (int i = 0; i < 4; i++) {
    msg += "\"t" + String(i) + "\":\"" + String(chenls[i].Input) + "\"";
    if (i < 3) {
      msg += ",";
    }
  }
  msg += "};";
  server.send(200, "text/json", msg);
}

void get_fq() {
  String msg = "{";
  for (int i = 0; i < 2; i++) {
    msg += "\"machine" + String(i) + "\"" + ": {";
    msg += "\"fq1\" : \"" + String(fq1) + "\",\"" + "fq2\"" + ":" + "\"" + String(fq2) + "\"";
    if (i < 3) 
      msg += "}, ";
    else
      msg += "} ";
  }
  msg += "}";
  server.send(200, "text/json", msg);
}

void get_PID(){
  String msg = "{";
  for (int i = 0; i < 4; i++){
    msg += "\"PID" + String(i) + "\":{";
    msg += "\"Kp\":\"" + String(Kp[i]) + "\"," + "\"Ki\":\"" + String(Ki[i]) + "\",\"Kd\":\"" + String(Kd[i]);
    if(i < 3)
      msg += "\"},";  
    else
      msg += "\"}";
  }
  msg += "};";
  server.send(200, "text/json", msg);
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
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
  //PID
  windowStartTime = millis();

  Setpoint = 40;

  myPID1.SetOutputLimits(0, WindowSize);
  myPID2.SetOutputLimits(0, WindowSize);
  myPID3.SetOutputLimits(0, WindowSize);
  myPID4.SetOutputLimits(0, WindowSize);

  myPID1.SetMode(AUTOMATIC);
  myPID2.SetMode(AUTOMATIC);
  myPID3.SetMode(AUTOMATIC);
  myPID4.SetMode(AUTOMATIC);
  pinMode(RELAY_PIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("PID started, delay...");
  delay(500);
  Serial.print("...");
}

long timer1 = 0;

void loop() {
  server.handleClient();

  if (timer1 + 1000 < millis()) {
    timer1 = millis();

    Input = thermocouple.readCelsius();
    Serial.println(Input);

  }
  myPID1.Compute();
  myPID2.Compute();
  myPID3.Compute();
  myPID4.Compute();
  if (millis() - windowStartTime > WindowSize)windowStartTime += WindowSize;
  if (Output < millis() - windowStartTime) digitalWrite(RELAY_PIN, LOW);
  else digitalWrite(RELAY_PIN, HIGH);
}