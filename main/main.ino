#include <Cth.h>

#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587

#define NOTE_C8  4186

typedef unsigned long ul;
typedef unsigned int ui;

const int pin_mode = A0;
const int pin_sr_data[] = {A3, A2};
const int pin_sr_clk = 7;
const int pin_sr_load[] = {5, 6};
const int pin_LEDs[] = {2, 4}; 
const int pin_line[] = {8, 9};
const int pin_7seg[] = {13, 10, 11, 12};
const int pin_speaker = 3;

const ul timelimit = 600;
const int dt = 1;

const bool mute = true;

ui cnt = 0;
bool cleared[] = {false, false};
bool flagGameover = false;

void setup() {
  pinMode(pin_sr_clk, OUTPUT);
  for (int i=0; i<2; i++) pinMode(pin_sr_data[i], INPUT);
  for (int i=0; i<2; i++) pinMode(pin_sr_load[i], OUTPUT);
  
  for (int i = 0; i < 2; i++) pinMode(pin_LEDs[i], OUTPUT);
  for (int i = 0; i < 2; i++) pinMode(pin_line[i], OUTPUT);
  for (int i = 0; i < 4; i++) pinMode(pin_7seg[i], OUTPUT);
  pinMode(pin_speaker, OUTPUT);
  Serial.begin(9600);
}

void ug(int f, float len, float rit = 0) {
  float static a = 45;
  if (f == 0) noTone(3);
  else if (!mute) tone(pin_speaker, f); 
  a -= rit;
  Scheduler.delay(60.0 / a / len * 1000);
}

void playSoundGameover() {
  ug(NOTE_C5, 4);
  ug(0, 8);
  ug(NOTE_G4, 8);
  ug(0, 4);
  ug(NOTE_E4, 4);

  ug(NOTE_A4, 6, 2);
  ug(NOTE_B4, 6, 2);
  ug(NOTE_A4, 6, 2);
  ug(NOTE_GS4, 6, 2);
  ug(NOTE_AS4, 6, 2);
  ug(NOTE_GS4, 6, 2);

  ug(NOTE_E4, 16, 2);
  ug(NOTE_D4, 16, 2);
  ug(NOTE_E4, 8);
  ug(NOTE_E4, 2);
  ug(NOTE_E4, 4);
  
  ug(0, 4);
}

void playSoundTick() {
  tone(pin_speaker, NOTE_C8);
  Scheduler.delay(100);
  noTone(pin_speaker);
}

void write7seg(int n) {
  for (int i = 0; i < 4; i++) digitalWrite(pin_7seg[i], (n >> i) & 1);
}

byte myShiftIn(int dataPin, int clockPin, int loadPin){

  byte data;

  digitalWrite(loadPin, LOW); //A-Hを格納
  digitalWrite(loadPin, HIGH); //確定
  
  data = digitalRead(dataPin) << 7; //Hの値を読む
  
  for (int i=6; i>=0; i--){
    digitalWrite(clockPin, HIGH);
    data |= digitalRead(dataPin) << i; //G,F,E...Aの値を読む
    digitalWrite(clockPin, LOW);
  }

  return data;
}

void gameover() {

  flagGameover = true;
  for (int i=0; i<2; i++) cleared[i] = false;
  
  Scheduler.start(playSoundGameover);
  
}

void blinkLED(int pin) {
  digitalWrite(pin, 1);
  Scheduler.delay(100);
  digitalWrite(pin, 0);
}

int lastTimeLeft = 0;
int timeLeft = 0;

byte lastData[] = {0, 0};

void readModuleData() {
  for (int i=0; i<2; i++) {
    byte data = myShiftIn(pin_sr_data[i], pin_sr_clk, pin_sr_load[i]);
    byte id = data >> 6;
    byte flags = data & 0b111111;

    /*
    Serial.print("Module" + String(i) + ": ");
    for (int j=0; j<8; j++) {
      Serial.print(String((data >> (7-j)) & 1));
    }
    Serial.println();
    */

    switch(id) {
      case 0: // wires
        if (flags == 0b111011) {
          cleared[i] = true;
        }
      break;
      case 1: // buttons
        if (flags == 0b110100) {
          cleared[i] = true;
        }
        if (lastData[i] & (lastData[i] ^ data)) { // negedge data[x]
          Scheduler.start(blinkLED, pin_LEDs[i]);
        }
      break;
    }

    lastData[i] = data;
  }
}

void loop() {

  int timeLeftMinutes = 0, timeLeftSeconds = 0;

  if (flagGameover) {
    
  } else {
    
    if (millis() < timelimit * 1000) {
      lastTimeLeft = timeLeft;
      timeLeft = (timelimit * 1000 - millis()) / 1000;
      timeLeftMinutes = timeLeft / 60;
      timeLeftSeconds = timeLeft % 60;
    } else {
      gameover();
    }

    if (cnt % 100 == 0) {
      readModuleData();
    }

    if (timeLeft != lastTimeLeft) { // every 1000 ms
      int value;
      byte data;
      Serial.println(String(timeLeftMinutes) + ":" + String(timeLeftSeconds));
      
      if (!mute) Scheduler.start(playSoundTick);
      //cnt += 100;
    }

  }
  
  for (int i = 0; i < 2; i++) {
    if (cleared[i]) digitalWrite(pin_LEDs[i], cleared[i]);
  }

  for (int i = 0; i < 2; i++) digitalWrite(pin_line[i], (cnt >> i) & 1);

  if (cnt % 4 == 0) write7seg(timeLeftMinutes);
  else if (cnt % 4 == 1) write7seg(2); // display ":"
  else if (cnt % 4 == 2) write7seg(timeLeftSeconds / 10);
  else if (cnt % 4 == 3) write7seg(timeLeftSeconds % 10);
  else write7seg(0);

  cnt++;

  delay(dt);
  
}
