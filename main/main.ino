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

#define DEBUG true

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

const ul timelimit = 600 - 1; //制限時間(s)
const int dt = 1;

const bool mute = false; //スピーカーを鳴らすかどうかのフラグ


bool cleared[] = {false, false};
bool flagGameover = false;

int timer_basis = 1000; //タイマーのカウントを1減らすまでの時間(ms)

bool miss_flag = false; //ミス時の特殊処理への移行フラグ

void setup() {
  pinMode(pin_sr_clk, OUTPUT);
  for (int i = 0; i < 2; i++) pinMode(pin_sr_data[i], INPUT);
  for (int i = 0; i < 2; i++) pinMode(pin_sr_load[i], OUTPUT);

  for (int i = 0; i < 2; i++) pinMode(pin_LEDs[i], OUTPUT);
  for (int i = 0; i < 2; i++) pinMode(pin_line[i], OUTPUT);
  for (int i = 0; i < 4; i++) pinMode(pin_7seg[i], OUTPUT);
  pinMode(pin_speaker, OUTPUT);
  Serial.begin(9600);

  Scheduler.startLoop(printTimer);//タイマーの表示処理の設定
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
  if(!miss_flag)noTone(pin_speaker);//ミスしたときのビープ音を止めないため
}

//
void playBeep(int soundms) {
  tone(pin_speaker, NOTE_C4);
  Scheduler.delay(soundms);
  noTone(pin_speaker);
}

void write7seg(int n) {
  for (int i = 0; i < 4; i++) digitalWrite(pin_7seg[i], (n >> i) & 1);
}

//残り時間の記録
int lastTimeLeft = timelimit;
int timeLeft = timelimit;

//TImerの残り時間表示
ui cnt_printTimer;
void printTimer() {
  int timeLeftMinutes = timeLeft / 60;
  int timeLeftSeconds = timeLeft % 60;
  //点灯させる７セグLEDの指定
  for (int i = 0; i < 2; i++) digitalWrite(pin_line[i], (cnt_printTimer >> i) & 1);
  //点灯させる内容の指定
  if (cnt_printTimer % 4 == 0) write7seg(timeLeftMinutes % 10);
  else if (cnt_printTimer % 4 == 1) write7seg(2); // display ":"
  else if (cnt_printTimer % 4 == 2) write7seg(timeLeftSeconds / 10);
  else if (cnt_printTimer % 4 == 3) write7seg(timeLeftSeconds % 10);
  else write7seg(0);
  cnt_printTimer = (cnt_printTimer + 1) % 4;
  Scheduler.delay(1);
}

byte myShiftIn(int dataPin, int clockPin, int loadPin) {

  byte data;

  digitalWrite(loadPin, LOW); //A-Hを格納
  digitalWrite(loadPin, HIGH); //確定

  data = digitalRead(dataPin) << 7; //Hの値を読む

  for (int i = 6; i >= 0; i--) {
    digitalWrite(clockPin, HIGH);
    data |= digitalRead(dataPin) << i; //G,F,E...Aの値を読む
    digitalWrite(clockPin, LOW);
  }

  return data;
}

//ゲームオーバー時の処理
void gameover() {

  flagGameover = true;
  for (int i = 0; i < 2; i++) cleared[i] = false;

  Scheduler.start(playSoundGameover);

}

//ゲームクリア時の処理
void gameclear() {
  flagGameover = true;

  //? ここでも音楽流す？
}

void blinkLED(int pin) {
  digitalWrite(pin, 1);
  Scheduler.delay(100);
  digitalWrite(pin, 0);
}



byte lastData[] = {0, 0};

//モジュールからの信号を受け取る
void readModuleData() {
  for (int i = 0; i < 2; i++) {
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

    //モジュールからのデータを処理する

    switch (id) {
      case 0: // wires
        if (flags == 0b111011) {
          cleared[i] = true;
        }
        break;
      case 1: // buttons
        if (flags == 0b110100) {
          cleared[i] = true;
        }
        if (flags == 0b101100) {
          timer_basis = 500;
          miss_flag = true;
        }
        if (lastData[i] & (lastData[i] ^ data)) { // negedge data[x]
          Scheduler.start(blinkLED, pin_LEDs[i]);
        }
        break;
    }

    lastData[i] = data;
  }
}



ul last_time_millis = 0UL;//1ミリ秒前の処理時刻を記録
//ui cnt = 0;

ui cnt_miss = 0;//ミス時のフラグ管理

void loop() {

  Scheduler.yield();//Scheduler.startLoopで起動している処理を進める(?)

  ul time_millis = millis();//起動してからのミリ秒時間の記録
  //  int timeLeftMinutes = 0, timeLeftSeconds = 0;

  if (last_time_millis != time_millis) { //ミリ秒レベルで前の処理した時刻と異なるとき

    if (flagGameover) {//ゲーム終了後のループ処理

    } else if (miss_flag) { //間違い時の特殊処理
      if(cnt_miss == 0){
        if(!mute)Scheduler.start(playBeep, 800);
        cnt_miss = 1;
      }
      else if(cnt_miss == 1100){//ミスしてから1100ms経過
        //ミスに関する諸フラグを下す
        miss_flag = false;
        cnt_miss = 0;
      }else{
        ++cnt_miss;
        //何もしない
      }
        
//      delay(1000);
       last_time_millis = time_millis; //前に処理した時刻を記録
      

    } else {//ゲーム中のループ処理


      //残り時間の更新処理
      if (timeLeft > 0) { //時間が残っているとき
        if (time_millis % timer_basis < last_time_millis % timer_basis) { //カウント基準時間が経過したとき
          //!計算量が若干多い
          //意図としては、ちょうど経過したタイミングに重い処理が走っていても時刻経過の判定を出せるようにするため

          timeLeft--;//残り時間を1引く

          //時間が進んだ時に走らせる処理
          if (DEBUG)Serial.println(String(timeLeft / 60) + ":" + String(timeLeft % 60));

          if (!mute) Scheduler.start(playSoundTick);//ビープ音を鳴らす
        }

      } else { //時間切れ
        gameover();
      }

      if (time_millis % 50 < last_time_millis % 50) {//50msごとにモジュールのデータを取得
        readModuleData();
      }

      //クリア判定
      if (cleared[0] & cleared[1]) {
        gameclear();
      }

    }

    for (int i = 0; i < 2; i++) {
      if (cleared[i]) digitalWrite(pin_LEDs[i], cleared[i]);
    }

    //  cnt++;
    last_time_millis = time_millis; //前に処理した時刻を記録
  } else {  //ims経過する前に走らせる処理


  }
  //  delay(dt);

}
