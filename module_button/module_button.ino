const int pin_buttons[] = {2, 4, 5, 6};
const int pin_LEDs[] = {7, 14}; // Buttons, Wires
const int pin_7seg[] = {10, 11, 12, 13};
const int pin_line[] = {8, 9};
const int pin_wires[] = {15, 16, 17, 18, 19};
const int pin_speaker = 3;

void setup() {
  for (int i=0; i<4; i++) pinMode(pin_buttons[i], INPUT_PULLUP);
  for (int i=0; i<2; i++) pinMode(pin_LEDs[i], OUTPUT);
  for (int i=0; i<2; i++) pinMode(pin_7seg[i], OUTPUT);
  for (int i=0; i<2; i++) pinMode(pin_line[i], OUTPUT);
  for (int i=0; i<5; i++) pinMode(pin_wires[i], INPUT_PULLUP);
  pinMode(pin_speaker, OUTPUT);
}

void loop() {
    int value;

    for (int i=0; i<4; i++) {
      value = digitalRead(pin_buttons[i]);
      Serial.print(value + ", ");
    }

    delay(1000);
}
