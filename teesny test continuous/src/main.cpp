#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Ticker.h>
#include <TimeLib.h>

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

// Remember which mode we're doing
int mode = 0;  // 0=stopped, 1=recording, 2=playing
int count = 1;

// The file where data is recorded
File frec;
char filename[20];
char *s = "sample_";

void startRecording() {
  Serial.print("Start recording file ");
  Serial.println(count);
  snprintf(filename, sizeof(filename), "%s%03d.RAW", s, count);

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

void continueRecording() {

  if (queue1.available() >= 2) {
    byte buffer[512];
    memcpy(buffer, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    memcpy(buffer+256, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    // elapsedMicros usec = 0;
    frec.write(buffer, 512);
    // Uncomment to see how long SD writes are taking, must be < 5802 us 
    // Serial.print("SD write, us=");
    // Serial.println(usec);
    digitalToggle(WHITELED);
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
  snprintf(filename, sizeof(filename), "%s%03d.RAW", s, count);

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
Ticker ticker1(resetRecording, 60000, 0, MILLIS);

void setup() {

  // Audio setup/enables
  AudioMemory(128);
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(1);
  // LED pin setup
  pinMode(REDLED, OUTPUT);
  pinMode(ORANGELED, OUTPUT);
  pinMode(YELLOWLED, OUTPUT);
  pinMode(GREENLED, OUTPUT);
  pinMode(BLUELED, OUTPUT);
  pinMode(WHITELED, OUTPUT);

  // Initialize the SD card
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // SD card error idicator
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
  // readSettings();
  mode = 1;
  startRecording();
  ticker1.start();
}


void loop() {
  ticker1.update();
  continueRecording();
}
