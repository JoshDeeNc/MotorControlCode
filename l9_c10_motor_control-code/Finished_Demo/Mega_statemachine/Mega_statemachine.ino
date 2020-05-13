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

void setup() {
  Serial.begin(9600);
  MOTOR.attach(motor, 500, 2500);
  for (int i=1; i<=5; i++) pinMode(hall_sensors[i], INPUT);
    for (int i=1; i<=2; i++) pinMode(lm[i], INPUT);
  //pinMode(top_limit_switch, INPUT);
  //pinMode(bottom_limit_switch, INPUT);
  
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
/* End This Part is for Motor and Sensor Team*/

/* 
 *  State Machine 
 *  
 *  General logic of the main loop:
 *  1. Read control box message for button presses and update targeted floors
 *  2. If lift is idle
 *      1. If there is a target floor in current direction
 *          1. Move lift in that direction
 *          2. Set lift to busy
 *      2. If there is no target floor in current direction
 *          1. Change the current direction of the lift      
 *  3. Else If lift is busy (in motion)
 *      1. If the limit switch has been been tripped
 *          1. Emergency stop
 *      2. If we have moved moved enough distance for one floor pass (checked with encoder)
 *          1. Update current floor
 *      3. If the current floor is equal to the target floor
 *          1. Stop the motor
 *          2. Set floor as no longer targeted.
 *          3. Set lift to idle/ready
 *  End Loop
 */

//STATES- 0=idle/ready  1=Busy/Task is being performed
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

//Main loop
void loop() {
  //Read control box message
  read_serial();

  //Lift is in an IDLE state
  if(state == 0) {
  
    //If a target_floor exists in the current lift_direction
    if(get_next_floor() != 0) {

        //Set state to busy
        state = 1;

        //Set target floor
        target_floor = get_next_floor();
        
        //Call drive team method to go in the current direction
        if(lift_direction == 1) 
          going_up();
        else going_down();
        
        if(debug) {
            Serial.print("Going to floor: ");
            Serial.print(target_floor);
            Serial.print("\n");
        }
    } else {
      //No target_floor found in current direction, change direction for next iteration
      lift_direction = -lift_direction;
      //Serial.println("No target floor found in current direction, changing direction");
    }

    //Lift is busy (in motion)
  } else if(state == 1) { 

      //Limit switch has been tripped, stop motor and alert control box
      if(checking_Limit_Switch() == true) {
        emergency_stop();
       if(debug)
        Serial.println("Limit switch has been tripped");
      }
    
    
      //If the encoder returns 1, one floor has been passed, update current floor
      if(checking_Hall_Sensors(lift_direction, current_floor, target_floor) == 1) {
        current_floor += lift_direction;
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

        going_up_speed = 1900;
        going_down_speed = 1100;
        
        //Reset floor entry and target floor, Reset state to idle
        floors[target_floor - 1] = false;
        target_floor = 0; 
        state = 0;               
        
        //Send floor-reached message to control box
        Serial.write((char)(current_floor + 48));

        
        //Debug
        if(debug) {
          Serial.println("Lift has reached target floor, waiting 5sec");
          Serial.println("------------------------------------------------");
        }
        
        //Wait at the target floor for 5 seconds
        delay(1000);

        
      }
      
  
  }

}

//Checks for a target floor in the current lift_direction
//returns the floor number if found, 0 otherwise
int get_next_floor() {

    //Iterate through floors[] from the current floor in the current lift direction
    for(int i = scurrent_floor - 1; i  < 5 && i >= 0; i += lift_direction) {
    
      //target floor has been found
      if(floors[i] == true) {         
        return i+1;
      }
    }

    //No target floor found in current lift direction, return false
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
  Serial.println("Stop button pressed, lift stopping");
}

//Read inputs sent from the control box
void read_serial() {
    char input[5];
    input[0] = 0;
    if (Serial.available() != 0) {
        Serial.readBytes(input, 1);
        if(debug) {
          Serial.print("Received: ");
          Serial.print(input[0]);
          Serial.print(" from control box\n");
         }
        switch (input[0]) {
          case 's':
            emergency_stop();
            break;
          case '1':
            floors[0] = true;
            break;
          case '2':
            floors[1] = true;
            break;
          case '3':
            floors[2] = true;
            break;
          case '4':
            floors[3] = true;
            break;
          case '5':
            floors[4] = true;
            break;     
        
        } 
    }
}
