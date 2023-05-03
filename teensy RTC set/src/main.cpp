#include "Arduino.h"
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>
#include <Wire.h>
#include "RTClib.h"

// Use this code while teensy is connected to a computer to set the rtc if it has not yet been set

RTC_PCF8523 rtc; 

#define ORANGELED 2
#define GREENLED 4

void rtcInitialize() {
  rtc.begin(&Wire1); // Wire1: RTC SDA 17, SCL 16

  if (! rtc.begin(&Wire1)) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    digitalWrite(ORANGELED, HIGH);
    delay(1000);
    digitalWrite(ORANGELED, LOW);
    delay(1000);
    while (1) delay(10);
  }

  else {
    digitalWrite(GREENLED, HIGH);
    delay(1000);
    digitalWrite(GREENLED, LOW);
    delay(1000);
  }

Serial.println("RTC lost power, let's set the time!");
rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

rtc.start(); 
}

void setup() {
  Serial.begin(57600);

#ifndef ESP8266
  while (!Serial);
#endif

  pinMode(ORANGELED, OUTPUT);
  pinMode(GREENLED, OUTPUT);

  rtcInitialize();
}


void loop() {
  DateTime now = rtc.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  delay(1000);
}
