// this library is public domain. enjoy!
// www.ladyada.net/learn/sensors/thermocouple

#include <avr/pgmspace.h>
//#include <util/delay.h>
#include <stdlib.h>
#include "max6675.h"

#define TERMOPIN_1 0
#define TERMOPIN_2 0
#define TERMOPIN_3 0

MAX6675::MAX6675(int8_t SCLK, int8_t CS, int8_t MISO) {
  sclk = SCLK;
  cs = CS;
  miso = MISO;

  pinMode(TERMOPIN_1, OUTPUT);
  pinMode(TERMOPIN_2, OUTPUT);
  pinMode(TERMOPIN_2, OUTPUT);
  pinMode(sclk, OUTPUT); 
  pinMode(miso, INPUT);

  digitalWrite(TERMOPIN_1, LOW);
  digitalWrite(TERMOPIN_2, LOW);
  digitalWrite(TERMOPIN_3, LOW);
}

double MAX6675::readCelsius(void) {
  uint16_t v;

  digitalWrite(TERMOPIN_1, LOW);
  digitalWrite(TERMOPIN_2, LOW);
  digitalWrite(TERMOPIN_3, LOW);
  
  delay(10);

  v = spiread();
  v <<= 8;
  v |= spiread();

  digitalWrite(TERMOPIN_1, cs&0b1==0?0:1);
  digitalWrite(TERMOPIN_2, cs&0b10==0?0:1);
  digitalWrite(TERMOPIN_3, cs&0b100==0?0:1);

  if (v & 0x4) {
    // uh oh, no thermocouple attached!
    return NAN; 
    //return -100;
  }
  v >>= 3;
  return v*0.25;
}

double MAX6675::readFahrenheit(void) {
  return readCelsius() * 9.0/5.0 + 32;
}

byte MAX6675::spiread(void) { 
  int i;
  byte d = 0;

  for (i=7; i>=0; i--)
  {
    digitalWrite(sclk, LOW);
    delay(1);
    if (digitalRead(miso)) {
      //set the bit to 0 no matter what
      d |= (1 << i);
    }

    digitalWrite(sclk, HIGH);
    delay(1);
  }

  return d;
}
