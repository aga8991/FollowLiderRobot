/*_____Distance_sensors_control_functions_____*/

/*___Defines___*/ 
#define echoPinM 6 
#define trigPinM 7

/*___Functions___*/
long ReadDist2()         //middle sensor
{
    long duration = 0;
    digitalWrite(trigPinM, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPinM, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPinM, LOW);
    duration = pulseIn(echoPinM, HIGH); 
    return duration;
}

int ReadDistance(uint8_t SensorIndex)     //1 - left, 2 - middle, 3 - right sensor
{
    int distance = 0;
    long duration = 0;
    //if (SensorIndex == 2) 
    duration = ReadDist2();
   // else if (SensorIndex == 1) duration = ReadDist1();
   // else if (SensorIndex == 3) duration = ReadDist3();
    distance = duration * 0.034 / 2;
    if (distance <= 200 && distance > 0)return distance;     //return distance in cm or 0 when to close or too far
    else return 0;
}

uint16_t correctDist(uint8_t sensorIndex)
{
  uint16_t temp_distance = ReadDistance(sensorIndex);
  if(temp_distance == 0)
  {
    for(uint8_t i = 0; i < 3; i++)
    {
      temp_distance = ReadDistance(sensorIndex);
      if(temp_distance != 0 && temp_distance <=200) break;
    }
  }
  return temp_distance;
}
