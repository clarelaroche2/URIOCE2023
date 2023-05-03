#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <TimeLib.h>
#include "RTClib.h"

// GUItool: begin automatically generated code
AudioInputI2S            i2s2;           //xy=105,63
AudioRecordQueue         queue1;         //xy=281,63
AudioConnection          patchCord1(i2s2, 1, queue1, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=265,212
// GUItool: end automatically generated code

const int myInput = AUDIO_INPUT_LINEIN;

// Use these with the Teensy Audio Shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  11
#define SDCARD_MISO_PIN  12
#define SDCARD_SCK_PIN   13
#define REDLED 1
#define ORANGELED 2
#define YELLOWLED 3
#define GREENLED 4
#define BLUELED 5
#define WHITELED 9

int mode = 0;  // 0=stopped, 1=recording
int count = 1;

RTC_PCF8523 rtc; 

// The file where data is recorded
File frec;
char filename[40];
char s[] = "T";
File settingsFile;

// Settings parameters
int fileLength;
int timeOn;
int timeOff;
int numFiles;
int startTime, elapsedTime;

// Initializing functions--------------------------------------------------------------------------------------------------------------------------

void SDcardInitialize() {
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    while (1) {
      digitalWrite(REDLED, HIGH);
      delay(1000);
      digitalWrite(REDLED, LOW);
      delay(1000);
    }
  }
  else {
    digitalWrite(GREENLED, HIGH);
    delay(1000);
    digitalWrite(GREENLED, LOW);
    delay(1000);
  }
}

void rtcInitialize() {
  #ifndef ESP8266
  #endif

  rtc.begin(&Wire1);

  if (! rtc.begin(&Wire1)) {
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
  rtc.start();
}

void readSettings() {
  // open the file
  settingsFile = SD.open("settings.txt");

  // check if the file is found
  if (!settingsFile) {
    digitalWrite(YELLOWLED, HIGH);
    delay(1000);
    digitalWrite(YELLOWLED, LOW);
    delay(1000);
  }
  else {
    digitalWrite(GREENLED, HIGH);
    delay(1000);
    digitalWrite(GREENLED, LOW);
    delay(1000);
  }
  // read the first line as fileLength in sec and convert to millis
  String line = settingsFile.readStringUntil('\n');
  fileLength = (line.toInt())*1000;

  // read the second line and assign the third and fourth lines based on its value
  line = settingsFile.readStringUntil('\n');
  int readSecondLine = line.toInt();
  
  if (readSecondLine == 1) {
    line = settingsFile.readStringUntil('\n');
    timeOn = (line.toInt())*1000;
    line = settingsFile.readStringUntil('\n');
    timeOff = (line.toInt())*1000;
    numFiles = timeOn/fileLength;
  }
  else {
    numFiles = INFINITY;
    timeOff = 0;
  }
  
  // close the file
 settingsFile.close();
}

// Recording functions----------------------------------------------------------------------------------------------------------------------------

void continueRecording() {
  if (queue1.available() >= 2) {
    byte buffer[512];
    memcpy(buffer, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    memcpy(buffer+256, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    frec.write(buffer, 512);
  }
}

void startRecording() {
  DateTime now = rtc.now();
  sprintf(filename, "%04d%02d%02d%s%02d%02d%02d.RAW", now.year(), now.month(), now.day(), s, now.hour(), now.minute(), now.second());
  
  if (SD.exists(filename)) {
    SD.remove(filename);
  }

  frec = SD.open(filename, FILE_WRITE);
  if (frec) {
    startTime = millis();
    elapsedTime = 0;
    queue1.begin();
    mode = 1;
    digitalWrite(WHITELED,HIGH);
    while (elapsedTime < fileLength) {
      continueRecording();
      elapsedTime = millis() - startTime;
    }
    frec.close();
  }
}

void stopRecording() {
  queue1.end();
  if (mode == 1) {
    while (queue1.available() > 0) {
      frec.write((byte*)queue1.readBuffer(), 256);
      queue1.freeBuffer();
    }
    frec.close();
    digitalWrite(WHITELED,LOW);
  }

  count++;
}

void setup() {
  // Audio setup/enables
  AudioMemory(128);
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(0.5);
  // LED pin setup
  pinMode(REDLED, OUTPUT);
  pinMode(ORANGELED, OUTPUT);
  pinMode(YELLOWLED, OUTPUT);
  pinMode(GREENLED, OUTPUT);
  pinMode(BLUELED, OUTPUT);
  pinMode(WHITELED, OUTPUT);

  rtcInitialize();
  SDcardInitialize();
  readSettings();
}

void loop() {
  for (int i = 0; i < numFiles; i++) {
    startRecording();
    stopRecording();
  }
  digitalWrite(BLUELED,HIGH);
  delay(timeOff);
  digitalWrite(BLUELED,LOW);
}
