#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
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
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988

const int pin_speaker = 3;

void ug(int f, float len, float rit = 0) {
  float static a = 45;
  if (f == 0) noTone(3);
  else tone(pin_speaker, f); 
  a -= rit;
  delay(60.0 / a / len * 1000);
}

void setup() {
  pinMode(pin_speaker, OUTPUT);
  Serial.begin(9600);
  
  TCCR2A = _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS20);

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

void loop() {

}
