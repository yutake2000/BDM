const int pin_buttons[] = {2, 4, 5, 6};
const int pin_wires[] = {15, 16, 17, 18, 19};
const int pin_LEDs[] = {7, 14}; // Buttons, Wires
const int pin_line[] = {8, 9};
const int pin_7seg[] = {10, 11, 12, 13};
const int pin_speaker = 3;

const int timelimit = 599;
const int dt = 10;

void setup() {
  for (int i = 0; i < 4; i++) pinMode(pin_buttons[i], INPUT_PULLUP);
  for (int i = 0; i < 5; i++) pinMode(pin_wires[i], INPUT_PULLUP);
  for (int i = 0; i < 2; i++) pinMode(pin_LEDs[i], OUTPUT);
  for (int i = 0; i < 2; i++) pinMode(pin_line[i], OUTPUT);
  for (int i = 0; i < 4; i++) pinMode(pin_7seg[i], OUTPUT);
  pinMode(pin_speaker, OUTPUT);

  Serial.println("b0\tb1\tb2\tb3\tw0\tw1\tw2\tw3\tw4");
}

void write7seg(int n) {
  for (int i = 0; i < 4; i++) digitalWrite(pin_7seg[i], (n >> i) & 1);
}

int cnt = 0;
bool cleared[] = {false, false};
void loop() {
  int timeLeft = timelimit - millis() / 1000;
  int timeLeftMinutes = timeLeft / 60;
  int timeLeftSeconds = timeLeft % 60;

  if (cnt * dt % 1000 == 0) { // every 1000 ms
    int value;
    for (int i = 0; i < 4; i++) {
      value = digitalRead(pin_buttons[i]);
      Serial.print(value + "\t");
    }

    for (int i = 0; i < 5; i++) {
      value = digitalRead(pin_wires[i]);
      Serial.print(value + "\t");
    }
  }

  for (int i = 0; i < 2; i++) digitalWrite(pin_LEDs[i], cleared[i]);

  for (int i = 0; i < 2; i++) digitalWrite(pin_line[i], (cnt >> i) & 1);

  if (cnt % 4 == 0) write7seg(timeLeftMinutes);
  else if (cnt % 4 == 1) write7seg(7); // display ":"
  else if (cnt % 4 == 2) write7seg(timeLeftSeconds / 10);
  else write7seg(timeLeftSeconds % 10);

  cnt++;

  delay(dt);
}
