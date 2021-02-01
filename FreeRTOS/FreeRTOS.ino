#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <Math.h>
#include "MotorsControl.h"
#include "DistanceSensorsControl.h"
#include "ESPControl.h"

#define on_off 21

void Task_GoingFoward(void *param);
void Task_CheckRssi(void *param);
void Task_Lost(void *param);
void Task_TurnRight(void *param);
void Task_TurnLeft(void *param);
void Task_Disappear(void *param);

void LostSignal();

uint16_t distance = 0;
uint16_t MAX_distance = 100;
uint16_t MIN_distance = 60;

TaskHandle_t Task_GoingFoward_Handler;
TaskHandle_t Task_CheckRssi_Handler;
TaskHandle_t Task_Lost_Handler;
TaskHandle_t Task_TurnRight_Handler;
TaskHandle_t Task_TurnLeft_Handler;
TaskHandle_t Task_Disappear_Handler;

void setup() 
{
  /* start/stop pressed */
  attachInterrupt(on_off, start_stop_pressed, CHANGE);
  
  /*motors*/
  pinMode(LBmotor, OUTPUT);
  pinMode(LAmotor, OUTPUT);
  pinMode(RBmotor, OUTPUT);
  pinMode(RAmotor, OUTPUT);
  /*distance sensor*/
  pinMode(trigPinM, OUTPUT);
  pinMode(echoPinM, INPUT_PULLUP);

  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200);

  xTaskCreate(Task_GoingFoward,"taskDist",100,NULL,1,&Task_GoingFoward_Handler);
  xTaskCreate(Task_CheckRssi,"taskRSSI",100,NULL,1,&Task_CheckRssi_Handler); 
  xTaskCreate(Task_Lost,"taskLost",100,NULL,2,&Task_Lost_Handler);
  xTaskCreate(Task_Disappear,"taskDisappear",100,NULL,1,&Task_Disappear_Handler);
  xTaskCreate(Task_TurnRight,"taskRight",100,NULL,1,&Task_TurnRight_Handler);
  xTaskCreate(Task_TurnLeft,"taskLeft",100,NULL,1,&Task_TurnLeft_Handler);
  vTaskSuspend(Task_GoingFoward_Handler);
  vTaskSuspend(Task_CheckRssi_Handler);
  vTaskSuspend(Task_Lost_Handler);
  vTaskSuspend(Task_TurnRight_Handler);
  vTaskSuspend(Task_TurnLeft_Handler);
  vTaskSuspend(Task_Disappear_Handler);
  vTaskStartScheduler();
}

void loop() {}

void Task_GoingFoward(void *param)
{
  (void) param;
  TickType_t getTick;
  getTick = xTaskGetTickCount();
  Serial.println("Going forward task\n");
  
  for(;;)
  {
    distance = correctDist(2);
    Serial.print("D:");
    Serial.println(distance);
    if(distance == 0)
    {
      vTaskResume(Task_Disappear_Handler);
      vTaskSuspend(Task_CheckRssi_Handler);
      Serial.println("Going forward task not active\n");
      vTaskSuspend(NULL);
    }
    if(distance <= MIN_distance) motors_speed = 0;
    else if(distance > MAX_distance) 
    {
      motors_speed += 5;
      if(motors_speed > 255) motors_speed = 255;
    }
    else if(distance < 80) 
    {
      motors_speed -= 100; 
      if(motors_speed < 0) motors_speed = 0;  
    }
    goFoward(motors_speed);
    vTaskDelayUntil(&getTick,100/portTICK_PERIOD_MS);
  }
}

void Task_CheckRssi(void *param)
{
   Serial.println("Check RSSI task\n");
  (void) param;

  for(;;)
  {
    uint8_t rightRSSI_av = 0;
    uint8_t leftRSSI_av = 0;
    while(rightRSSI_av == 0) rightRSSI_av = readSerialR();
    while(leftRSSI_av == 0) leftRSSI_av = readSerialL();
   Serial.println("rssi:");
   Serial.println(rightRSSI_av);
   Serial.println("\n");
   Serial.println(leftRSSI_av);
   Serial.println("\n");
  
    if((rightRSSI_av > 65) && (leftRSSI_av > 65)) 
    {
      Serial.println("Check RSSI task not active\n");
      LostSignal();
    } 
    else if(rightRSSI_av > leftRSSI_av + 12) 
    { 
      vTaskResume(Task_TurnLeft_Handler);
      Serial.println("Check RSSI task not active\n");
    }
    else if(leftRSSI_av > rightRSSI_av + 12) 
    {
      vTaskResume(Task_TurnRight_Handler);
      Serial.println("Check RSSI task not active\n");
    }  
    vTaskDelay(200/portTICK_PERIOD_MS);
  }
}

void Task_Lost(void *param)
{
  (void) param;
  goFoward(0);
  //buzzer turn on;

  Serial.println("Lost task active\n");
  for(;;)
  {
    uint8_t rightRSSI_av = 0;
    uint8_t leftRSSI_av = 0;
    while(rightRSSI_av == 0) rightRSSI_av = readSerialR();
    while(leftRSSI_av == 0)leftRSSI_av = readSerialL();
    if((rightRSSI_av < 50) && (leftRSSI_av < 50)) 
    {
      //buzzer off
      //if button pressed
      {
        Serial.println("Lost task not active\n");
        vTaskResume(Task_CheckRssi_Handler);
        vTaskResume(Task_GoingFoward_Handler);
        digitalWrite(20, LOW);
        vTaskSuspend(NULL);
      }
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}

void Task_TurnRight(void *param)
{
  (void) param;
  vTaskSuspend(Task_CheckRssi_Handler);
  vTaskSuspend(Task_GoingFoward_Handler);

  Serial.println("Turn right task active\n");
  goRight(100);
  for(;;)
  {
    uint8_t rightRSSI_av = 0;
    uint8_t leftRSSI_av = 0;
    while(rightRSSI_av == 0) rightRSSI_av = readSerialR();
    while(leftRSSI_av == 0)leftRSSI_av = readSerialL();
    if(abs(rightRSSI_av - leftRSSI_av) <= 8) 
    {
      goFoward(150);
      vTaskResume(Task_CheckRssi_Handler);
      vTaskResume(Task_GoingFoward_Handler);
      Serial.println("Turn right task not active\n");
      vTaskSuspend(NULL);
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}

void Task_TurnLeft(void *param)
{
  (void) param;
  vTaskSuspend(Task_CheckRssi_Handler);
  vTaskSuspend(Task_GoingFoward_Handler);

  Serial.println("Turn left task active\n");
  goLeft(100);
  for(;;)
  {
    uint8_t rightRSSI_av = 0;
    uint8_t leftRSSI_av = 0;
    while(rightRSSI_av == 0) rightRSSI_av = readSerialR();
    while(leftRSSI_av == 0)leftRSSI_av = readSerialL();
    if(abs(rightRSSI_av - leftRSSI_av) <= 8) 
    {
      vTaskResume(Task_CheckRssi_Handler);
      vTaskResume(Task_GoingFoward_Handler);
      Serial.println("Turn left task not active\n");
      vTaskSuspend(NULL);
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}

void Task_Disappear(void *param)
{
  (void) param;
  Serial.println("DIsappear task active\n");
  
  for(;;)
  {
    uint8_t rightRSSI_av = 0;
    uint8_t leftRSSI_av = 0;
    while(rightRSSI_av == 0) rightRSSI_av = readSerialR();
    while(leftRSSI_av == 0)leftRSSI_av = readSerialL();
  
    if((rightRSSI_av > 65) && (leftRSSI_av > 65)) 
    {
      Serial.println("DIsappear task not active\n");
      LostSignal();
      vTaskSuspend(NULL);
    } 
    else if(rightRSSI_av > leftRSSI_av + 8) 
    { 
      Serial.println("DIsappear task not active\n");
      vTaskResume(Task_TurnLeft_Handler);
      vTaskSuspend(NULL);
    }
    else if(leftRSSI_av > rightRSSI_av + 8) 
    {
      Serial.println("DIsappear task not active\n");
      vTaskResume(Task_TurnRight_Handler);
      vTaskSuspend(NULL);
    } 
    else
    {
      Serial.println("DIsappear task not active\n");
      goFoward(150);
      delay(100);
      vTaskResume(Task_GoingFoward_Handler);
      vTaskResume(Task_CheckRssi_Handler);
      vTaskSuspend(NULL);
    }
    vTaskDelay(500/portTICK_PERIOD_MS);
  }
}

void LostSignal()
{ 
  Serial.println("Alert!!!\n");
  vTaskSuspend(Task_CheckRssi_Handler);
  vTaskSuspend(Task_GoingFoward_Handler);
  vTaskSuspend(Task_Disappear_Handler);
  vTaskResume(Task_Lost_Handler);
}

void start_stop_pressed()
{
  if(digitalRead(on_off)==HIGH)
  {
    vTaskResume(Task_GoingFoward_Handler);
    vTaskResume(Task_CheckRssi_Handler);
  }
  else
  {
    vTaskSuspend(Task_GoingFoward_Handler);
    vTaskSuspend(Task_CheckRssi_Handler);
    vTaskSuspend(Task_Lost_Handler);
    vTaskSuspend(Task_TurnRight_Handler);
    vTaskSuspend(Task_TurnLeft_Handler);
    vTaskSuspend(Task_Disappear_Handler);
  }
}
