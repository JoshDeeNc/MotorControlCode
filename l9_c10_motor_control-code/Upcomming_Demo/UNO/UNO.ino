#define button1 2
#define button2 3
#define button3 4

void setup() {
  Serial.begin(9600);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
}

void loop() {
  if (digitalRead(button1) == HIGH) Serial.write(1);
  else if (digitalRead(button2) == HIGH) Serial.write(2);
  else if (digitalRead(button3) == HIGH) Serial.write(3);
}
