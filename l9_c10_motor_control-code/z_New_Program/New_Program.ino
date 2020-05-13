#define Buzzer 5
#define motorPWM 6
#define motorA 7
#define motorB 8

#define button1 9
#define button2 10
#define button3 11

#define echo 12
#define trig 13

int button1Press = 0;
int button2Press = 0;
int button3Press = 0;

int currentLevel = 1;
int desiredLevel = -1;

void setup() {
  Serial.begin(9600);

  pinMode(Buzzer, OUTPUT);
  
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  pinMode(motorPWM, OUTPUT);
  pinMode(motorA, OUTPUT);
  pinMode(motorB, OUTPUT);
  digitalWrite(motorA, LOW);
  digitalWrite(motorB, LOW);
  Serial.println("You are in level 1");

}

int sensor() {
  unsigned long duration;
  int distance;
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(5);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  distance = int(duration/2/29.412);
  return distance;
}

int going_up() {
  analogWrite(motorPWM, 255);
  
  while(sensor() > 12) {
    digitalWrite(motorA, LOW);
    digitalWrite(motorB, HIGH);
  }
  return 1;
}

int going_down() {
  analogWrite(motorPWM, 255);
  
  while(sensor() > 12) {
    digitalWrite(motorA, HIGH);
    digitalWrite(motorB, LOW);
  }
  return 1;
}


void loop() {
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
