/*
  Comms - Arduino MEGA
  Teams: L9_C_10, L3_C_10
*/

#include <Servo.h>

// TODO: Change motor pin based on PWM pins
const int Motor_pin1 = 6;

// Motor constants
const int Motor_max_forward = 800; //going up
const int Motor_max_reverse = 1800; //going down
const int Motor_stop = 1440;   //stop
const int time_up = 500;      //time to going up each level
const int time_down = 500;    //time to going down to each level

// Address for comms link
const long commsAddress = 9600;

// Max number of floors
const int numFloors = 5;

// Current command sent over from Control Box
// (0 = Do nothing (default); 1 = Stop lift; 2 = Go up; 3 = Go down)
int controlBoxCommand = 0;

// Current floor
int currentFloor = 1;

// Servo object
Servo Motor1;


// Main setup function which initialises everything
void setup() {
	Motor1.attach(Motor_pin1, 400, 2600);
	setupCommsLink();
}

// Function to setup comms link to RX Arduino UNO
void setupCommsLink() {
	Serial.begin(commsAddress);
}


// Main loop function which listens for changes
void loop() {
	if(receiveComms() != 0) {

		if(controlBoxCommand == 1) {

			// Stop lift
			emergencyStop();
			delay(10000);

		} else if(controlBoxCommand == 2) {

			// Go up one floor
			if(currentFloor != numFloors) {
				currentFloor++;
                goUp();
				delay(5000);
			}

		} else if(controlBoxCommand == 3) {

			// Go down one floor
			if(currentFloor != 1) {
				currentFloor--;
                goDown();
				delay(5000);
			}
			
		}

		// Send success/failure response to Control Box (0 = Success; 1 = Error)
		sendComms(0);
		controlBoxCommand = 0;
	}
}

// Function to send a value to Control Box (Arduino UNO)
void sendComms(int value) {
	Serial.print(value);
}

// Function to receive a floor value from Control Box
// (0 = Do nothing (default); 1 = Stop lift; 2 = Go up; 3 = Go down)
int receiveComms() {
	while(Serial.available() != 0) {
        controlBoxCommand = Serial.parseInt();
    }
	return controlBoxCommand;
}

// Function to move lift up one floor
void goUp() {
    delay(10);
    Motor1.writeMicroseconds(Motor_max_forward);
    delay(time_up); //time need to be measured
	Motor1.writeMicroseconds(Motor_stop);
	delay(10);
}

// Function to move lift down one floor
void goDown() {
  delay(10);
  Motor1.writeMicroseconds(Motor_max_reverse);
  delay(time_down); //time need to be measured
  Motor1.writeMicroseconds(Motor_stop);
  delay(10);
  
}

// Function to stop lift
void emergencyStop() {
    delay(10);
    Motor1.writeMicroseconds(Motor_stop);
    delay(10);
}
