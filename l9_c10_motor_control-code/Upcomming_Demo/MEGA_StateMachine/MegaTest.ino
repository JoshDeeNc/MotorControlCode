/*
  State Machine & Motors - Arduino MEGA (Lift 9)
  Team: L9_C_20 & L9_C_10
*/

#include <Encoder.h>
#include <Servo.h>
#include <SoftwareSerial.h>

#define motor 6
#define top_limit_switch 7
#define bottom_limit_switch 8
int going_up_speed = 1900;
int going_down_speed = 1100;
int stop_motor_value = 1500;
int hall_sensors[6] = {0, 9, 10, 11, 12, 13};
int lm[4] = {0, 7, 8};
Servo MOTOR;
Encoder myEnc(2, 3);

//STATES- -1=setup   0=idle/ready  1=Busy/Task is being performed
int state = 0;

//Bool array with each entry representing one floor. A true entry represents a floor pressed on the control box.
bool floors[5] = {false, false, false, false, false};

//Current floor of the lift
int current_floor = 1;

//Direction the lift is moving, 1=UP  -1=DOWN
int lift_direction = 1;

//Floor the lift is attempting to reach, 0 if none
int target_floor = 0;

bool debug = true;

void setup() {
  Serial.begin(9600);
  MOTOR.attach(motor, 500, 2500);
  for (int i=1; i<=5; i++) pinMode(hall_sensors[i], INPUT);
    for (int i=1; i<=2; i++) pinMode(lm[i], INPUT);
  //pinMode(top_limit_switch, INPUT);
  //pinMode(bottom_limit_switch, INPUT);
  
  setup_lift();
}

//Make_Motor_Move
void going_up()  {MOTOR.writeMicroseconds(going_up_speed);} 
void going_down(){MOTOR.writeMicroseconds(going_down_speed);}
void motor_stop(){MOTOR.writeMicroseconds(stop_motor_value);}
void Encoder_Speed_Adjustment(){
}
//End_Make_Motor_Move

//Checking_Encoder_perLevel
int oldPosition = 0;
/*int Encoder_per_level(){
  int value = 0;
    int newPosition = myEnc.read();
      if (abs(newPosition-oldPosition) > 3977 && abs(newPosition-oldPosition) < 3987){
          value = 1;
          oldPosition = newPosition;
      }
    return value;
}*/
bool checking_Limit_Switch(){
   for (int i=1; i<=2; i++){
    if (digitalRead(lm[i]) == LOW) return true; 
   }
   return false;
}
int checking_Hall_Sensors(int lift_direction, int currentLevel, int desiredLevel){
  if (digitalRead(hall_sensors[currentLevel + lift_direction]) == LOW && currentLevel + lift_direction == desiredLevel) {
    oldPosition = myEnc.read();
    going_up_speed = 1900;
    going_down_speed = 1100;
    return 1;
  }
  else if (digitalRead(hall_sensors[currentLevel+lift_direction]) == LOW) {
      oldPosition = myEnc.read(); 
      return 1;
  }
  else if (currentLevel - desiredLevel == 1) going_down_speed = 1280, going_down();
  else if (desiredLevel - currentLevel == 1) going_up_speed = 1700, going_up();
  return 0;
}



//Returns the level of the lift if found by the hall effect sensors, 0 otherwise
int hall_effect_check() {
  for(int i = 1; i < 6; i++) {
    if(digitalRead(hall_sensors[i]) == LOW) {
      return i;
    }
  }
  return 0;
}

//Slows the lift if within 1 flooor of arrival
void speed_adjust() {
  if (abs(current_floor - target_floor)==1){
     going_up_speed = 1700;
     going_down_speed = 1320;
  } else {
     going_up_speed = 1900;
     going_down_speed = 1100;
  }
  if(state == 1) {
    move_lift();
  }
}

void move_lift() {
  if(lift_direction == 1)
    going_up();
  else going_down();
}

//Startup calibration of the lift
void setup_lift() {
  if(hall_effect_check() != 0) {
    current_floor = hall_effect_check();
    return;
  }
  //Slow the lift
  going_up_speed = 1700;
  going_down_speed = 1320;

  state = -1;
  while(state == -1) {    
    move_lift();
    if(checking_Limit_Switch() == true) {
      lift_direction = -lift_direction;
      move_lift();
      delay(150); //Allows time for the lift to move off the switch before the next check
    }

    current_floor = hall_effect_check();
    if(current_floor != 0) {
      motor_stop();
      state = 0;
    }
    read_serial();
  }  
  going_up_speed = 1900;
  going_down_speed = 1100;
}

//Main loop
void loop() {
  //Read control box message
  read_serial();

  //Set the target floor
  target_floor = get_next_floor();

  //Lift is in an IDLE state
  if(state == 0) {
  
    //If a target_floor exists in the current lift_direction
    if(target_floor != 0) {
      state = 1;
      speed_adjust();
      move_lift();
    } else {
      //No target_floor found in current direction, change direction for next iteration
      lift_direction = -lift_direction;
    }

    //Lift is busy (in motion)
  } else if(state == 1) { 
      //Adjust speed if within certain distance of target
      speed_adjust();

      //Limit switch has been tripped, stop motor and alert control box
      if(checking_Limit_Switch() == true) {
        emergency_stop();
       if(debug) 
        Serial.println("Limit switch has been tripped");
      }    
    
      //If the encoder returns 1, one floor has been passed, update current floor
      if(hall_effect_check() != 0) {
        current_floor = hall_effect_check();
        if(debug) {
            Serial.print("Current floor: ");
            Serial.print(current_floor);
            Serial.print("\n");
        }
      }

      //The lift has reached the target floor
      if(current_floor == target_floor) {
        //Stop the motor
        motor_stop();
        
        //Reset floor entry and target floor, Reset state to idle
        floors[target_floor - 1] = false;
        target_floor = 0; 
        state = 0;               
        
        //Send floor-reached message to control box
        Serial.write((char)(current_floor + 48));
        
        if(debug) {
          Serial.println("Lift has reached target floor, waiting 2sec");
          Serial.println("------------------------------------------------");
        }

        //Wait at the target floor for 2 seconds
        delay(2000);
       
      }   
  }

}

//Checks for a target floor in the current lift_direction
//returns the floor number if found, 0 otherwise
int get_next_floor() {

    //Iterate through floors[] from the current floor in the current lift direction
    for(int i = current_floor - 1; i  < 5 && i >= 0; i += lift_direction) {
    
      //target floor has been found
      if(floors[i] == true) { 
        if(debug) {
          Serial.print("TARGET FLOOR: ");
          Serial.print(target_floor);
          Serial.print("\n");
        }        
        return i+1;
      }
    }
  return 0;  
}

//Stop button has been pressed
void emergency_stop() {
  motor_stop();

  //Reset target floors to stop previous operations
  for(int i = 0; i < 5; i++) {
    floors[i] = false;
  }
  target_floor = 0;

  //Set state to idle to allow new operations
  state = 0;

  //Debug
  if(debug)
    Serial.println("Emergency stop");
}

//Read inputs sent from the control box
void read_serial() {
    char input[5];
    input[0] = 0;
    if (Serial.available() != 0) {
        Serial.readBytes(input, 1);
        int conversion = ((int)input[0]) - 49;
        if(conversion >= 0 && conversion <= 4) floors[conversion] = true;
        if(conversion == 66) emergency_stop();

        if(debug) {
          Serial.print("Received: ");
          Serial.print(conversion);
          Serial.print(" from control box\n");
         }
    }
}