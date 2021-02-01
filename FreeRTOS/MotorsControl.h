/*_____Motors_control_functions_____*/


/*___Defines___*/
#define LBmotor 13    //left  motor A1-B connected to D13
#define LAmotor 12    //left  motor A1-A connected to D12
#define RBmotor 11    //right motor B1-B connected to D11
#define RAmotor 10    //right motor B1-A connected to D10

/*___Variables___*/
uint16_t motors_speed = 100;    //in pwm 0-255

/*___Functions___*/
void leftMotorForward(uint16_t speedPWM)
{
	analogWrite(LAmotor, speedPWM);
	analogWrite(LBmotor, 0);
}

void leftMotorBackward(uint16_t speedPWM)
{
	analogWrite(LAmotor, 0);
	analogWrite(LBmotor, speedPWM);
}

void rightMotorForward(uint16_t speedPWM)
{
	analogWrite(RAmotor, speedPWM);
	analogWrite(RBmotor, 0);
}

void rightMotorBackward(uint16_t speedPWM)
{
	analogWrite(RAmotor, 0);
	analogWrite(RBmotor, speedPWM);
}

void goFoward(uint16_t speedPWM)
{
	rightMotorForward(speedPWM);
	leftMotorForward(speedPWM);
}

void goBackward(uint16_t speedPWM)
{
	rightMotorBackward(speedPWM);
	leftMotorBackward(speedPWM);
}

void goRight(uint16_t speedPWM)
{
	rightMotorBackward(0);
	leftMotorForward(speedPWM);
}

void goLeft(uint16_t speedPWM)
{
	rightMotorForward(speedPWM);
	leftMotorBackward(0);
}

void stopMotors()
{
	goFoward(0);
}
