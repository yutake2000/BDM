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

typedef unsigned long ul;
typedef unsigned int ui;

const int pin_buttons[] = {2, 4, 5, 6};
const int pin_wires[] = {15, 16, 17, 18, 19};
const int pin_LEDs[] = {7, 14}; // Buttons, Wires
const int pin_line[] = {8, 9};
const int pin_7seg[] = {10, 13, 12, 11};
const int pin_speaker = 3;

const int timelimit = 599;
const int dt = 1;

ui cnt = 0;
bool cleared[] = {false, false};
bool flagGameover = false;

void setup() {
  for (int i = 0; i < 4; i++) pinMode(pin_buttons[i], INPUT_PULLUP);
  for (int i = 0; i < 5; i++) pinMode(pin_wires[i], INPUT_PULLUP);
  for (int i = 0; i < 2; i++) pinMode(pin_LEDs[i], OUTPUT);
  for (int i = 0; i < 2; i++) pinMode(pin_line[i], OUTPUT);
  for (int i = 0; i < 4; i++) pinMode(pin_7seg[i], OUTPUT);
  pinMode(pin_speaker, OUTPUT);
  Serial.begin(9600);

  Serial.println("b0\tb1\tb2\tb3\tw0\tw1\tw2\tw3\tw4");
}

void ug(int f, float len, float rit = 0) {
  float static a = 45;
  if (f == 0) noTone(3);
  else tone(pin_speaker, f); 
  a -= rit;
  delay(60.0 / a / len * 1000);
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

void write7seg(int n) {
  for (int i = 0; i < 4; i++) digitalWrite(pin_7seg[i], (n >> i) & 1);
}

void gameover() {

  flagGameover = true;
  for (int i=0; i<2; i++) cleared[i] = false;
  
  playSoundGameover();
  
}

void loop() {

  int timeLeft = 0, timeLeftMinutes = 0, timeLeftSeconds = 0;

  if (flagGameover) {
    
  } else {
    
    if (millis() / 1000 > timelimit) {
      timeLeft = timelimit - millis() / 1000;
      timeLeftMinutes = timeLeft / 60;
      timeLeftSeconds = timeLeft % 60;
    }
  
    if (cnt * dt % 1000 == 0) { // every 1000 ms
      int value;
      Serial.println(String(timeLeftMinutes) + ":" + String(timeLeftSeconds));
      for (int i = 0; i < 4; i++) {
        value = digitalRead(pin_buttons[i]);
        if (!value) { // for test
          gameover();
          return;
        }
        Serial.print(value + "\t");
      }
  
      for (int i = 0; i < 5; i++) {
        value = digitalRead(pin_wires[i]);
        Serial.print(value + "\t");
      }
    }

  }
  
  for (int i = 0; i < 2; i++) digitalWrite(pin_LEDs[i], cleared[i]);

  for (int i = 0; i < 2; i++) digitalWrite(pin_line[i], (cnt >> i) & 1);

  if (cnt % 4 == 0) write7seg(timeLeftMinutes);
  else if (cnt % 4 == 1) write7seg(2); // display ":"
  else if (cnt % 4 == 2) write7seg(timeLeftSeconds / 10);
  else if (cnt % 4 == 3) write7seg(timeLeftSeconds % 10);
  else write7seg(0);

  cnt++;

  delay(dt);
  
}
