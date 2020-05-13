#define pwmPin 11

void setup() {
  pinMode(pwmPin, OUTPUT);
  digitalWrite(pwmPin, HIGH);
}

void loop() {
  analogWrite(pwmPin, 570); // 1550 STOP // 1600 MAX SPEED /570 MAX SPEED 
  delay(5000);
}
