#define pwmPin 11

#define button1 8
#define button2 9
#define button3 10

int neutral = 1550;
int maxClockWise = 1600;
int maxAntiClockWise = 570;

int button1Press = 0;
int button2Press = 0;
int button3Press = 0;
int desiredLevel = 1;
void setup() {
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(pwmPin, OUTPUT);
  digitalWrite(pwmPin, HIGH);
}

int going_up() {
  analogWrite(pwmPin, maxClockWise);
  delay(1000);
  
  return 1;
}

int going_down() {
  analogWrite(pwmPin, maxAntiClockWise);// 1550 STOP // 1600 MAX SPEED /570 MAX SPEED 
  delay(5000);
  
  return 1;
}

void loop() {

  analogWrite(pwmPin, neutral);
  delay(1000);
  
  button1Press = digitalRead(button1);
  button2Press = digitalRead(button2);
  button3Press = digitalRead(button3);

  desiredLevel = button1Press == HIGH ? 1 : button2Press == HIGH ? 2 : button3Press == HIGH ? 3 : -1;
  if(desiredLevel != -1 && desiredLevel != currentLevel) {
    while(currentLevel != desiredLevel) {
      if(currentLevel > desiredLevel) Serial.println("Going Down"), currentLevel -=going_down();
      else if(currentLevel < desiredLevel) Serial.println("Going Up"), currentLevel +=going_up();
      Serial.print("You are in level"); Serial.println(currentLevel);
      delay(1000);
    }
    digitalWrite(motorA, LOW);
    digitalWrite(motorB, LOW);
    desiredLevel = -1;
  }

 

}
