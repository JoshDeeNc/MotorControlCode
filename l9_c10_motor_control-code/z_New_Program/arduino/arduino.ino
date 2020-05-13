#define PWMA                 9
#define ENCODE             2
double Input,Setpoint, kp=0.5, ki=0.1;
long Velocity_L;
long lastLoopTime;
long loopTime;
double error = 0;
double errorI = 0; 
int pwmVal;
void setup() {

	TCCR1B = TCCR1B & B11111000 | B00000100;          //设置pwm频率
	pinMode(PWMA, OUTPUT);
	pinMode(ENCODE, INPUT);
	analogWrite(PWMA, 0);
	Serial.begin(115200);
	attachInterrupt(0, READ_ENCODER_L, CHANGE);           //开启外部中断 编码器接口1
	Setpoint = 120;
}
void loop() {

	if (Serial.available()) {
		String rec = Serial.readStringUntil('\n');
		kp = rec.substring(rec.indexOf('p') + 1, rec.indexOf('i')).toFloat();
		ki = rec.substring(rec.indexOf('i') + 1, rec.indexOf('t')).toFloat();
		Setpoint = rec.substring(rec.indexOf('t') + 1, rec.indexOf('\n')).toFloat();
	}
	loopTime = millis() - lastLoopTime;
	if (loopTime > 20) {
		Input =  Velocity_L / 7.0 *(1000.0/loopTime);
		error = Setpoint - Input;
		errorI += (error * loopTime);
		lastLoopTime = millis();
		pwmVal += (kp * error) + (ki * errorI);
		pwmVal = constrain(pwmVal, 0, 255);
		analogWrite(PWMA, pwmVal);
		Velocity_L = 0;
		Serial.print(int(Setpoint));
		Serial.print(',');
		Serial.print(int(Input));
		Serial.print(',');
		Serial.println(pwmVal);

	}
}

void READ_ENCODER_L() {    // 码盘计数
	Velocity_L++;
}