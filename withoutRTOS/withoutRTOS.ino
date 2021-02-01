//uint8_t readSerialAP();                               //returns if "Start" was pressed 1-yes, 0-no
void adjustMotorsSpeed(uint16_t distance_average);
void directionByRSSI();
void buttonPressed();
 
#include "MotorsControl.h"
#include "DistanceSensorsControl.h"
#include "ESPControl.h"

#define OnOff 21

/*---Variables---*/
uint16_t MAX_distance = 120;
uint16_t MIN_distance = 80;

void setup() 
{
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200);
  Serial3.begin(115200);
  pinMode(LBmotor, OUTPUT);
  pinMode(LAmotor, OUTPUT);
  pinMode(RBmotor, OUTPUT);
  pinMode(RAmotor, OUTPUT);
  pinMode(trigPinM, OUTPUT);
  pinMode(echoPinM, INPUT_PULLUP);
}

void loop() 
{
  if(digitalRead(OnOff) == HIGH)
  {
    uint16_t dist_front = correctDist(2);              //distance from distance sensor
    if((dist_front != 0)&&(dist_front < 120)) 
    {
      adjustMotorsSpeed(dist_front);      //owner still in front of robot
      Serial.println("\nFront visible ");
      Serial.println(dist_front);
    }
    else
    {
      Serial.println("\nTurn side ");
      directionByRSSI();   
    }
  }
}

/*____MotorsSpeed_Control_Functions____*/
void adjustMotorsSpeed(uint16_t distance_average)
{
  if(distance_average <= MIN_distance) motors_speed = 0;
  else if(distance_average > MAX_distance) 
  {
    motors_speed += 5;
    if(motors_speed > 255) motors_speed = 255;
  }
  else if(distance_average < 100) 
  {
    motors_speed -= 100; 
    if(motors_speed < 0) motors_speed = 0;  
  }
  goFoward(motors_speed);
}
