


void writeSetting() {
  for (int i = 0; i < sizeof(Data); i++)
    EEPROM.write(START_ADR + i, data.b[i]);
  if (EEPROM.commit()) {
    Serial.println("EEPROM successfully committed");
  } else {
    Serial.println("ERROR! EEPROM commit failed");
  }
}

void readSetting() {
  for (int i = 0; i < sizeof(Data); i++)
    data.b[i] = EEPROM.read(START_ADR + i);
}

//=======================

void write_string_EEPROM (int Addr, String Str) {

  Serial.println();
  Serial.print("Adr=");
  Serial.print(Addr);

  byte lng = Str.length();
  if (lng > 15 )  lng = 0;
  EEPROM.write(Addr , lng);
  unsigned char* buf = new unsigned char[15];
  Str.getBytes(buf, lng + 1);
  Addr++;
  for (byte i = 0; i < lng; i++) {
    EEPROM.write(Addr + i, buf[i]);
    //delay(10);
  }
  EEPROM.commit();
}

String read_string_EEPROM (int Addr) {
  Serial.println();
  Serial.print("Adr=");
  Serial.print(Addr);

  byte lng = EEPROM.read(Addr);
  String str;
  Addr++;
  for (byte i = 0; i < lng; i++)
    str += (char)EEPROM.read(i + Addr);
  return str += "\0";
}

void writeWiFiSetting() {
  write_string_EEPROM(START_ADR + sizeof(Data), APSSID );
  write_string_EEPROM(START_ADR + sizeof(Data)  + STRING_MAXLENGTH , APPSK);
}

void readWiFiSetting() {

  APSSID = read_string_EEPROM(START_ADR + sizeof(Data));
  APPSK =  read_string_EEPROM(START_ADR + sizeof(Data) + STRING_MAXLENGTH);

}
