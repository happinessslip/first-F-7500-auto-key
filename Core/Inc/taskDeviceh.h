/**
  ******************************************************************************
  * @file           : taskDeviceh.h
  * @brief          : taskDevice.c header file
  ******************************************************************************
  * @attention
  *
  * 
  *
  ******************************************************************************
*/
#include "stm32f1xx_hal.h"

typedef struct __TaskInputPortTypeDef
{
  GPIO_TypeDef *GPIOx;
  int16_t GPIO_Pin;
  uint16_t vChatTime;
  
}TaskInputPortTypeDef;

void TaskInputThread(void);
void ResetADCData(void);
void TaskAdc1Thread(void);
void ResetJigIoPin(void);

