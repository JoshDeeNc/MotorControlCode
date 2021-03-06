#define Buzzer 5

#define pwmMot 6

#define halPin 8

#define butto1 9  //level 1
#define butto2 10 //level 2
#define butto3 11 //level 3

#define SeEcho 12
#define SeTrig 13

int button1_status = 0;
int button2_status = 0;
int button3_status = 0;

int current_level = 1;
int required_level =-1;

void setup(){
  Serial.begin(9600);
  pinMode(Buzzer, OUTPUT);
  digitalWrite(Buzzer, HIGH);
  pinMode(halPin, INPUT);
  pinMode(butto1, INPUT);
  pinMode(butto2, INPUT);
  pinMode(butto3, INPUT);
  pinMode(SeTrig, OUTPUT);
  pinMode(SeEcho, INPUT);
  pinMode(pwmMot, OUTPUT);
  Serial.println("you are in level 1");
}

void emergencyStop(){
  analogWrite(pwmMot, 255);
}
int sensor() {
  unsigned long duration;
  int distance;
  digitalWrite(SeTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(SeTrig, HIGH);
  delayMicroseconds(5);
  digitalWrite(SeTrig,LOW);
  duration = pulseIn(SeEcho,HIGH);  
  distance = int(duration/2/29.412);
  return distance;
}

bool hallSen(){
  return digitalRead(halPin);
}

int going_up(){
  while (sensor() > 12){
    analogWrite(pwmMot, 1600);
  }
  return 1;
}

int going_down(){
  while (sensor() > 12){
    analogWrite(pwmMot, 570);
  }
  return 1;
}

void loop() {
  button1_status = digitalRead(butto1);
  button2_status = digitalRead(butto2);
  button3_status = digitalRead(butto3);
  required_level = button1_status == HIGH ? 1 : button2_status == HIGH ? 2 : button3_status == HIGH ? 3 : -1;
  if (required_level != -1 && required_level != current_level){
    while (current_level != required_level){
      if (current_level > required_level) Serial.println("Going Down"), current_level-=going_down();
      else if (current_level< required_level) Serial.println("Going Up"), current_level+=going_up();
      Serial.print("you are in level "); Serial.println(current_level);
      delay(1000);
    }
    analogWrite(pwmMot, 1550); //stop elevator
    digitalWrite(Buzzer, LOW);
    delay(100);
    digitalWrite(Buzzer, HIGH);
    required_level = -1;
  }
}
