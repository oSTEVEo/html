/* Create a WiFi access point and provide a web server on it. */
#include <PID_v1.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

//#include "site.h"
const String data256 = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Document</title><style>body{background-color:#969696fd;padding:0px;margin:0px;width:100%;height:100%;padding:0px;margin:0px}#teemp{background-color:salmon;margin:10px}.monitor,.pid_monitor{margin:10px;font-size:2.33333rem;color:gray;text-shadow:2px 4px 3px rgba(0,0,0,0.2);font-family:'Oswald',sans-serif}.sets{font-size:2.33333rem;margin-left:20px}#temps,#fqs,#pids{align-self:top-center;padding:15px;padding-top:1px;width:auto;margin:20px;background-color:white;border-radius:20px}</style></head><body><div id=\"temps\"><div id=\"temp_monitor\"><div class=\"monitor\" id=\"temp0\">Температура 1: -0 C&#176;</div><div class=\"monitor\" id=\"temp1\">Температура 2: -0 C&#176;</div><div class=\"monitor\" id=\"temp2\">Температура 3: -0 C&#176;</div><div class=\"monitor\" id=\"temp3\">Температура 4: -0 C&#176;</div></div><div class=\"sets\" name=\"set_temp\"> <select id=\"temps_form\"><option>1 Термодатчик</option><option>2 Термодатчик</option><option>3 Термодатчик</option><option>4 Термодатчик</option> </select> <input type=\"text\" name=\"set temperature\" id=\"set_temp\" placeholder=\"Введите температуру\"> <input type=\"button\" value=\"Установить\"></div></div><div id=\"fqs\"><div id=\"fq_monitor\"><div class=\"monitor\" id=\"fq0\">Мотор 1: -0 HZ</div><div class=\"monitor\" id=\"fq1\">Мотор 2: -0 HZ</div></div><div class=\"sets\" name=\"set_fq\"> <select id=\"fq_form\"><option>1 Мотор</option><option>2 Мотор</option> </select> <input type=\"text\" name=\"set fq\" id=\"set_fq\" placeholder=\"Введите частоту\"> <input type=\"button\" value=\"Установить\"></div></div><div id=\"pids\"><table id=\"pid_monitor\" cellpadding=\"5px\"><tr><td></td><td>kp</td><td>ki</td><td>kd</td></tr><tr><td>Нагреватель 1</td><td id=\"pid0_kp\">-1</td><td id=\"pid0_ki\">-1</td><td id=\"pid0_kd\">-1</td></tr><tr><td>Нагреватель 2</td><td id=\"pid1_kp\">-1</td><td id=\"pid1_ki\">-1</td><td id=\"pid1_kd\">-1</td></tr><tr><td>Нагреватель 3</td><td id=\"pid2_kp\">-1</td><td id=\"pid2_ki\">-1</td><td id=\"pid2_kd\">-1</td></tr><tr><td>Нагреватель 4</td><td id=\"pid3_kp\">-1</td><td id=\"pid3_ki\">-1</td><td id=\"pid3_kd\">-1</td></tr></table><div class=\"sets\" name=\"set_pid\"> <select id=\"temps_for_pid\"><option>1 Нагреватель</option><option>2 Нагреватель</option><option>3 Нагреватель</option><option>4 Нагреватель</option> </select><select id=\"pid_form\"><option>kp</option><option>ki</option><option>kd</option> </select> <input type=\"text\" name=\"set pid\" id=\"set_pid\" placeholder=\"Введите значение\"> <input type=\"button\" value=\"Установить\"></div></div></div> <script>var DEBUG=false;var key=0;function httpGet(theUrl){var xmlHttp=new XMLHttpRequest();xmlHttp.open(\"GET\",theUrl,false);xmlHttp.send(null);if(xmlHttp.responseText==\"File Not Found\"){console.log(\"404 on server, error 404\");return\"{}\"}return xmlHttp.responseText}function get_temp(){var value;if(DEBUG){value='{\"t0\":\"0.00\",\"t1\":\"0.00\",\"t2\" :\"0.00\",\"t3\":\"0.00\"}'}else{value=httpGet(\"/get_temp\")}var tmp=JSON.parse(value);document.getElementById(\"temp0\").innerText=`Температура 1>${tmp.t0}C°`;document.getElementById(\"temp1\").innerText=`Температура 2>${tmp.t1}C°`;document.getElementById(\"temp2\").innerText=`Температура 3>${tmp.t2}C°`;document.getElementById(\"temp3\").innerText=`Температура 4>${tmp.t3}C°`}function get_fq(){if(DEBUG){value='{\"f0\":\"0\",\"f1\":\"0\"}'}else{value=httpGet(\"/get_fq\")}var tmp=JSON.parse(value);document.getElementById(\"fq0\").innerText=`Мотор 1>${tmp.fq0}Hz`;document.getElementById(\"fq1\").innerText=`Мотор 2>${tmp.fq1}Hz`;}function get_PID(){if(DEBUG){value='{\"PID0\":{\"Kp\":\"2.00\",\"Ki\":\"5.00\",\"Kd\":\"1.00\"},\"PID1\":{\"Kp\":\"2.00\",\"Ki\":\"3.00\",\"Kd\":\"4.00\"},\"PID2\":{\"Kp\":\"2.00\",\"Ki\":\"5.00\",\"Kd\":\"1.00\"},\"PID3\":{\"Kp\":\"2.00\",\"Ki\":\"5.00\",\"Kd\":\"1.00\"}}'}else{value=httpGet(\"/get_PID\")}var tmp=JSON.parse(value);var a=Object.values(tmp);for(var i=0;i<4;i+=1){document.getElementById(`pid${i}_kp`).innerText=a[i].Kp;document.getElementById(`pid${i}_ki`).innerText=a[i].Ki;document.getElementById(`pid${i}_kd`).innerText=a[i].Kd}}function set_temp(chen_num,value){console.log(\"set temp\");var request=\"/set_temp?chenl=\";request+=chen_num;request+=\"&temp=\";request+=value;request+=\"&key=\";request+=key;var result;var result=httpGet(request);console.log(\"Request is: \"+request+\"nResult is: \"+result)}function set_fq(chen_num,frequency){console.log(\"set fq\");var request=\"/set_fq?chenl=\";request+=chen_num;request+=\"&frequency=\";request+=frequency;request+=\"&key=\";request+=key;var result;var result=httpGet(request);console.log(\"Request is: \"+request+\"nResult is: \"+result)}function set_PID(chenl,kp_,ki_,kd_){console.log(\"set fq\");var request=\"/set_fq?chenl=\";request+=chenl;request+=\"&kp=\";request+=kp_;request+=\"&ki=\";request+=ki_;request+=\"&kd=\";request+=kd_;request+=\"&key=\";request+=key;var result;var result=httpGet(request);console.log(\"Request is: \"+request+\"nResult is: \"+result)}setInterval(get_temp,500);setInterval(get_fq,500);setInterval(get_PID,500);</script> </body></html>";

#ifndef APSSID
#define APSSID "ESPap1"
#define APPSK  "qweqweqweasd"
#endif

#include <PID_v1.h>

#define RELAY_PIN 12

#include "max6675.h"


/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

int fq0 = 0;
int fq1 = 0;

int temps[4] = {20, 20, 20, 20};

ESP8266WebServer server(80);


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
  server.send(200, "text/html", data256);
  //for(int i = 0; i < sizeof(data256)/sizeof(char); i++){
  // server.write((char)pgm_read_byte(&data256[i])); 
  //}
  //server.end();
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
    if (chenl == 0) {
      fq0 = fq;
      Serial.println("FQ0=" + String(fq0));
    }
    else if (chenl == 1) {
      fq1 = fq;
      Serial.println("FQ1=" + String(fq1));
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
    msg += "\"t" + String(i) + "\":\"" + String(temps[i]) + "\"";
    if (i < 3) {
      msg += ",";
    }
  }
  msg += "}";
  server.send(200, "text/json", msg);
}

void get_fq() {
  String msg = "{\"fq0\":\""+String(fq0)+"\",\"fq1\":\""+String(fq1)+"\"}";
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
  msg += "}";
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