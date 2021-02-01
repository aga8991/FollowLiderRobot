/*_____ESPs_control_functions_____*/
uint8_t cutAndCountAvarage(String got);
void directionByRSSI();

/*___Defines___*/
#define OnOff 21                    //pin connected to ESP AP 13, which gets signal from App

/*___Variables___*/
uint8_t startPressed = 0;           //Start button: 1-pressed, 0-not pressed

/*___Functions___*/
int8_t readSerialL()
{
    String x;
    if (Serial1.available())
    {
        x = Serial1.readString();
        int8_t xInt = cutAndCountAvarage(x);
        Serial.print("L: ");
        Serial.print(xInt);
        Serial.print("\n");
        return xInt;
    }
    else return 0;
}

int8_t readSerialR()
{
    String x;
    if (Serial2.available())
    {
        x = Serial2.readString();
        int8_t xInt = cutAndCountAvarage(x); 
        Serial.print("R: ");
        Serial.print(xInt);
        Serial.print("\n");       
        return xInt;
    }
    else return 0;
}

uint8_t cutAndCountAvarage(String got)
{
  uint8_t L = got.length();
  uint8_t Avr = 0;
  uint16_t suma = 0;
  if(L%3 == 0)
  {
    L = L/3;
    for(uint8_t i = 0; i < L; i++)
    {
      String temp = got.substring(1+(3*i),3+(3*i));
      //Serial.print(temp);
      //Serial.print("\t");
      uint8_t tempInt = temp.toInt();
      suma += tempInt;
    }
    Avr = suma/L;
  }
  else int8_t Avr = got.toInt();
  return Avr;
}

void directionByRSSI()
{
  uint8_t rightRSSI_av = 0;
  uint8_t leftRSSI_av = 0;
  
  while(rightRSSI_av == 0) rightRSSI_av = readSerialR();
  while(leftRSSI_av == 0)leftRSSI_av = readSerialL();
  Serial.println(rightRSSI_av);
  Serial.println("\n");
  Serial.println(leftRSSI_av);
  Serial.println("\n");

  
  if((rightRSSI_av > 60) && (leftRSSI_av > 60)) 
  {
    Serial.println("buzzzz"); 
  } //alert
  else if(rightRSSI_av > leftRSSI_av + 8) 
  {
    Serial.println("\nlewooooo");
    goLeft(100);
    delay(10);
    directionByRSSI();
  }
  else if(leftRSSI_av > rightRSSI_av + 8) 
  {
    Serial.println("\nprawooooo");
    goRight(100);
    delay(10);
    directionByRSSI();   
  }
  else 
  {
    Serial.println("\nprzooood");
    motors_speed = 0;
    goFoward(motors_speed);
    return;
  }
}
