const int DIN_PIN = 7;

void setup(){
    pinMode(DIN_PIN, INPUT_PULLUP);
    Serial.begin(9600);
}

void loop(){
    int value;
    
    value = digitalRead(DIN_PIN);
    Serial.println(value);

    delay(1000);
}