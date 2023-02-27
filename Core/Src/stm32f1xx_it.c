/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_it.h"
#include "FreeRTOS.h"
#include "task.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cmsis_os.h"
#include "my_generic.h"
#include "define_consts.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
//extern unsigned short gu16TimerJob,gu16TimerMake100ms;
//extern DWORD_VAL gu32Flag;
uint8_t gu8Uart1Rxbuff[MAX_RX_DATA],gu8Uart1Txbuff[15],gu8UartRxMax;
uint8_t gu8RxData[MAX_RX_DATA], gu8Uart1RxAddr, gu8Uart1RxFunc;
uint8_t gu8Uart1RxCnt,gu8Uart1TxCnt,gu8Uart1TxCntMax,gu8TaskUart1Tx;
uint16_t gu16TimerUart1;
uint8_t gu8UartTmpStep;
extern WORD_VAL gu16Flag1; 
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern UART_HandleTypeDef huart1;
/* USER CODE BEGIN EV */
extern osEventFlagsId_t FlagEvent01Handle;
extern osThreadId_t FunctionJobHandle;

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
#if (INCLUDE_xTaskGetSchedulerState == 1 )
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
  {
#endif /* INCLUDE_xTaskGetSchedulerState */
  xPortSysTickHandler();
#if (INCLUDE_xTaskGetSchedulerState == 1 )
  }
#endif /* INCLUDE_xTaskGetSchedulerState */
  /* USER CODE BEGIN SysTick_IRQn 1 */
  ++gu16TimerUart1;
  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles USART1 global interrupt. 
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
  if(USART1->SR & USART_SR_RXNE){
		//USART1->SR &= ~USART_SR_RXNE;
		gu8UartTmpStep = (uint8_t)(USART1->DR & (uint32_t)0x00FF);
		if(FLAG_UART_RX1){
			gu8RxData[gu8Uart1RxCnt++]=gu8UartTmpStep;
			if(gu8Uart1RxCnt > gu8UartRxMax){
				FLAG_UART_RX1 = 0;
        osThreadFlagsSet( FunctionJobHandle, EVENT_FLAG_UART1_485); 
			}
      else if(gu8Uart1RxCnt == 3){
        gu8UartRxMax = gu8RxData[2] + 4;
        if(gu8UartRxMax > (MAX_RX_DATA-1)){
          FLAG_UART_RX1 = 0;
        }
      }
		}
		else{
			gu8RxData[0] = gu8RxData[1];
			gu8RxData[1] =  gu8UartTmpStep;
			if((gu8RxData[0] == gu8Uart1RxAddr) && (gu8RxData[1] == gu8Uart1RxFunc)){
				
				FLAG_UART_RX1 = 1;
				gu8Uart1RxCnt = 2;
        gu8UartRxMax = MAX_RX_DATA-1;//gu8RxData[2] + 4;
				
			}
		}
	}
	if(FLAG_UART_TX1){
		if(USART1->SR & USART_SR_TXE){
			// USART1->SR &= ~USART_SR_TXE;
			if(gu8Uart1TxCnt < gu8Uart1TxCntMax){
				USART1->DR = (uint32_t)gu8Uart1Txbuff[gu8Uart1TxCnt];
				++gu8Uart1TxCnt;
        gu16TimerUart1=0;
			}
			else{
        __HAL_UART_DISABLE_IT(&huart1, UART_IT_TXE);
				FLAG_UART_TX1 = 0;
				osThreadFlagsSet( FunctionJobHandle, EVENT_FLAG_UART1_485); 
			}
		}
	}
  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
