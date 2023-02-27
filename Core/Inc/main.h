/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Relay_Wifi_Pin GPIO_PIN_2    // Relay_Wifi_Pin -> SOL_KEY3
#define Relay_Wifi_GPIO_Port GPIOE

#define Relay_RF_Pin  GPIO_PIN_3     // Relay_Temp_Pin -> SOL_KEY4
#define Relay_RF_GPIO_Port  GPIOE

#define Sol_SetSw_Pin GPIO_PIN_4     // Sol_SetSw_Pin -> SOL_SETSW1
#define Sol_SetSw_GPIO_Port GPIOE

#define Sol_OpClSw_Pin GPIO_PIN_5    // Sol_OpClSw_Pin -> SOL_OPELSW2
#define Sol_OpClSw_GPIO_Port GPIOE

#define Sol_OpClSw_Pin2 GPIO_PIN_6   // Sol_HandleSw_Pin -> SOL_RESET3
#define Sol_OpClSw_GPIO_Port2 GPIOE

#define State_Deadbolt_Pin GPIO_PIN_13  // State_Deadbolt_Pin -> State_Deadbolt_Pin
#define State_Deadbolt_GPIO_Port GPIOC

#define Spare_In1_Pin GPIO_PIN_14   // Spare_In1_Pin -> Spare_In1_Pin
#define Spare_In1_GPIO_Port GPIOC

#define En_UL1_Pin GPIO_PIN_4    // En_UL1_Pin -> En_UL1_Pin
#define En_UL1_GPIO_Port GPIOC
#define En_UL2_Pin GPIO_PIN_5    // En_UL2_Pin -> En_UL2_Pin
#define En_UL2_GPIO_Port GPIOC

#define Change_LowVol_Pin GPIO_PIN_0
#define Change_LowVol_GPIO_Port GPIOB
#define En_VBAT_Pin GPIO_PIN_1
#define En_VBAT_GPIO_Port GPIOB
#define En_VEMG_Pin GPIO_PIN_2
#define En_VEMG_GPIO_Port GPIOB
#define Reset485_Pin GPIO_PIN_7
#define Reset485_GPIO_Port GPIOE

#define Sol_HandleSw_Pin GPIO_PIN_8    // Sol_FixPbaOn_Pin -> SOL_KEY5
#define Sol_HandleSw_GPIO_Port GPIOE

#define Sol_FixPbaOn_Pin GPIO_PIN_9    // Sol_FixPbaOff_Pin -> SOL_FIXPBA
#define Sol_FixPbaOn_GPIO_Port GPIOE

#define Sol_TriggerOn_Pin GPIO_PIN_10   // Sol_TriggerOn_Pin -> SOL_TIRGGER
#define Sol_TriggerOn_GPIO_Port GPIOE

#define Sol_TriggerOff_Pin GPIO_PIN_11   // Sol_TriggerOff_Pin -> SOL_KEY6
#define Sol_TriggerOff_GPIO_Port GPIOE

#define SpareOut1_Pin GPIO_PIN_12   // SpareOut1_Pin -> SOL_KEY7
#define SpareOut1_GPIO_Port GPIOE

#define SpareOut2_Pin GPIO_PIN_13   // SpareOut2_Pin -> SOL_KEY8
#define SpareOut2_GPIO_Port GPIOE

#define SpareOut3_Pin GPIO_PIN_14   // SpareOut3_Pin -> SOL_KEY9
#define SpareOut3_GPIO_Port GPIOE

#define SpareOut4_Pin GPIO_PIN_15   // SpareOut4_Pin -> SOL_KEY0
#define SpareOut4_GPIO_Port GPIOE

#define SpareOut5_Pin GPIO_PIN_10   // SpareOut5_Pin -> SOL_KEYSTAR
#define SpareOut5_GPIO_Port GPIOB

#define SpareOut6_Pin GPIO_PIN_11   // SpareOut6_Pin -> SOL_KEYNUM
#define SpareOut6_GPIO_Port GPIOB

#define SpareOut7_Pin GPIO_PIN_12   // SpareOut7_Pin -> SOL_CARD
#define SpareOut7_GPIO_Port GPIOB

#define SOL_PROSTART_Pin GPIO_PIN_13    // SOL_PROSTART
#define SOL_PROSTART_GPIO_Port GPIOB

#define SOL_SPARE_OUT6_Pin GPIO_PIN_14    // SOL_SPARE_OUT6
#define SOL_SPARE_OUT6_GPIO_Port GPIOB

#define SOL_SPARE_OUT7_Pin GPIO_PIN_15    // SOL_SPARE_OUT7
#define SOL_SPARE_OUT7_GPIO_Port GPIOB

#define RELAY_SPARE_OUT1_Pin GPIO_PIN_10    // RELAY_SPARE_OUT1
#define RELAY_SPARE_OUT1_GPIO_Port GPIOD

#define RELAY_SPARE_OUT2_Pin GPIO_PIN_11    // RELAY_SPARE_OUT2
#define RELAY_SPARE_OUT2_GPIO_Port GPIOD

#define RELAY_SPARE_OUT3_Pin GPIO_PIN_12    // RELAY_SPARE_OUT3
#define RELAY_SPARE_OUT3_GPIO_Port GPIOD

#define RELAY_SPARE_OUT4_Pin GPIO_PIN_13    // RELAY_SPARE_OUT4
#define RELAY_SPARE_OUT4_GPIO_Port GPIOD

#define RS485_de_Pin GPIO_PIN_6
#define RS485_de_GPIO_Port GPIOC
#define LCD_DIR_Pin GPIO_PIN_7
#define LCD_DIR_GPIO_Port GPIOC
#define LCD_CS1_Pin GPIO_PIN_8
#define LCD_CS1_GPIO_Port GPIOC
#define SW2_Pin GPIO_PIN_9
#define SW2_GPIO_Port GPIOC
#define SW1_Pin GPIO_PIN_8
#define SW1_GPIO_Port GPIOA
#define LCD_CS2_Pin GPIO_PIN_11
#define LCD_CS2_GPIO_Port GPIOA
#define LCD_RES_Pin GPIO_PIN_12
#define LCD_RES_GPIO_Port GPIOA
#define LCD_RS_Pin GPIO_PIN_10
#define LCD_RS_GPIO_Port GPIOC
#define LCD_RW_Pin GPIO_PIN_11
#define LCD_RW_GPIO_Port GPIOC
#define LCD_E_Pin GPIO_PIN_12
#define LCD_E_GPIO_Port GPIOC
#define LCD_DB0_Pin GPIO_PIN_0
#define LCD_DB0_GPIO_Port GPIOD
#define LCD_DB1_Pin GPIO_PIN_1
#define LCD_DB1_GPIO_Port GPIOD
#define LCD_DB2_Pin GPIO_PIN_2
#define LCD_DB2_GPIO_Port GPIOD
#define LCD_DB3_Pin GPIO_PIN_3
#define LCD_DB3_GPIO_Port GPIOD
#define LCD_DB4_Pin GPIO_PIN_4
#define LCD_DB4_GPIO_Port GPIOD
#define LCD_DB5_Pin GPIO_PIN_5
#define LCD_DB5_GPIO_Port GPIOD
#define LCD_DB6_Pin GPIO_PIN_6
#define LCD_DB6_GPIO_Port GPIOD
#define LCD_DB7_Pin GPIO_PIN_7
#define LCD_DB7_GPIO_Port GPIOD
#define LCD_LED_Pin GPIO_PIN_5
#define LCD_LED_GPIO_Port GPIOB
#define LCD_nOE_Pin GPIO_PIN_6
#define LCD_nOE_GPIO_Port GPIOB
#define Test_Pin GPIO_PIN_7
#define Test_GPIO_Port GPIOB


#define Relay_HA_Pin GPIO_PIN_0     // Relay_HA_Pin -> SOL_KEY1
#define Relay_HA_GPIO_Port GPIOE

#define Relay_Temp_Pin  GPIO_PIN_1   // Relay_RF_Pin -> SOL_KEY2
#define Relay_Temp_GPIO_Port  GPIOE


// void   MX_GPIO_Init(void);
// void   MX_ADC1_Init(void);
// void   MX_TIM1_Init(void);
// void   MX_TIM2_Init(void);
// void   MX_TIM3_Init(void);
// void   MX_TIM6_Init(void);
// void   MX_TIM7_Init(void);
// void   MX_USART1_UART_Init(void);
// void   MX_USART3_UART_Init(void);

/* USER CODE BEGIN Private defines */
#define Uart1Tx_Pin GPIO_PIN_9
#define Uart1Rx_Pin GPIO_PIN_10
#define Uart1_Port GPIOA

//LCD
#define clr_lcd_rs() HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET)
#define lcd_rs_cmd() HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET)
#define set_lcd_rs() HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin,GPIO_PIN_SET)
#define lcd_rs_data() HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin,GPIO_PIN_SET)
#define clr_lcd_rw() HAL_GPIO_WritePin(LCD_RW_GPIO_Port, LCD_RW_Pin,GPIO_PIN_RESET)
#define set_lcd_rw() HAL_GPIO_WritePin(LCD_RW_GPIO_Port, LCD_RW_Pin,GPIO_PIN_SET)
#define set_lcd_en() HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin,GPIO_PIN_SET)
#define clr_lcd_en() HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin,GPIO_PIN_RESET)
#define set_lcd_led() HAL_GPIO_WritePin(LCD_LED_GPIO_Port, LCD_LED_Pin,GPIO_PIN_SET)
#define clr_lcd_led() HAL_GPIO_WritePin(LCD_LED_GPIO_Port, LCD_LED_Pin,GPIO_PIN_RESET)
#define set_lcd_cs1() HAL_GPIO_WritePin(LCD_CS1_GPIO_Port, LCD_CS1_Pin,GPIO_PIN_SET)
#define clr_lcd_cs1() HAL_GPIO_WritePin(LCD_CS1_GPIO_Port, LCD_CS1_Pin,GPIO_PIN_RESET)
#define set_lcd_cs2() HAL_GPIO_WritePin(LCD_CS2_GPIO_Port, LCD_CS2_Pin,GPIO_PIN_SET)
#define clr_lcd_cs2() HAL_GPIO_WritePin(LCD_CS2_GPIO_Port, LCD_CS2_Pin,GPIO_PIN_RESET)
#define set_lcd_res() HAL_GPIO_WritePin(LCD_RES_GPIO_Port, LCD_RES_Pin,GPIO_PIN_SET)
#define clr_lcd_res() HAL_GPIO_WritePin(LCD_RES_GPIO_Port, LCD_RES_Pin,GPIO_PIN_RESET)
// Interface for DC5V Lcd
#define set_lcd_noe() HAL_GPIO_WritePin(LCD_nOE_GPIO_Port, LCD_nOE_Pin,GPIO_PIN_SET)
#define clr_lcd_noe() HAL_GPIO_WritePin(LCD_nOE_GPIO_Port, LCD_nOE_Pin,GPIO_PIN_RESET)
#define set_lcd_dir() HAL_GPIO_WritePin(LCD_DIR_GPIO_Port, LCD_DIR_Pin,GPIO_PIN_SET)
#define clr_lcd_dir() HAL_GPIO_WritePin(LCD_DIR_GPIO_Port, LCD_DIR_Pin,GPIO_PIN_RESET)

#define set_485_tx() HAL_GPIO_WritePin(RS485_de_GPIO_Port, RS485_de_Pin,GPIO_PIN_SET)
#define clr_485_rx() HAL_GPIO_WritePin(RS485_de_GPIO_Port, RS485_de_Pin,GPIO_PIN_RESET)

// Relay AND SOL

// PBA FIX
#define set_fix_pba() HAL_GPIO_WritePin(Sol_FixPbaOn_GPIO_Port, Sol_FixPbaOn_Pin,GPIO_PIN_SET)
#define clr_fix_pba() HAL_GPIO_WritePin(Sol_FixPbaOn_GPIO_Port, Sol_FixPbaOn_Pin,GPIO_PIN_RESET)

// SOL_TIRGGER
#define set_trigger() HAL_GPIO_WritePin(Sol_TriggerOn_GPIO_Port, Sol_TriggerOn_Pin,GPIO_PIN_SET)
#define clr_trigger() HAL_GPIO_WritePin(Sol_TriggerOn_GPIO_Port, Sol_TriggerOn_Pin,GPIO_PIN_RESET)

// POWER 9V EMG
#define set_vemg_target() HAL_GPIO_WritePin(En_VEMG_GPIO_Port, En_VEMG_Pin,GPIO_PIN_SET)
#define clr_vemg_target() HAL_GPIO_WritePin(En_VEMG_GPIO_Port, En_VEMG_Pin,GPIO_PIN_RESET)

// POWER 6V
#define set_vbat_target() HAL_GPIO_WritePin(En_VBAT_GPIO_Port, En_VBAT_Pin,GPIO_PIN_SET)
#define clr_vbat_target() HAL_GPIO_WritePin(En_VBAT_GPIO_Port, En_VBAT_Pin,GPIO_PIN_RESET)

// POWER 4.5V
#define set_low_battery() HAL_GPIO_WritePin(Change_LowVol_GPIO_Port, Change_LowVol_Pin,GPIO_PIN_SET)
#define clr_low_battery() HAL_GPIO_WritePin(Change_LowVol_GPIO_Port, Change_LowVol_Pin,GPIO_PIN_RESET)

// OPEN SWITCH
#define set_sw_ansim1() HAL_GPIO_WritePin(Sol_OpClSw_GPIO_Port, Sol_OpClSw_Pin,GPIO_PIN_SET)
#define clr_sw_ansim1() HAL_GPIO_WritePin(Sol_OpClSw_GPIO_Port, Sol_OpClSw_Pin,GPIO_PIN_RESET)

// RESET SWITCH
#define set_sw_ansim2() HAL_GPIO_WritePin(Sol_OpClSw_GPIO_Port2, Sol_OpClSw_Pin2,GPIO_PIN_SET)
#define clr_sw_ansim2() HAL_GPIO_WritePin(Sol_OpClSw_GPIO_Port2, Sol_OpClSw_Pin2,GPIO_PIN_RESET)

// SET SWITCH
#define set_reg_sw() HAL_GPIO_WritePin(Sol_SetSw_GPIO_Port, Sol_SetSw_Pin,GPIO_PIN_SET)
#define clr_reg_sw() HAL_GPIO_WritePin(Sol_SetSw_GPIO_Port, Sol_SetSw_Pin,GPIO_PIN_RESET)

// SOL_KEY1
#define set_home_auto() HAL_GPIO_WritePin(Relay_HA_GPIO_Port, Relay_HA_Pin,GPIO_PIN_SET)
#define clr_home_auto() HAL_GPIO_WritePin(Relay_HA_GPIO_Port, Relay_HA_Pin,GPIO_PIN_RESET)

// SOL_KEY2
#define set_fire_sensor() HAL_GPIO_WritePin(Relay_Temp_GPIO_Port, Relay_Temp_Pin,GPIO_PIN_SET)
#define clr_fire_sensor() HAL_GPIO_WritePin(Relay_Temp_GPIO_Port, Relay_Temp_Pin,GPIO_PIN_RESET)

// SOL_KEY3
#define set_wifi() HAL_GPIO_WritePin(Relay_Wifi_GPIO_Port, Relay_Wifi_Pin,GPIO_PIN_SET)
#define clr_wifi() HAL_GPIO_WritePin(Relay_Wifi_GPIO_Port, Relay_Wifi_Pin,GPIO_PIN_RESET)

//#define Relay_Wifi_Pin GPIO_PIN_2
//#define Relay_Wifi_GPIO_Port GPIOE

// SOL_KEY4
#define set_rf_pin() HAL_GPIO_WritePin(Relay_RF_GPIO_Port, Relay_RF_Pin,GPIO_PIN_SET)
#define clr_rf_pin() HAL_GPIO_WritePin(Relay_RF_GPIO_Port, Relay_RF_Pin,GPIO_PIN_RESET)

// SOL_KEY5
#define set_handle_sw() HAL_GPIO_WritePin(Sol_HandleSw_GPIO_Port, Sol_HandleSw_Pin,GPIO_PIN_SET)
#define clr_handle_sw() HAL_GPIO_WritePin(Sol_HandleSw_GPIO_Port, Sol_HandleSw_Pin,GPIO_PIN_RESET)

// SOL_KEY6
#define set_KEY6() HAL_GPIO_WritePin(Sol_TriggerOff_GPIO_Port, Sol_TriggerOff_Pin,GPIO_PIN_SET)
#define clr_KEY6() HAL_GPIO_WritePin(Sol_TriggerOff_GPIO_Port, Sol_TriggerOff_Pin,GPIO_PIN_RESET)

// SOL_KEY7
#define set_KEY7() HAL_GPIO_WritePin(SpareOut1_GPIO_Port, SpareOut1_Pin,GPIO_PIN_SET)
#define clr_KEY7() HAL_GPIO_WritePin(SpareOut1_GPIO_Port, SpareOut1_Pin,GPIO_PIN_RESET)

// SOL_KEY8
#define set_KEY8() HAL_GPIO_WritePin(SpareOut2_GPIO_Port, SpareOut2_Pin,GPIO_PIN_SET)
#define clr_KEY8() HAL_GPIO_WritePin(SpareOut2_GPIO_Port, SpareOut2_Pin,GPIO_PIN_RESET)

// SOL_KEY9
#define set_KEY9() HAL_GPIO_WritePin(SpareOut3_GPIO_Port, SpareOut3_Pin,GPIO_PIN_SET)
#define clr_KEY9() HAL_GPIO_WritePin(SpareOut3_GPIO_Port, SpareOut3_Pin,GPIO_PIN_RESET)

// SOL_KEY0
#define set_KEY0() HAL_GPIO_WritePin(SpareOut4_GPIO_Port, SpareOut4_Pin,GPIO_PIN_SET)
#define clr_KEY0() HAL_GPIO_WritePin(SpareOut4_GPIO_Port, SpareOut4_Pin,GPIO_PIN_RESET)

// SOL_KEYSTAR
#define set_KEYSTAR() HAL_GPIO_WritePin(SpareOut5_GPIO_Port, SpareOut5_Pin,GPIO_PIN_SET)
#define clr_KEYSTAR() HAL_GPIO_WritePin(SpareOut5_GPIO_Port, SpareOut5_Pin,GPIO_PIN_RESET)

// SOL_KEYNUM
#define set_KEYNUM() HAL_GPIO_WritePin(SpareOut6_GPIO_Port, SpareOut6_Pin,GPIO_PIN_SET)
#define clr_KEYNUM() HAL_GPIO_WritePin(SpareOut6_GPIO_Port, SpareOut6_Pin,GPIO_PIN_RESET)

// SOL_CARD
#define set_KEYCARD() HAL_GPIO_WritePin(SpareOut7_GPIO_Port, SpareOut7_Pin,GPIO_PIN_SET)
#define clr_KEYCARD() HAL_GPIO_WritePin(SpareOut7_GPIO_Port, SpareOut7_Pin,GPIO_PIN_RESET)

// SOL_PROSTART
#define set_PRO() HAL_GPIO_WritePin(SOL_PROSTART_GPIO_Port, SOL_PROSTART_Pin,GPIO_PIN_SET)
#define clr_PRO() HAL_GPIO_WritePin(SOL_PROSTART_GPIO_Port, SOL_PROSTART_Pin,GPIO_PIN_RESET)

// 대기 전류
#define set_sleep_pin() HAL_GPIO_WritePin(RELAY_SPARE_OUT3_GPIO_Port, RELAY_SPARE_OUT3_Pin,GPIO_PIN_SET)
#define clr_sleep_pin() HAL_GPIO_WritePin(RELAY_SPARE_OUT3_GPIO_Port, RELAY_SPARE_OUT3_Pin,GPIO_PIN_RESET)

// SOL 대체용 버튼 접점 TEST (릴레이 B 접점을 이용)
#define set_B1point_pin() HAL_GPIO_WritePin(RELAY_SPARE_OUT1_GPIO_Port, RELAY_SPARE_OUT1_Pin,GPIO_PIN_SET)  // 등록 버튼 RELAY_K4 B접점
#define clr_B1point_pin() HAL_GPIO_WritePin(RELAY_SPARE_OUT1_GPIO_Port, RELAY_SPARE_OUT1_Pin,GPIO_PIN_RESET)
#define set_B2point_pin() HAL_GPIO_WritePin(RELAY_SPARE_OUT2_GPIO_Port, RELAY_SPARE_OUT2_Pin,GPIO_PIN_SET)  // open 버튼 RELAY_K5 B접점
#define clr_B2point_pin() HAL_GPIO_WritePin(RELAY_SPARE_OUT2_GPIO_Port, RELAY_SPARE_OUT2_Pin,GPIO_PIN_RESET)
#define set_B3point_pin() HAL_GPIO_WritePin(RELAY_SPARE_OUT4_GPIO_Port, RELAY_SPARE_OUT4_Pin,GPIO_PIN_SET)  // 리셋 버튼 RELAY_K6 B접점
#define clr_B3point_pin() HAL_GPIO_WritePin(RELAY_SPARE_OUT4_GPIO_Port, RELAY_SPARE_OUT4_Pin,GPIO_PIN_RESET)


// Input
#define get_state_deadbolt() (State_Deadbolt_GPIO_Port->IDR & State_Deadbolt_Pin)

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
