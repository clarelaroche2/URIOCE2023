#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Ticker.h>
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
#define HALLEFFECTSENSOR 0

int mode = 0;  // 0=stopped, 1=recording
int count = 1;

RTC_PCF8523 rtc; 

// The file where data is recorded
File frec;
char filename[20];
char s[] = "T";
File settingsFile;

// Settings parameters
int fileLength = 20000; 
int timeOn;
int timeOff;

// Initializing functions--------------------------------------------------------------------------------------------------------------------------

void SDcardInitialize() {
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    while (1) {
      Serial.println("Unable to access the SD card");
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
  rtc.start();
}

void readSettings() {
  // open the file
  settingsFile = SD.open("settings.txt");

  // check if the file is found
  if (!settingsFile) {
    Serial.println("Settings file not found!");
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
    timeOn = line.toInt();
    line = settingsFile.readStringUntil('\n');
    timeOff = line.toInt();
  }

  // close the file
 settingsFile.close();

  // print out the values read from the file
  Serial.print("File length (msec): ");
  Serial.println(fileLength);
  if (readSecondLine == 1) {
    Serial.print("Time on (sec): ");
    Serial.println(timeOn);
    Serial.print("Time off (sec): ");
    Serial.println(timeOff);
  }
  if (readSecondLine == 0) {
    Serial.println("No duty cycle - continuous recording");
  }
}

// Recording functions----------------------------------------------------------------------------------------------------------------------------

void startRecording() {
  Serial.print("Start recording file ");
  Serial.println(count);

  DateTime now = rtc.now();
  sprintf(filename, "%04d%02d%02d%s%02d%02d%02d.RAW", now.year(), now.month(), now.day(), s, now.hour(), now.minute(), now.second());
  
  if (SD.exists(filename)) {
    SD.remove(filename);
  }

  frec = SD.open(filename, FILE_WRITE);
  Serial.println("SD file open");
  Serial.println(filename);
  if (frec) {
    queue1.begin();
    mode = 1;
    digitalWrite(WHITELED,HIGH);
  }
}

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

void resetRecording() {
  queue1.end();
  if (mode == 1) {
    while (queue1.available() > 0) {
      frec.write((byte*)queue1.readBuffer(), 256);
      queue1.freeBuffer();
    }
    Serial.println("stopRecording");
        frec.close();
  }

  count++;

  Serial.print("Start recording file ");
  Serial.println(count);
  
  DateTime now = rtc.now();
  sprintf(filename, "%04d%02d%02d%s%02d%02d%02d.RAW", now.year(), now.month(), now.day(), s, now.hour(), now.minute(), now.second());

  if (SD.exists(filename)) {
    SD.remove(filename);
  }

  frec = SD.open(filename, FILE_WRITE);
  Serial.println("SD file open");
  if (frec) {
    queue1.begin();
    mode = 1;
  }
}

// ticker1 period determines length of segments to be determined by user
Ticker ticker1(resetRecording, fileLength, 0, MILLIS);

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

  mode = 1;
  startRecording();
  ticker1.start();
}

void loop() {
  ticker1.update();
  continueRecording();
}
