/**
  ******************************************************************************
  * @file           : function.c
  * @brief          : application function
  ******************************************************************************
  * @attention
  *
  * 
  *
  ******************************************************************************
*/
/* USER CODE Header */
#include <string.h>
#include "main.h"
#include "cmsis_os.h"
#include "my_generic.h"
#include "define_consts.h"
#include "taskDeviceh.h"
#include "dataform.h"
/* USER CODE END Includes */

/* Define variables */
typedef StaticSemaphore_t osStaticSemaphoreDef_t;
/* Definitions for myCountingSem01 */
osSemaphoreId_t LcdTaskSemCntHandle;
osStaticSemaphoreDef_t LcdTaskSemCntControlBlock;
const osSemaphoreAttr_t LcdTaskSemCnt_attributes = {
  .name = "LcdTaskSemCnt",
  .cb_mem = &LcdTaskSemCntControlBlock,
  .cb_size = sizeof(LcdTaskSemCntControlBlock),
};

uint8_t gu8JobStep, gu8TaskLcd, gu8JobCheckNum, gu8JobCheckStep;
uint8_t gu8LcdDisplayData[2][64][8];
uint8_t gu8TaskGraphicLcdX,gu8TaskGraphicLcdY;
uint8_t gu8GLcdXStart, gu8GLcdYStart, gu8GLcdXEnd, gu8GLcdYEnd;
// uint8_t gu8TmpStep1;
uint16_t gu16TimerJob, gu16TimerLcd;
uint16_t gu16JobBuff[SIZE_OF_gu16JobBuff];
uint16_t gu16JobTmp1, gu16JobTmp2, gu16JobTmpStep1;
uint32_t gu32Tmp;
int16_t gi16TmpData;

extern UART_HandleTypeDef huart1;

extern WORD_VAL gu16Flag1; 
extern uint8_t gu8Uart1Rxbuff[MAX_RX_DATA],gu8Uart1Txbuff[15],gu8UartRxMax,gu8Uart1RxAddr, gu8Uart1RxFunc;
extern uint8_t gu8RxData[MAX_RX_DATA];
extern uint8_t gu8Uart1RxCnt,gu8Uart1TxCnt,gu8Uart1TxCntMax,gu8TaskUart1Tx;
extern WORD_VAL gu16IoBits;
extern void MX_GPIO_Init(void);

uint16_t WriteDataOnLcd(uint8_t u8GLcdXStart,uint8_t u8GLcdXSize,uint8_t u8GLcdYStart,uint8_t u8GLcdYSize,const unsigned char* upConstAdd);
extern uint16_t CheckKeyLed(void);
extern uint16_t GetKeyLedValue(void);


/* End Define variables */
/*
CRC
*/
#define POLYNORMIAL 0xA001
uint16_t CalCRC16(uint8_t *upchMsg, uint8_t u8DataLen)
{
  uint8_t i,t;
  uint16_t crc, flag;
  crc = 0xFFFF;
  t = u8DataLen;
  while(t--)
  {
    crc ^= *upchMsg++;
    for (i=0; i<8; i++)
    {
      flag = crc & 0x0001;
      crc >>= 1;
      if(flag) crc ^= POLYNORMIAL;
    }
  }
  return crc;
}
/*-----------------------------------------------------
  * @brief  send Uart1 Tx : Get Current data
  * @param  None
  * @retval None
-----------------------------------------------------*/
void GetCurentData(void)
{
 
  uint16_t u16CalCrc;
  set_485_tx();
  osDelay(2);
  
  gu8Uart1Txbuff[0] = 0x01; // slave address
  gu8Uart1Txbuff[1] = 0x04; // function
  gu8Uart1Txbuff[2] = 0x00; // Start Address
  gu8Uart1Txbuff[3] = 0x00; // Start Address
  gu8Uart1Txbuff[4] = 0x00; // Point number
  gu8Uart1Txbuff[5] = 0x04; // Point number
  // gu8Uart1Txbuff[6] = 0xc9; // CRC16
  // gu8Uart1Txbuff[7] = 0xf1; // CRC16

  u16CalCrc = CalCRC16(gu8Uart1Txbuff,6);
  gu8Uart1Txbuff[6] = (uint8_t)u16CalCrc; // CRC16
  gu8Uart1Txbuff[7] = (uint8_t)(u16CalCrc >> 8); // CRC16

  gu8Uart1RxAddr = gu8Uart1Txbuff[0];
  gu8Uart1RxFunc = gu8Uart1Txbuff[1] ;
  gu8Uart1TxCnt=0;
  gu8Uart1TxCntMax=8;
  FLAG_UART_TX1 = 1;
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_TXE);

  // 
  osThreadFlagsWait( EVENT_FLAG_UART1_485 , osFlagsWaitAny, DEFAULT_OS_WAIT_TIME);// osWaitForever
  osThreadFlagsClear(EVENT_FLAG_UART1_485);
  
  osDelay(2);
  FLAG_UART_RX1 = 0;
  clr_485_rx();
}
/*-----------------------------------------------------
  * @brief  Check Data From  Uart1 Rx : Get Current data
  * @param  None
  * @retval None
-----------------------------------------------------*/
void CheckRxData(void){
  uint16_t u16CalCrc, u16RxData;
  uint8_t u8Cnt;
  
  u16CalCrc = CalCRC16(gu8RxData,(gu8UartRxMax - 1));

  u16RxData = (uint16_t)gu8RxData[gu8UartRxMax]; // CRC16
  u16RxData <<= 8;
  u16RxData &= 0xff00;
  u16RxData |= (uint16_t)(gu8RxData[gu8UartRxMax - 1]); // CRC16

  if(u16CalCrc != u16RxData){
    gi16TmpData = 0xffff;
    return;
  }
  gi16TmpData = (int16_t)gu8RxData[3];
  gi16TmpData <<= 8;
  gi16TmpData &= 0xff00;
  gi16TmpData |= (int16_t)gu8RxData[4];
  for(u8Cnt = gu8RxData[6] ; u8Cnt < 3 ; u8Cnt++){
    gi16TmpData *= 10;
  }
}


/*-----------------------------------------------------
  * @brief  CheckFistSetting()   
  * @param  초기 근접센서(데드볼트) 상태 검사
  * @retval None
  * @retval if(gu16TimerJob < 300){ // (gu16TimerJob 가 300보다 보다 작으면 break;로 감, 아니면 다음 단계 ( ++gu8JobCheckStep;  gu16TimerJob = 0; )로 감.
  * @retval 동작 전원: set_low_battery();
-----------------------------------------------------*/
void CheckFistSetting(void)
{
  switch(gu8JobCheckStep)
  {
    case 1:                           // 스탭1 = led 점등된게 있는지 확인
    {      
      if(gu16TimerJob > 200){         // Wait for cylinder, 기본 10ms 타이머, 10초 초과해도 반응없으면 에러처리, 기다리는 항목 아님.
         gu8JobCheckStep = 0xff;      // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 실행되어 에러 발생, 검사 스탭 표시
         break;
       }
       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
    }

    case 2:                            // 근접센서(데드볼트) 상태 검사
    {
      if(gu16TimerJob > 200){
    	 gu8JobCheckStep = 0xff;
    	 break;
      }

      if(get_state_deadbolt())     // 만약 데드볼트가 open 상태면 다음 step
      {
       ++gu8JobCheckStep;
    	 gu16TimerJob = 0;
    	 break;
      }
      else                         // 데드볼트가 close 상태면 다음 항목으로
      {
         clr_low_battery();          // 4.5V 전원 off
    	 gu8JobCheckStep = 0;
    	 gu16TimerJob = 0;
    	 break;
      }
    }
    
    case 3:                            // 데드볼트가 OPEN 4.5V 전원 공급
    {
      if(gu16TimerJob > 500){
         gu8JobCheckStep = 0xff;       
         break;
       }

         set_low_battery();                // 4.5V 전원 공급
      if(gu16TimerJob < 500){           // 100ms초 기다림
         break;
      }
      
       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
    }

    case 4:                             // OPEN 버튼 동작
    {
      if(gu16TimerJob > 200){
         gu8JobCheckStep = 0xff;        
         break;
       }
  
         set_sw_ansim1();                  // OPEN 버튼 동작
      if(gu16TimerJob < 200){           // 100ms초 기다림
         break;
      }
      
       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
    }

    case 5:                             // OPEN 버튼 해제
    {
      if(gu16TimerJob > 100){
         gu8JobCheckStep = 0xff;        
         break;
       }
  
         clr_sw_ansim1();                  // OPEN 버튼 해제
      if(gu16TimerJob < 100){            // 100ms초 기다림
         break;
      }
      
       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
    }

    case 6:                             // 근접센서(데드볼트) 상태 검사
    {
      if(gu16TimerJob > 100){           
         gu8JobCheckStep = 0xff;       
         break;
       }
        
      if(get_state_deadbolt())       // 만약 데드볼트가 open 상태이면 error 종료
      {
    	 gu8JobCheckStep = 0xff;     // 검사 스탭 표시하면서 Error
    	 break;
      }
      else
      {                              // 데드볼트가 close 상태 정상 종료
    	 clr_low_battery();          // 4.5V 전원 off
    	 gu8JobCheckStep = 0;
    	 gu16TimerJob = 0;
         break;
      }
    }

    default:
    {
      if(gu8JobCheckStep){
         gu8JobCheckStep = 0xff;
      }      
      break;
    }
  }
}

/*-----------------------------------------------------
  * @brief 공장 초기화
  * @param  None
  * @retval None
-----------------------------------------------------*/
void CheckFactoryReset(void)
{
  switch(gu8JobCheckStep)
  {
    case 1:                           
    {
      if(gu16TimerJob > 100){         // Wait for cylinder, 기본 10ms 타이머, 10초 초과해도 반응없으면 에러처리, 기다리는 항목 아님.
         gu8JobCheckStep = 0xff;      // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 실행되어 에러 발생, 검사 스탭 표시
         break;
       }
        
       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
    }

    case 2:                           // open 버튼 동작
    {
      if(gu16TimerJob > 200){           
         gu8JobCheckStep = 0xff;        
         break;
       }
  
         set_sw_ansim1();
      if(gu16TimerJob < 150){         // 100ms초 기다림
         break;
      }
      
       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break; 
    }

    case 3:                             // 4.5V 전원 on
    {
      if(gu16TimerJob > 200){
         gu8JobCheckStep = 0xff;      
         break;
       }
        
         set_low_battery();
      if(gu16TimerJob < 100){         // 100ms초 기다림
         break;
       }

         gu16JobTmpStep1 = CheckKeyLed();   // LED 값 읽어오기
         gu16JobTmpStep1 &= KEY_LED_ALL;    // 점등된 LED 값과 비교하기
      if(gu16JobTmpStep1 & (KEY_LED_RD))
      //if(gu16JobTmpStep1 & (KEY_LED_K4))   // 전체 LED가 점등되는지 확인
       {
       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
       }
      else
       {
    	 clr_low_battery();
    	 gu8JobCheckStep = 0xff;
    	 break;
       }

         break;
    }

    case 4:                             // 등록 버튼 해제
    {
      if(gu16TimerJob > 200){
    	 gu8JobCheckStep = 0xff;
    	 break;
   	   }

         clr_sw_ansim1();                // 등록 버튼 해제
      if(gu16TimerJob < 100){           // 100ms초 기다림
         break;
        }
         clr_low_battery();
         gu8JobCheckStep = 0;
         gu16TimerJob = 0;
         break;
    }

   default:
    {
      if(gu8JobCheckStep){
         gu8JobCheckStep = 0xff;
         break;
      }      
         break;
    }
  }
}


/*-----------------------------------------------------
  * @brief  CARD 인식 검사
  * @param  None
  * @retval None
  * @retval test: 저전압을 공급함으로써 저전압 인식 기능까지 동시 검사 진행
  * @retval 동작 전원: set_vbat_battery();
-----------------------------------------------------*/
void CheckCard(void)
{
	switch(gu8JobCheckStep)
	 {
     case 1:                               
     {                            
        if(gu16TimerJob > 100){            // 10초 초과해도 반응없으면 에러처리, 기다리는 석 아님.
           gu8JobCheckStep = 0xff;         // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 발생, 검사 스탭 표시
           break;
        }

         ++gu8JobCheckStep;
           gu16TimerJob = 0;
           break;
     }

     case 2:                              // 4.5V 전원 공급
     {
        if(gu16TimerJob > 500){
           gu8JobCheckStep = 0xff;        
           break;
         }

           set_low_battery();                // 4.5V 전원 공급
        if(gu16TimerJob < 400){             // 100ms초 기다림
           break;
         }
      
         ++gu8JobCheckStep;
           gu16TimerJob = 0;
           break;
       } 

      case 3:                               // 첫번째 카드 동작
       {
        if(gu16TimerJob > 200){
           gu8JobCheckStep = 0xff;
           break;
         }

           set_KEYCARD();

           gu16JobTmpStep1 = CheckKeyLed();     // led 값 읽어오기
           //gu16JobTmpStep1 &= KEY_LED_ALL;
        if(gu16JobTmpStep1 & (KEY_LED_RD)){      // 저전압 led 값이 있는지 확인, 있다면 종료,
         ++gu8JobCheckStep;
           gu16TimerJob = 0;
         }
           break;
       }

      case 4:
       {
        if(gu16TimerJob > 200){
           gu8JobCheckStep = 0xff;
           break;
         }

           clr_KEYCARD();
        if(gu16TimerJob < 200){
           break;
         }

           gu8JobCheckStep = 0;
           gu16TimerJob = 0;
           break;
       }

     default:
       {
        if(gu8JobCheckStep){
           gu8JobCheckStep = 0xff;
         }
           break;
       }
    }
}

/*-----------------------------------------------------
  * @brief  Password Open
  * @param  None
  * @retval None
-----------------------------------------------------*/
void CheckPasswordOpen(void)
{
  switch(gu8JobCheckStep)
  {
    case 1:
    {
      if(gu16TimerJob > 100){         // Wait for cylinder, 기본 10ms 타이머, 10초 초과해도 반응없으면 에러처리, 기다리는 항목 아님.
         gu8JobCheckStep = 0xff;      // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 실행되어 에러 발생, 검사 스탭 표시
         break;
       }
       
       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
    }

    case 2:                             // 6V 전원 공급
    {
      if(gu16TimerJob > 300){
         gu8JobCheckStep = 0xff;        
         break;
       }

         set_vbat_target();                // 6V 전원 공급
      if(gu16TimerJob < 300){           // 200ms초 기다림
         break;
      }
      
       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
    }

    case 3:                             // 웨이크 업 동작 "5"번, "8번" 터치
    {
      if(gu16TimerJob > 100){           // Wait for cylinder, 기본 10ms 타이머, 10초 초과해도 반응없으면 에러처리, 기다리는 항목 아님.
         gu8JobCheckStep = 0xff;        // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 실행되어 에러 발생, 검사 스탭 표시
         break;
       }
  
        set_handle_sw();                  // "5" 터치 동작
        set_KEY0();                       // "0" 터치 동작
      if(gu16TimerJob < 50){           // 100ms초 기다림
         break;
      }

         gu16JobTmpStep1 = CheckKeyLed();
         gu16JobTmpStep1 &= KEY_LED_NUMALL;
      if(gu16JobTmpStep1 & (KEY_LED_NUMALL)){
       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
       }
      else{                              // LED가 미 점등된게 있을 경우
         gu8JobCheckStep = 0xff;          // 검사 중 에러가 발생했을 경우 에러 처리, 시간 초과 경우도 포함, 검사 중 에러가 발생한 스탭도 표시
         break;
      }

    }

    case 4:                             // "5"번, "8번" 터치 해제
    {
      if(gu16TimerJob > 100){           
         gu8JobCheckStep = 0xff;        
         break;
       }
       
         clr_handle_sw();                  // "5" 터치 동작 해제
         clr_KEY0();                       // "0" 터치 동작 해제
      if(gu16TimerJob < 50){           // 100ms초 기다림
         break;
      }

       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
    }

    case 5:                             // "1"번 터치
    {
      if(gu16TimerJob > 100){
    	 gu8JobCheckStep = 0xff;
    	 break;
       }

         set_home_auto();                  // "1" 터치 동작
      if(gu16TimerJob < 50){            // 50ms초 기다림
         break;
      }

       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
    }

    case 6:                             // "1" 터치 해제 or 확인, key led 중 1번 led가 점등시 ok , 미 점등시 NG
    {
      if(gu16TimerJob > 100){           // Wait for cylinder, 기본 10ms 타이머, 10초 초과해도 반응없으면 에러처리, 기다리는 항목 아님.
         gu8JobCheckStep = 0xff;        // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 실행되어 에러 발생, 검사 스탭 표시
         break;
       }
  
         clr_home_auto();                       // "1" 터치 동작 해제
      if(gu16TimerJob < 50){                 // 50ms초 기다림
         break;
      }

         gu16JobTmpStep1 = CheckKeyLed();
         gu16JobTmpStep1 &= KEY_LED_NUMALL;
      if(gu16JobTmpStep1 & (KEY_LED_K2)){
         ++gu8JobCheckStep;
           gu16TimerJob = 0;
           break;
       }
      else{                                // 미 점등된 LED가 있을 경우
         gu8JobCheckStep = 0xff;          // 에러가 발생한 스탭 표시
         break;
       }
         break;
    }
    
    case 7:                             // "2"번 터치
    {      
         set_fire_sensor();                // "2" 터치 동작
      if(gu16TimerJob < 50){            // 50ms초 기다림
         break;
      }      
       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
    }

    case 8:                             // "2" 터치 해제 or 확인, key led 중 2번 led가 점등시 ok , 미 점등시 NG
    {
      if(gu16TimerJob > 100){           // Wait for cylinder, 기본 10ms 타이머, 10초 초과해도 반응없으면 에러처리, 기다리는 항목 아님.
         gu8JobCheckStep = 0xff;        // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 실행되어 에러 발생, 검사 스탭 표시
         break;
       }
  
         clr_fire_sensor();                     // "2" 터치 동작 해제
      if(gu16TimerJob < 50){                 // 50ms초 기다림
         break;
      }

         gu16JobTmpStep1 = CheckKeyLed();
         gu16JobTmpStep1 &= KEY_LED_NUMALL;
      if(gu16JobTmpStep1 & (KEY_LED_K8)){

       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
       }
      else{                                // 미 점등된 LED가 있을 경우
          gu8JobCheckStep = 0xff;          // 에러가 발생한 스탭 표시
          break;
       }
          break;             
    }

    case 9:                             // "3"번 터치
    {
         set_wifi();                       // "3" 터치 동작
      if(gu16TimerJob < 50){            // 50ms초 기다림
         break;
      }      
       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;      
    }

    case 10:                             // "3" 터치 해제 or 확인, key led 중 3번 led가 점등시 ok , 미 점등시 NG
    {
      if(gu16TimerJob > 100){           // Wait for cylinder, 기본 10ms 타이머, 10초 초과해도 반응없으면 에러처리, 기다리는 항목 아님.
         gu8JobCheckStep = 0xff;        // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 실행되어 에러 발생, 검사 스탭 표시
         break;
       }
  
      clr_wifi();                       // "3" 터치 동작 해제      
      if(gu16TimerJob < 50){            // 50ms초 기다림
         break;
      }

         gu16JobTmpStep1 = CheckKeyLed();
         gu16JobTmpStep1 &= KEY_LED_NUMALL;
      if(gu16JobTmpStep1 & (KEY_LED_K6)){
       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
       }
      else{                                // 미 점등된 LED가 있을 경우
          gu8JobCheckStep = 0xff;          // 에러가 발생한 스탭 표시
          break;
       }
          break;             
    }

    case 11:                             // "4"번 터치
    {
         set_rf_pin();                       // "4" 터치 동작
      if(gu16TimerJob < 50){            // 50ms초 기다림
         break;
      }      
       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;      
    }

    case 12:                             // "4" 터치 해제 or 확인, key led 중 4번 led가 점등시 ok , 미 점등시 NG
    {
      if(gu16TimerJob > 100){           // Wait for cylinder, 기본 10ms 타이머, 10초 초과해도 반응없으면 에러처리, 기다리는 항목 아님.
         gu8JobCheckStep = 0xff;        // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 실행되어 에러 발생, 검사 스탭 표시
         break;
       }
  
         clr_rf_pin();                     // "4" 터치 동작 해제
      if(gu16TimerJob < 50){            // 50ms초 기다림
         break;
      }

         gu16JobTmpStep1 = CheckKeyLed();
         gu16JobTmpStep1 &= KEY_LED_NUMALL;
      if(gu16JobTmpStep1 & (KEY_LED_K5)){
       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
       }
      else{                                // 미 점등된 LED가 있을 경우
          gu8JobCheckStep = 0xff;          // 에러가 발생한 스탭 표시
          break;
       }
          break;             
    }

    case 13:                             // "*" 터치
    {
         set_KEYSTAR();                    // "*" 터치 동작
      if(gu16TimerJob < 50){            // 50ms초 기다림
         break;
      }
      
       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
    }

    case 14:
    {
      if(gu16TimerJob > 100){           // Wait for cylinder, 기본 10ms 타이머, 10초 초과해도 반응없으면 에러처리, 기다리는 항목 아님.
         gu8JobCheckStep = 0xff;        // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 실행되어 에러 발생, 검사 스탭 표시
         break;
       }
  
         clr_KEYSTAR();                        // "*" 터치 동작 해제
      if(gu16TimerJob < 50){                // 50ms초 기다림
         break;
      }

         gu16JobTmpStep1 = CheckKeyLed();        // 전체 KEY LED OFF 검사
         gu16JobTmpStep1 &= KEY_LED_NUMALL;
      if(!(gu16JobTmpStep1 & (KEY_LED_K9))){  // 전체 KEY LED가 OFF일 경우

       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
      }
      else{                                 // 점등되어 있을 경우
           gu8JobCheckStep = 0xff;          // 에러가 발생한 스탭 표시
           break;
      }

           break;

    }

    case 15:                             // 근접센서(데드볼트) 상태 검사
    {
      if(gu16TimerJob > 200){
         gu8JobCheckStep = 0xff;
         break;
       }

      if(gu16TimerJob > 200){
         break;
       }

      if(!(get_state_deadbolt())){      // 만약 데드볼트가 close 상태이면 Error 처리
      	 gu8JobCheckStep = 0xff;        // 검사 스탭 표시하면서 Error
         break;
       }
      else                              // 데드볼트가 open 상태이면 종료하고 다음 항목으로
      {
        clr_vbat_target();                // 6V 전원 off
        gu8JobCheckStep = 0;
        gu16TimerJob = 0;
        break;
      }
      break;
    }
    
    default:
    {
      if(gu8JobCheckStep){
        gu8JobCheckStep = 0xff;
      }      
      break;
    }    
  }  
}


/*-----------------------------------------------------
  * @brief  Check Trigger
  * @param  None
  * @retval None OPEN -> CLOSE 상태
-----------------------------------------------------*/
void CheckTrigger(void)
{
  switch(gu8JobCheckStep)
  {
   case 1:
    {
      if(gu16TimerJob > 100){         // Wait for cylinder, 기본 10ms 타이머, 10초 초과해도 반응없으면 에러처리, 기다리는 항목 아님.
         gu8JobCheckStep = 0xff;      // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 실행되어 에러 발생, 검사 스탭 표시
         break;
       }
       
       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;      
    }

   case 2:                             // 6V 전원 공급
    {
      if(gu16TimerJob > 300){
         gu8JobCheckStep = 0xff;        
         break;
       }

         set_vbat_target();                // 6V 전원 공급
      if(gu16TimerJob < 100){           // 100ms초 기다림
         break;
      }
      
       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
    }

   case 3:                              // 트리거 동작
    {
      if(gu16TimerJob > 500){           
         gu8JobCheckStep = 0xff;        
         break;
       }
  
         set_trigger();                     // 트리거 동작
      if(gu16TimerJob < 400){
         break;
      }

       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
    }

   case 4:                               // 트리거 동작 해제
    {
      if(gu16TimerJob > 100){           
         gu8JobCheckStep = 0xff;        
         break;
       }
      
         clr_trigger();                     // 트리거 동작 해제
      if(gu16TimerJob < 50){             // 50ms초 기다림
         break;
      }        

      ++gu8JobCheckStep;
        gu16TimerJob = 0;
        break;  
     }

   case 5:                             // 근접센서(데드볼트) 상태 검사
    {
      if(gu16TimerJob > 200){           // Wait for cylinder, 기본 10ms 타이머, 10초 초과해도 반응없으면 에러처리, 기다리는 항목 아님.
         gu8JobCheckStep = 0xff;        // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 실행되어 에러 발생, 검사 스탭 표시
         break;
       }

// 문의: 정상적으로 판정하여 else문으로 동작하나 error가 표시됨 이유는?        
      if(get_state_deadbolt()){      // 만약 데드볼트가 close 상태이면 Error 처리
      	 gu8JobCheckStep = 0xff;        // 검사 스탭 표시하면서 Error
         break;
      }
      else                              // 데드볼트가 open 상태이면 종료하고 다음 항목으로
      {  
         clr_vbat_target();                // 6V 전원 off
         gu8JobCheckStep = 0;
         gu16TimerJob = 0;                                   
      	 break;
      }
      break;

    }
    
   case 6:                             // 닫힘 문자 LED 확인 = 점등시 ok , 미 점등시 NG
    {
      if(gu16TimerJob > 100){           // Wait for cylinder, 기본 10ms 타이머, 10초 초과해도 반응없으면 에러처리, 기다리는 항목 아님.
         gu8JobCheckStep = 0xff;        // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 실행되어 에러 발생, 검사 스탭 표시
         break;
      }

         gu16JobTmpStep1 = CheckKeyLed();
      if(gu16JobTmpStep1 & (KEY_LED_K3)){

    	 gu8JobCheckStep = 0;
    	 gu16TimerJob = 0;
         break;
      }

      else{    	  // 미 점등시
         gu8JobCheckStep = 0xff;          // 에러가 발생한 스탭 표시
         break;
      }
         break;
    }

    default:
    {
      if(gu8JobCheckStep){
         gu8JobCheckStep = 0xff;
         break;
      }      

      break;
    }     
  }
} 

/*-----------------------------------------------------
  * @brief  Check open test
  * @param  None
  * @retval None close -> OPEN 상태
-----------------------------------------------------*/
void Checkopenswitch1(void)
{
  switch(gu8JobCheckStep)
  {
   case 1:
    {
      if(gu16TimerJob > 100){         // Wait for cylinder, 기본 10ms 타이머, 10초 초과해도 반응없으면 에러처리, 기다리는 항목 아님.
         gu8JobCheckStep = 0xff;      // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 실행되어 에러 발생, 검사 스탭 표시
         break;
       }

       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
    }

   case 2:                             // 6V 전원 공급
    {
      if(gu16TimerJob > 400){
         gu8JobCheckStep = 0xff;
         break;
       }

         set_vbat_target();             // 6V 전원 공급
      if(gu16TimerJob < 300){           // 100ms초 기다림
         break;
      }

       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
    }

   case 3:                              // OPEN 버튼 동작
    {
      if(gu16TimerJob > 200){
         gu8JobCheckStep = 0xff;
         break;
       }

         set_sw_ansim1();
      if(gu16TimerJob < 150){            // 100ms초 기다림
         break;
      }
       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
    }

   case 4:                               // 해제
    {
      if(gu16TimerJob > 100){
         gu8JobCheckStep = 0xff;
         break;
       }

         clr_sw_ansim1();                // 동작 해제
      if(gu16TimerJob < 50){             // 50ms초 기다림
         break;
       }

      ++gu8JobCheckStep;
        gu16TimerJob = 0;
        break;
    }

    case 5:                             // 근접센서(데드볼트) 상태 검사
    {
      if(gu16TimerJob > 300){
         gu8JobCheckStep = 0xff;
         break;
       }

      if(get_state_deadbolt()){   // 만약 데드볼트가 close 상태이면 종료
         gu8JobCheckStep = 0;
         gu16TimerJob = 0;
         break;
       }
      else                           // 데드볼트가 open 상태이면 다음 진행
       {
         gu8JobCheckStep = 0xff;     // 검사 스탭 표시하면서 Error
         break;
       }
    }

    default:
    {
      if(gu8JobCheckStep){
         gu8JobCheckStep = 0xff;
      }
         break;
    }
  }
}

/*-----------------------------------------------------
  * @brief  Check close test
  * @param  None
  * @retval None open -> close 상태
-----------------------------------------------------*/
void Checkopenswitch2(void)
{
  switch(gu8JobCheckStep)
  {
   case 1:
    {
      if(gu16TimerJob > 100){         // Wait for cylinder, 기본 10ms 타이머, 10초 초과해도 반응없으면 에러처리, 기다리는 항목 아님.
         gu8JobCheckStep = 0xff;      // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 실행되어 에러 발생, 검사 스탭 표시
         break;
       }

       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
    }

   case 2:                             // 6V 전원 공급
    {
      if(gu16TimerJob > 400){
         gu8JobCheckStep = 0xff;
         break;
       }

         set_vbat_target();             // 6V 전원 공급
      if(gu16TimerJob < 300){           // 100ms초 기다림
         break;
       }

       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
    }

   case 3:                              // OPEN 버튼 동작
    {
      if(gu16TimerJob > 200){
         gu8JobCheckStep = 0xff;
         break;
       }

         set_sw_ansim1();
      if(gu16TimerJob < 150){            // 100ms초 기다림
         break;
      }
       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
    }

   case 4:                               // 해제
    {
      if(gu16TimerJob > 100){
         gu8JobCheckStep = 0xff;
         break;
       }

         clr_sw_ansim1();                // 동작 해제
      if(gu16TimerJob < 50){             // 50ms초 기다림
         break;
       }

      ++gu8JobCheckStep;
        gu16TimerJob = 0;
        break;
    }

   case 5:                             // 근접센서(데드볼트) 상태 검사
    {
      if(gu16TimerJob > 300){          // Wait for cylinder, 기본 10ms 타이머, 10초 초과해도 반응없으면 에러처리, 기다리는 항목 아님.
         gu8JobCheckStep = 0xff;       // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 실행되어 에러 발생, 검사 스탭 표시
         break;
       }

      if(get_state_deadbolt()){        // 만약 데드볼트가 open 상태이면 Error 처리
         gu8JobCheckStep = 0xff;       // 검사 스탭 표시하면서 Error
         break;
       }
      else                             // 데드볼트가 open 상태이면 다음 진행
       {
       ++gu8JobCheckStep;
         gu16TimerJob = 0;
    	 break;
       }
    }

   case 6:
    {
      if(gu16TimerJob > 300){           //
         gu8JobCheckStep = 0xff;        //
         break;
       }

         gu16JobTmpStep1 = CheckKeyLed();
      if(gu16JobTmpStep1 & (KEY_LED_K3)){
         gu8JobCheckStep = 0;
    	 gu16TimerJob = 0;
    	 break;
       }
      else                                // 데드볼트가 open 상태이면 다음 진행
       {
    	 gu8JobCheckStep = 0xff;          // 검사 스탭 표시하면서 Error
      	 break;
       }
         break;
    }

   default:
    {
      if(gu8JobCheckStep){
         gu8JobCheckStep = 0xff;
         break;
      }

      break;
    }
  }
}

/*-----------------------------------------------------
  * @brief  Check Sleep Current
  * @param  None
  * @retval None 문의: 평균 값 측정하는 방법
-----------------------------------------------------*/

void CheckSleep(void)
{
  uint32_t event;
  switch(gu8JobCheckStep)
  {
    case 1:
    {
      ++gu8JobCheckStep;
        gu16TimerJob = 0;
        break;
    }
    
    case 2:
    {      
      if(gu16TimerJob < 300){
        break;
      }

        set_vbat_target();                // 6V 전원 on
      ++gu8JobCheckStep;
        gu16TimerJob = 0;
        break;
    }

    case 3:
    {      
      if(gu16TimerJob < 600){
        break;
      }

        set_sleep_pin();
        gu16JobTmp2 = 0;
      ++gu8JobCheckStep;
        gu16TimerJob = 0;
        break;
    }

    case 4:
    {
      if(gu16TimerJob < 300){
         break;
      }

         gu16JobTmp2 = 0;
         gu32Tmp = 0;
       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
    }

    case 5:
    {
      // if(gu16TimerJob < 2){
      //   break;
      // }
      set_sleep_pin();
      GetCurentData();
      event = osThreadFlagsWait(EVENT_FLAG_UART1_485 , osFlagsWaitAny, DEFAULT_OS_WAIT_TIME);// osWaitForever
      CheckRxData();
      if(event == osFlagsErrorTimeout){  
         gi16TmpData = 0xffff;
      }
      if(gi16TmpData == 0xffff){
        gu8JobCheckStep = 0xff;
      }
      else{
        //++gu8JobCheckStep;
        if(++gu16JobTmp2 < 100){
             gu32Tmp += (uint32_t)gi16TmpData;
        }
        else{
           gu16JobTmp2 = 100;
        }
        if(gu16TimerJob > 150){

           gu32Tmp /= (uint32_t)gu16JobTmp2;
           gi16TmpData = (int16_t)gu32Tmp;

// gi16TmpData 값이 -1 임?

           gu16TimerJob = 0;
         ++gu8JobCheckStep;

        }
      }
         //gu16TimerJob = 0;
           break;
    }

    case 6:
    {
// 1. 4/4 전류 측정에서 ERROR 후 메타 복구가 안됨?
      if((gi16TmpData > 80) && (gi16TmpData < 200)){
// 2. IF문에서 ELSE문으로 감?
        ++gu8JobCheckStep;
          gu16TimerJob = 0;
          break;
      }
      if(++gu16JobTmp2 < 3){
         --gu8JobCheckStep;
           gu16TimerJob = 0;
      }
      else{
        gu8JobCheckStep = 0xff;
        gu16TimerJob = 0;
      }
      break;
    }

    case 7:
    {      
      if(gi16TmpData < 0){
        gu16JobTmp1 = 0;
      }
      else if(gi16TmpData > 9999){
        gu16JobTmp1 = 9999;
      }
      else{
        gu16JobTmp1 = (uint16_t)gi16TmpData;
      }

      for(gu16JobTmpStep1 = 0; gu16JobTmpStep1 < 4 ; gu16JobTmpStep1++){
        gu16JobBuff[gu16JobTmpStep1] = gu16JobTmp1 % 10;
        gu16JobTmp1 /= 10;
      }
      
      if(!WriteDataOnLcd(POSITION_STATE_X,1,POSITION_STATE_YN,8,(const unsigned char*)&constLcdNum[gu16JobBuff[3]][0])){
          break;
        }
      if(!WriteDataOnLcd(POSITION_STATE_X,1,(POSITION_STATE_YN+8),8,(const unsigned char*)&constLcdNum[gu16JobBuff[2]][0])){
          break;
        }
      if(!WriteDataOnLcd(POSITION_STATE_X,1,(POSITION_STATE_YN+16),8,(const unsigned char*)&constLcdNum[gu16JobBuff[1]][0])){
          break;
        }
      if(!WriteDataOnLcd(POSITION_STATE_X,1,(POSITION_STATE_YN+24),8,(const unsigned char*)&constLcdNum[gu16JobBuff[0]][0])){
          break;
        }
      
          clr_sleep_pin();
      if((gi16TmpData > 80) && (gi16TmpData < 200)){
          clr_vbat_target();                // 6V 전원 off
          gu8JobCheckStep = 0;        
      }
      else{
          gu8JobCheckStep = 0xff;
      }
          gu16TimerJob = 0;
          break;
    }

    default:
    {
      if(gu8JobCheckStep){
        gu8JobCheckStep = 0xff;
      }
      
      break;
    }
  }
}

/*-----------------------------------------------------
  * @brief  Check 9V
  * @param  None
  * @retval None
-----------------------------------------------------*/
void Check9V(void)
{
  switch(gu8JobCheckStep)
   {
    case 1:                           
    {
      if(gu16TimerJob > 100){ 
         gu8JobCheckStep = 0xff; 
         break;
       }

       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
       }

    case 2:                           
    {
      if(gu16TimerJob > 500){
         gu8JobCheckStep = 0xff; 
         break;
       }

         set_vemg_target();
      if(gu16TimerJob < 400){
         break;
       }

       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
       }

    case 3:                             // 스탭2 = 등록 버튼 동작
    {
      if(gu16TimerJob > 100){
         gu8JobCheckStep = 0xff; 
         break;
       }
                              
         set_reg_sw();
      if(gu16TimerJob < 100){
         break;
       }

         gu16JobTmpStep1 = CheckKeyLed();
         gu16JobTmpStep1 &= KEY_LED_NUMALL;
      if(gu16JobTmpStep1 & (KEY_LED_KST)){
       ++gu8JobCheckStep;
       	 gu16TimerJob = 0;
         break;
       }
      else{                         // LED가 미 점등된게 있을 경우
         gu8JobCheckStep = 0xff;    // 검사 중 에러가 발생했을 경우 에러 처리, 시간 초과 경우도 포함, 검사 중 에러가 발생한 스탭도 표시
         break;
       }
         break;
    }

    case 4:                             // 스탭3 = 등록 버튼 해제
    {
      if(gu16TimerJob > 100){ 
         gu8JobCheckStep = 0xff; 
         break;
       }         
                                
         clr_reg_sw();                    // 500ms 동안 반복함.
      if(gu16TimerJob < 50){          // (gu16TimerJob 가 50보다 보다 작으면 break;로 감. 아니면 다음 단계 ( ++gu8JobCheckStep;  gu16TimerJob = 0; )로 감.
         break;
       }

       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
    }

    case 5:
    {
      if(gu16TimerJob > 200){
         gu8JobCheckStep = 0xff; 
         break;
       }
                                           
         set_sw_ansim2();
      if(gu16TimerJob < 50){
         break;
       }

       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;

    }

    case 6:                        // 리셋 해제
    {
      if(gu16TimerJob > 100){
         gu8JobCheckStep = 0xff;
         break;
      }

         clr_sw_ansim2();
      if(gu16TimerJob < 50){
         break;
      }

       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
    }

    case 7:
    {
      if(gu16TimerJob > 200){
         gu8JobCheckStep = 0xff;
         break;
       }

      if(gu16TimerJob < 150){
         break;
       }

         gu16JobTmpStep1 = CheckKeyLed();
         gu16JobTmpStep1 &= KEY_LED_NUMALL;
      if(gu16JobTmpStep1 & (KEY_LED_KK)){
         gu8JobCheckStep = 0xff;
      	 break;
       }
      else{                       // LED가 미 점등된게 있을 경우
         gu8JobCheckStep = 0;
       	 gu16TimerJob = 0;
         break;
       }
         break;
    }

    default:
    {
      if(gu8JobCheckStep){
         gu8JobCheckStep = 0xff;
         break;
      }
         break;
    }
  }
}

/*-----------------------------------------------------
  * @brief  Check Low Battery
  * @param  None
  * @retval None
  * @retval 동작 전원: set_low_battery();
-----------------------------------------------------*/
void CheckLowBattery(void)
{
  switch(gu8JobCheckStep)
  {
    case 1:                              // 실행 후 동작 시간 감시
    {
      if(gu16TimerJob > 100){            // Wait for cylinder, 기본 10ms 타이머, 10초 초과해도 반응없으면 에러처리, 기다리는 항목 아님.
         gu8JobCheckStep = 0xff;         // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 실행되어 에러 발생, 검사 스탭 표시
         break;
       }

       ++gu8JobCheckStep;
       gu16TimerJob = 0;
       break;
    }

    case 2:                               // 4.5V 전원 공급
    {
      if(gu16TimerJob > 500){
         gu8JobCheckStep = 0xff;
         break;
       }

      set_low_battery();                // 4.5V 전원 공급
      if(gu16TimerJob < 500){           // 100ms초 기다림
         break;
      }

       ++gu8JobCheckStep;
       gu16TimerJob = 0;
       break;
    }

    case 3:                             // open 버튼 동작(저 전압 확인)
    {
      if(gu16TimerJob > 200){
         gu8JobCheckStep = 0xff;
         break;
       }

      set_sw_ansim1();                  // OPEN 버튼 동작
      if(gu16TimerJob < 100){           // 100ms초 기다림
         break;
      }

       ++gu8JobCheckStep;
       gu16TimerJob = 0;
       break;
    }

    case 4:                             // open 버튼 동작 해제
    {
      if(gu16TimerJob > 100){
         gu8JobCheckStep = 0xff;
         break;
       }

      clr_sw_ansim1();                  // OPEN 버튼 해제
      if(gu16TimerJob < 100){           // 350ms초 기다림
         break;
      }

       ++gu8JobCheckStep;
       gu16TimerJob = 0;
       break;
    }

    case 5:                               // low led 점등 여부 확인 => 점등시: 양 / 미점등시: 불
    {
      if(gu16TimerJob > 200){
         gu8JobCheckStep = 0xff;
         break;
       }

// 문의 = ADC 값을 리드한 값을 확인하는 방법?
// if(gu16JobTmpStep1 == (KEY_LED_4)){          // gu32Adc1Data[4] = LOW LED
         gu16JobTmpStep1 = CheckKeyLed();       // low led(KEY_LED_4)
      if(gu16JobTmpStep1 & (KEY_LED_RD)){

         ++gu8JobCheckStep;
           gu16TimerJob = 0;
           break;
       }
      else{                                     // LED가 미 점등 됐을 경우
          gu8JobCheckStep = 0xff;               // 검사 중 에러가 발생했을 경우 에러 처리, 시간 초과 경우도 포함, 검사 중 에러가 발생한 스탭도 표시
      }
          break;
    }

    case 6:                             // 근접센서(데드볼트) 상태 검사
    {
      if(gu16TimerJob > 100){
         gu8JobCheckStep = 0xff;
         break;
       }

      if(get_state_deadbolt()){         // 만약 데드볼트가 open이면 Error 표시하고 종료
         gu8JobCheckStep = 0xff;        // 검사 스탭 표시하면서 Error
       }
      else                              // 데드볼트가 close 상태이면 다음 항목 이동
       {
        clr_low_battery();              // 4.5V 전원 OFF
    	gu8JobCheckStep = 0;
        gu16TimerJob = 0;
        break;
       }
    }

    default:
    {
      if(gu8JobCheckStep){
         gu8JobCheckStep = 0xff;
      }
         break;
    }
  }
}


/*-----------------------------------------------------
  * @brief  PASSWORD TEST
  * @param  None
  * @retval None
-----------------------------------------------------*/
void CheckPassWord(void)
{
  switch(gu8JobCheckStep)
  {
    case 1:
    {
      if(gu16TimerJob > 100){         // Wait for cylinder, 기본 10ms 타이머, 10초 초과해도 반응없으면 에러처리, 기다리는 항목 아님.
         gu8JobCheckStep = 0xff;      // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 실행되어 에러 발생, 검사 스탭 표시
         break;
       }
       
       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;      
    }

    case 2:                             // 6V 전원 공급
    {
      if(gu16TimerJob > 300){
         gu8JobCheckStep = 0xff;        
         break;
       }

      set_vbat_target();                // 6V 전원 공급
      if(gu16TimerJob < 200){           // 100ms초 기다림
         break;
      }
      
       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
    }

    case 3:                             // "1"번 터치
    {
      if(gu16TimerJob > 100){           // Wait for cylinder, 기본 10ms 타이머, 10초 초과해도 반응없으면 에러처리, 기다리는 항목 아님.
         gu8JobCheckStep = 0xff;        // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 실행되어 에러 발생, 검사 스탭 표시
         break;
       }
  
      set_home_auto();                  // "1" 터치 동작
      if(gu16TimerJob < 50){            // 50ms초 기다림
         break;
      }  

       ++gu8JobCheckStep;
       gu16TimerJob = 0;
       break;      
    }

    case 4:                                 // "1" 터치 해제 or 확인 = key led 중 1번 led가 점등시 ok , 미 점등시 NG
    {
      if(gu16TimerJob > 100){           
         gu8JobCheckStep = 0xff;        
         break;
       }
  
      clr_home_auto();                       // "1" 터치 동작 해제      
      if(gu16TimerJob < 50){                 // 50ms초 기다림
         break;
      }

      gu16JobTmpStep1 = CheckKeyLed();        // 1번 LED(KEY_LED_K3)
      if(gu16JobTmpStep1 & (KEY_LED_K3)){
         ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
       }
      else{                                // 미 점등된 LED가 있을 경우
         gu8JobCheckStep = 0xff;           // 에러가 발생한 스탭 표시
       }
         break;             
    }
    
    case 5:                             // "2"번 터치
    {
      if(gu16TimerJob > 100){           
         gu8JobCheckStep = 0xff;        
         break;
       }
  
      set_fire_sensor();                // "2" 터치 동작
      if(gu16TimerJob < 50){            // 50ms초 기다림
         break;
      }      
       ++gu8JobCheckStep;
       gu16TimerJob = 0;
       break;      
    }

    case 6:                                  // "2" 터치 해제 or 확인= key led 중 2번 led가 점등시 ok , 미 점등시 NG
    {
      if(gu16TimerJob > 100){           
         gu8JobCheckStep = 0xff;        
         break;
       }
  
      clr_fire_sensor();                     // "2" 터치 동작 해제      
      if(gu16TimerJob < 50){                 // 50ms초 기다림
         break;
      }

      gu16JobTmpStep1 = CheckKeyLed();        // 2번 LED(KEY_LED_K2) 
      if(gu16JobTmpStep1 & (KEY_LED_K2)){
         ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
       }
      else{                                // 미 점등된 LED가 있을 경우
          gu8JobCheckStep = 0xff;          // 에러가 발생한 스탭 표시
       }
          break;             
    }

    case 7:                             // "3"번 터치
    {
      if(gu16TimerJob > 100){           
         gu8JobCheckStep = 0xff;        
         break;
       }
  
      set_wifi();                       // "3" 터치 동작
      if(gu16TimerJob < 50){            // 50ms초 기다림
         break;
      }

       ++gu8JobCheckStep;
       gu16TimerJob = 0;
       break;      
    }

    case 8:                             // "3" 터치 해제 or 확인, key led 중 3번 led가 점등시 ok , 미 점등시 NG
    {
      if(gu16TimerJob > 100){           
         gu8JobCheckStep = 0xff;        
         break;
       }
  
      clr_wifi();                       // "3" 터치 동작 해제      
      if(gu16TimerJob < 50){            // 50ms초 기다림
         break;
      }
      gu16JobTmpStep1 = CheckKeyLed();        // 3번 LED(KEY_LED_K1) 
      if(gu16JobTmpStep1 & (KEY_LED_K1)){
         ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
       }
      else{                                // 미 점등된 LED가 있을 경우
          gu8JobCheckStep = 0xff;          // 에러가 발생한 스탭 표시
       }
          break;             
    }

    case 9:                             // "4"번 터치
    {
      if(gu16TimerJob > 100){           
         gu8JobCheckStep = 0xff;        
         break;
       }
  
      set_rf_pin();                       // "4" 터치 동작
      if(gu16TimerJob < 50){              // 50ms초 기다림
         break;
      }      
       ++gu8JobCheckStep;
       gu16TimerJob = 0;
       break;      
    }

    case 10:                             // "4" 터치 해제 or 확인, key led 중 4번 led가 점등시 ok , 미 점등시 NG
    {
      if(gu16TimerJob > 100){           
         gu8JobCheckStep = 0xff;        
         break;
       }
  
      clr_rf_pin();                     // "4" 터치 동작 해제      
      if(gu16TimerJob < 50){            // 50ms초 기다림
         break;
      }
      gu16JobTmpStep1 = CheckKeyLed();        // 4번 LED(KEY_LED_K6) 
      if(gu16JobTmpStep1 & (KEY_LED_K6)){
         ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
       }
      else{                                // 미 점등된 LED가 있을 경우
          gu8JobCheckStep = 0xff;          // 에러가 발생한 스탭 표시
       }
          break;             
    }

    case 11:                             // "*" 터치
    {
      if(gu16TimerJob > 100){           
         gu8JobCheckStep = 0xff;        
         break;
       }
  
      set_KEYSTAR();                    // "*" 터치 동작
      if(gu16TimerJob < 50){            // 50ms초 기다림
         break;
      }
      
       ++gu8JobCheckStep;
       gu16TimerJob = 0;
       break;      
    }

    case 12:                             // "*" 터치 해제 or 확인, key led가 off 되었으면 ok , 계속 점등시 NG
    {
      if(gu16TimerJob > 100){           
         gu8JobCheckStep = 0xff;        
         break;
       }
  
      set_KEYSTAR();                        // "0" 터치 동작 해제      
      if(gu16TimerJob < 50){                // 50ms초 기다림
         break;
      } 

      if(!(gu16JobTmpStep1 & (KEY_LED_NUMALL))){
        
         ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
       }
      else{                                // 계속 점등된 LED가 있을 경우
         gu8JobCheckStep = 0xff;           // 에러가 발생한 스탭 표시
       }
         break;             
    }   

    case 13:                               // 근접센서(데드볼트) 상태 검사
    {
      if(gu16TimerJob > 100){           
         gu8JobCheckStep = 0xff;        
         break;
       }
        
      if(get_state_deadbolt()){            // 만약 데드볼트가 open 상태이면 다음 항목으로

         clr_vbat_target();                // 6V 전원 off
    	   gu8JobCheckStep = 0;
         gu16TimerJob = 0;                       
      }
      else                                 // 데드볼트가 close 상태이면 Error 처리
      {
         gu8JobCheckStep = 0xff;                                  
    	   break;
      }
    }
    
    default:
    {
      if(gu8JobCheckStep){
        gu8JobCheckStep = 0xff;
      }      
      break;
    }
  }
}

/*-----------------------------------------------------
  * @brief  트리거 자동 설정
  * @param  None
  * @retval None
-----------------------------------------------------*/
void CheckTriggerSetting(void)
{
  switch(gu8JobCheckStep)
  {
   case 1: // 스탭1 = led 점등된게 있는지 확인
    { 
      if(gu16TimerJob > 100){ // Wait for cylinder, 기본 10ms 타이머, 10초 초과해도 반응없으면 에러처리, 기다리는 석 아님.
         gu8JobCheckStep = 0xff; // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 발생, 검사 스탭 표시
         break;
       }
 
        ++gu8JobCheckStep;
          gu16TimerJob = 0;
          break;
    }   

    case 2: // 스탭2 = 등록 버튼 동작
    {
      if(gu16TimerJob > 100){ // Wait for cylinder, 기본 10ms 타이머
         gu8JobCheckStep = 0xff; // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 발생, 검사 스탭 표시
         break;
       }
                              
      set_B1point_pin();     // set 버튼 동작 함수, 500ms 동안 set_B1point_pin()를 반복함.
      if(gu16TimerJob < 50){ // (gu16TimerJob 가 50보다 보다 작으면 break;로 감. 아니면 다음 단계 ( ++gu8JobCheckStep;  gu16TimerJob = 0; )로 감.
         break;
       }
        ++gu8JobCheckStep;
          gu16TimerJob = 0;
          break;
    }

    case 3: // 스탭3 = 등록 버튼 해제
    {
      if(gu16TimerJob > 100){ // Wait for cylinder, 기본 10ms 타이머
         gu8JobCheckStep = 0xff; // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 발생, 검사 스탭 표시
         break;
       }         
                                
      clr_B1point_pin();         // 500ms 동안 clr_B1point_pin()를 반복함.
      if(gu16TimerJob < 100){    // (gu16TimerJob 가 50보다 보다 작으면 break;로 감. 아니면 다음 단계 ( ++gu8JobCheckStep;  gu16TimerJob = 0; )로 감.
         break;
       }

       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
    }

    case 4: // 스탭4 = 숫자 1,2,3,4,5,6,7,8,9,0,*,# LED가 점등되었는지 확인, 판단 => 점등시: 양 / 미점등시: 불
    {
      if(gu16TimerJob > 200){ // Wait for cylinder, 기본 10ms 타이머
         gu8JobCheckStep = 0xff; // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 발생, 검사 스탭 표시
         break;
       }

       gu16JobTmpStep1 = CheckKeyLed();
    // gu16JobTmpStep1 &= ~gu16JobTmp1; 
      if(gu16JobTmpStep1 & (KEY_LED_NUMALL)){

       ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
       }
      else{                        // LED가 미 점등된게 있을 경우
         gu8JobCheckStep = 0xff;    // 검사 중 에러가 발생했을 경우 에러 처리, 시간 초과 경우도 포함, 검사 중 에러가 발생한 스탭도 표시
       }
         break;
    }

    case 5:                             // "8"번 터치
    {
      if(gu16TimerJob > 100){           // Wait for cylinder, 기본 10ms 타이머, 10초 초과해도 반응없으면 에러처리, 기다리는 항목 아님.
         gu8JobCheckStep = 0xff;        // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 실행되어 에러 발생, 검사 스탭 표시
         break;
       }
  
      set_KEY8();                       // "8" 터치 동작
      if(gu16TimerJob < 50){            // 50ms초 기다림
         break;
      }      
       ++gu8JobCheckStep;
       gu16TimerJob = 0;
       break;      
    }

    case 6:                             // "8" 터치 해제 or 확인, key led 중 8번 led가 점등시 ok , 미 점등시 NG
    {
      if(gu16TimerJob > 100){           // Wait for cylinder, 기본 10ms 타이머, 10초 초과해도 반응없으면 에러처리, 기다리는 항목 아님.
         gu8JobCheckStep = 0xff;        // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 실행되어 에러 발생, 검사 스탭 표시
         break;
       }
  
      clr_KEY8();                       // "8" 터치 동작 해제      
      if(gu16TimerJob < 50){            // 50ms초 기다림
         break;
      }
      gu16JobTmpStep1 = CheckKeyLed();        // 8번 LED(KEY_LED_K8) 
      if(gu16JobTmpStep1 & (KEY_LED_K8)){
         ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
       }
      else{                                // 미 점등된 LED가 있을 경우
          gu8JobCheckStep = 0xff;          // 에러가 발생한 스탭 표시
       }
          break;             
    }

    case 7:                             // "2"번 터치
    {
      if(gu16TimerJob > 100){           // Wait for cylinder, 기본 10ms 타이머, 10초 초과해도 반응없으면 에러처리, 기다리는 항목 아님.
         gu8JobCheckStep = 0xff;        // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 실행되어 에러 발생, 검사 스탭 표시
         break;
       }
  
      set_fire_sensor();                // "2" 터치 동작
      if(gu16TimerJob < 50){            // 50ms초 기다림
         break;
      }      
       ++gu8JobCheckStep;
       gu16TimerJob = 0;
       break;      
    }

    case 8:                             // "2" 터치 해제 or 확인, key led 중 2번 led가 점등시 ok , 미 점등시 NG
    {
      if(gu16TimerJob > 100){           // Wait for cylinder, 기본 10ms 타이머, 10초 초과해도 반응없으면 에러처리, 기다리는 항목 아님.
         gu8JobCheckStep = 0xff;        // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 실행되어 에러 발생, 검사 스탭 표시
         break;
       }
  
      clr_fire_sensor();                // "2" 터치 동작 해제      
      if(gu16TimerJob < 50){            // 50ms초 기다림
         break;
      }
      gu16JobTmpStep1 = CheckKeyLed();        // 2번 LED(KEY_LED_K2) 
      if(gu16JobTmpStep1 & (KEY_LED_K2)){
         ++gu8JobCheckStep;
         gu16TimerJob = 0;
         break;
       }
      else{                                // 미 점등된 LED가 있을 경우
          gu8JobCheckStep = 0xff;          // 에러가 발생한 스탭 표시
       }
          break;             
    }

    case 9:                             // "*" 터치 
    {
      if(gu16TimerJob > 100){           // Wait for cylinder, 기본 10ms 타이머, 10초 초과해도 반응없으면 에러처리, 기다리는 항목 아님.
         gu8JobCheckStep = 0xff;        // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 실행되어 에러 발생, 검사 스탭 표시
         break;
       }
  
      set_KEYSTAR();                    // "*" 터치 동작
      if(gu16TimerJob < 50){            // 50ms초 기다림
         break;
      }
      
       ++gu8JobCheckStep;
       gu16TimerJob = 0;
       break;      
    }

    case 10:                             // "*" 터치 해제 or 확인, key led가 off 되었으면 ok , 계속 점등시 NG
    {
      if(gu16TimerJob > 100){           // Wait for cylinder, 기본 10ms 타이머, 10초 초과해도 반응없으면 에러처리, 기다리는 항목 아님.
         gu8JobCheckStep = 0xff;        // 검사 항목 중 에러 발생, 지정된 시간이 초과 되었을때 실행되어 에러 발생, 검사 스탭 표시
         break;
       }
  
      set_KEYSTAR();                        // "0" 터치 동작 해제      
      if(gu16TimerJob < 50){                // 50ms초 기다림
         break;
      } 

         gu16JobTmpStep1 = CheckKeyLed();        
      if(!(gu16JobTmpStep1 & (KEY_LED_NUMALL))){
         gu8JobCheckStep = 0;
         gu16TimerJob = 0;
         break;
       }
      else{                                // 계속 점등된 LED가 있을 경우
          gu8JobCheckStep = 0xff;          // 에러가 발생한 스탭 표시
       }
          break;             
    }

    default:
    {
      if(gu8JobCheckStep){
           gu8JobCheckStep = 0xff;
        }
         break;
    }
  }  
}

/*-----------------------------------------------------
  * @brief  Check Version
  * @param  None
  * @retval None
-----------------------------------------------------*/
void CheckVersion(void)
{
  switch(gu8JobCheckStep)
  {
    case 1:
    {
      if(gu16TimerJob > 100){ // Wait for cylinder
        break;
      }
      set_vbat_target(); //set_vemg_target();

      ++gu8JobCheckStep;
      gu16TimerJob = 0;
      break;
    }
    case 2:
    {
      gu16JobTmp1 = 0;
      ++gu8JobCheckStep;
      //--- Test
      // gu16JobBuff[0] = 3;
      // gu16JobBuff[1] = 10;
      // gu16JobBuff[2] = 2;
      // gu16JobBuff[3] = 1;
      // gu8JobCheckStep = 3;
      //-----Test End

      gu16TimerJob = 0;
      break;
    }
    case 3:
    {
      if(gu16TimerJob > 800){
        gu8JobCheckStep = 0xff;
        break;
      }
      gu16JobTmpStep1 = CheckKeyLed();
      if(gu16JobTmpStep1 & (KEY_LED_K7 | KEY_LED_K0 | KEY_LED_K3 | KEY_LED_K2 | KEY_LED_K1 | KEY_LED_K6 | KEY_LED_K5 | KEY_LED_K4 | KEY_LED_K9 | KEY_LED_K8)){
        FLAG_ADC_FINISH = 0;
        FLAG_ADC_START = 0;
        ++gu8JobCheckStep;
        gu16TimerJob = 0;
      }
      
      break;
    }
    case 4:
    {
      if(!FLAG_ADC_FINISH){
        break;
      }
      FLAG_ADC_FINISH = 0;
      FLAG_ADC_START = 0;
      gu16JobTmpStep1 = GetKeyLedValue();
      if(gu16JobTmpStep1 != 0xffff){
        gu16JobBuff[gu16JobTmp1] = gu16JobTmpStep1;
        FLAG_ADC_FINISH = 0;
        FLAG_ADC_START = 0;
        ++gu8JobCheckStep;
        gu16TimerJob = 0;
        
      }
      if(gu16TimerJob > 100){
        gu8JobCheckStep = 0xff;
        gu16TimerJob = 0;
      }
      
      break;
    }
    case 5:
    {
      if(gu16TimerJob < 5){
        break;
      }
      FLAG_ADC_FINISH = 0;
      FLAG_ADC_START = 0;

      ++gu8JobCheckStep;
      gu16TimerJob = 0;
      break;
    }
    case 6:
    {
      if(gu16TimerJob > 500){
        gu8JobCheckStep = 0xff;
        gu16TimerJob = 0;
        break;
      }
      if(!FLAG_ADC_FINISH){
        break;
      }
      FLAG_ADC_FINISH = 0;
      FLAG_ADC_START = 0;
      gu16JobTmpStep1 = GetKeyLedValue();
      if(gu16JobTmpStep1 != gu16JobBuff[gu16JobTmp1]){
        if(++gu16JobTmp1 > 3){//3
          ++gu8JobCheckStep;
          gu16TimerJob = 0;
        }
        else{
          --gu8JobCheckStep;
          --gu8JobCheckStep;
          gu16TimerJob = 0;
        }
      }
      
      break;
    }
    case 7:
    {
      if(gu16TimerJob > 300){
        gu8JobCheckStep = 0xff;
        // break;
      }
      gu16JobTmpStep1 = CheckKeyLed();// KEY_LED_KSH | KEY_LED_KST |  
      gu16JobTmpStep1 &= (KEY_LED_NUMALL);
      if(gu16JobTmpStep1 & (KEY_LED_NUMALL)){

        gu8JobCheckStep = 8;
        gu16TimerJob = 0;
      }
      
      break;
    }
    case 8:
    {
      if(gu16JobBuff[0] < 10){
        if(!WriteDataOnLcd(POSITION_STATE_X,1,POSITION_STATE_YN,8,(const unsigned char*)&constLcdNum[gu16JobBuff[0]][0])){
          break;
        }
      }
      if(gu16JobBuff[1] == 10){
        if(!WriteDataOnLcd(POSITION_STATE_X,1,(POSITION_STATE_YN+8),8,(const unsigned char*)&constLcdNum[10][0])){
          break;
        }
      }
      if(gu16JobBuff[2] < 10){
        if(!WriteDataOnLcd(POSITION_STATE_X,1,(POSITION_STATE_YN+16),8,(const unsigned char*)&constLcdNum[gu16JobBuff[2]][0])){
          break;
        }
      }
      if(gu16JobBuff[3] < 10){
        if(!WriteDataOnLcd(POSITION_STATE_X,1,(POSITION_STATE_YN+24),8,(const unsigned char*)&constLcdNum[gu16JobBuff[3]][0])){
          break;
        }
      }
      
      ++gu8JobCheckStep;
      gu16TimerJob = 0;
      
      break;
    }
    case 9:
    {
      if(gu16TimerJob > 400){
        gu8JobCheckStep = 0xff;
        // break;
      }
      gu16JobTmpStep1 = CheckKeyLed();// KEY_LED_KSH | KEY_LED_KST |  
      gu16JobTmpStep1 &= (KEY_LED_KSH | KEY_LED_KST |  KEY_LED_K7 | KEY_LED_K0 |  KEY_LED_K6 | KEY_LED_K9 | KEY_LED_K8);
      if(gu16JobTmpStep1 & (KEY_LED_KSH | KEY_LED_KST |  KEY_LED_K7 | KEY_LED_K0 |  KEY_LED_K6 | KEY_LED_K9 | KEY_LED_K8)){
          break;
      }
      gu8JobCheckStep = 0;
      gu16TimerJob = 0;
      break;
    }
    default:
    {
      if(gu8JobCheckStep){
        gu8JobCheckStep = 0xff;
      }
      
      break;
    }
  }
}

/*-----------------------------------------------------
  * @brief  Check Ansim S/W
  * @param  None
  * @retval None
-----------------------------------------------------*/
void CheckAnsimSw(void)
{
  switch(gu8JobCheckStep)
  {
    case 1:
    {
      gu16JobTmp1 = CheckKeyLed();
      if(gu16JobTmp1 & KEY_LED_K5){
        gu8JobCheckStep = 0xff;
        gu16TimerJob = 0;
        break;
      }
      set_sw_ansim1();

      ++gu8JobCheckStep;
      gu16TimerJob = 0;
      break;
    }
    case 2:
    {
      gu16JobTmpStep1 = CheckKeyLed();
      gu16JobTmpStep1 &= ~gu16JobTmp1;
      if(gu16JobTmpStep1 == KEY_LED_K5){
        clr_sw_ansim1();

        ++gu8JobCheckStep;
        gu16TimerJob = 0;
      }
      if(gu16TimerJob > 300){
        gu8JobCheckStep = 0xff;
        gu16TimerJob = 0;
      }
      
      break;
    }
    case 3:
    {
      clr_sw_ansim1();
      if(gu16TimerJob < 10){
        break;
      }
      gu16JobTmpStep1 = CheckKeyLed();
      gu16JobTmpStep1 &= ~gu16JobTmp1;
      if(gu16JobTmpStep1 == KEY_LED_K5){
        break;
      }
      set_sw_ansim2();

      ++gu8JobCheckStep;
      gu16TimerJob = 0;
      break;
    }
    case 4:
    {
      gu16JobTmpStep1 = CheckKeyLed();
      gu16JobTmpStep1 &= ~gu16JobTmp1;
      if(gu16JobTmpStep1 == KEY_LED_K5){
        clr_sw_ansim2();

        gu8JobCheckStep = 0;
        gu16TimerJob = 0;
      }
      if(gu16TimerJob > 300){
        gu8JobCheckStep = 0xff;
        gu16TimerJob = 0;
      }
      
      break;
    }
    default:
    {
      if(gu8JobCheckStep){
        gu8JobCheckStep = 0xff;
      }
      
      break;
    }
  }
}

/*-----------------------------------------------------
  * @brief  Check HomeAuto
  * @param  None
  * @retval None
-----------------------------------------------------*/
void CheckHomeAuto(void)
{
  switch(gu8JobCheckStep)
  {
    case 1:
    {
      gu16JobTmp1 = CheckKeyLed();
      if(gu16JobTmp1 & KEY_LED_K6){
        gu8JobCheckStep = 0xff;
        gu16TimerJob = 0;
        break;
      }
      set_home_auto();

      ++gu8JobCheckStep;
      gu16TimerJob = 0;
      break;
    }
    case 2:
    {
      gu16JobTmpStep1 = CheckKeyLed();
      gu16JobTmpStep1 &= ~gu16JobTmp1;
      if(gu16JobTmpStep1 == KEY_LED_K6){
        clr_home_auto();

        gu8JobCheckStep = 0;
        gu16TimerJob = 0;
      }
      if(gu16TimerJob > 300){
        gu8JobCheckStep = 0xff;
        gu16TimerJob = 0;
      }
      
      break;
    }
    default:
    {
      if(gu8JobCheckStep){
        gu8JobCheckStep = 0xff;
      }
      
      break;
    }
  }
}

/*-----------------------------------------------------
  * @brief  Check Fire
  * @param  None
  * @retval None
-----------------------------------------------------*/
void CheckFireSensor(void)
{
  switch(gu8JobCheckStep)
  {
    case 1:
    {
      gu16JobTmp1 = CheckKeyLed();
      if(gu16JobTmp1 & KEY_LED_K7){
        gu8JobCheckStep = 0xff;
        gu16TimerJob = 0;
        break;
      }
      set_fire_sensor();

      ++gu8JobCheckStep;
      gu16TimerJob = 0;
      break;
    }
    case 2:
    {
      gu16JobTmpStep1 = CheckKeyLed();
      gu16JobTmpStep1 &= ~gu16JobTmp1;
      if(gu16JobTmpStep1 == KEY_LED_K7){
        clr_fire_sensor();

        gu8JobCheckStep = 0;
        gu16TimerJob = 0;
      }
      if(gu16TimerJob > 300){
        gu8JobCheckStep = 0xff;
        gu16TimerJob = 0;
      }
      
      break;
    }
    default:
    {
      if(gu8JobCheckStep){
        gu8JobCheckStep = 0xff;
      }
      
      break;
    }
  }
}

/*-----------------------------------------------------
  * @brief  Check handle
  * @param  None
  * @retval None
-----------------------------------------------------*/
void Checkhandle(void)
{
  switch(gu8JobCheckStep)
  {
    case 1:
    {
      gu16JobTmp1 = CheckKeyLed();
      if(!(gu16JobTmp1 & KEY_LED_K8)){
        gu8JobCheckStep = 0xff;
        gu16TimerJob = 0;
        break;
      }
      set_handle_sw();

      ++gu8JobCheckStep;
      gu16TimerJob = 0;
      break;
    }
    case 2:
    {
      gu16JobTmpStep1 = CheckKeyLed();
      //gu16JobTmpStep1 &= ~gu16JobTmp1;
      if(!(gu16JobTmpStep1 & KEY_LED_K8)){
        clr_handle_sw();

        gu8JobCheckStep = 0;
        gu16TimerJob = 0;
      }
      if(gu16TimerJob > 300){
        gu8JobCheckStep = 0xff;
        gu16TimerJob = 0;
      }
      
      break;
    }

    default:
    {
      if(gu8JobCheckStep){
        gu8JobCheckStep = 0xff;
      }
      
      break;
    }
  }
}

/*-----------------------------------------------------
  * @brief  Change from Low Voltage to normaal voltage
  * @param  None
  * @retval None
-----------------------------------------------------*/
void ChangeNormalVotage(void)
{
  switch(gu8JobCheckStep)
  {
    case 1:
    {
      gu16JobTmp1 = CheckKeyLed();
      clr_low_battery();
      
      ++gu8JobCheckStep;
      gu16TimerJob = 0;
      break;
    }
    case 2:
    {
      if(gu16TimerJob < 20){
        break;
      }
      gu16JobTmpStep1 = CheckKeyLed();
      
      if(gu16JobTmpStep1 != gu16JobTmp1){
        gu8JobCheckStep = 0xff;
      }
      else{
        gu8JobCheckStep = 0;
      }
      gu16TimerJob = 0;
      break;
    }
    default:
    {
      if(gu8JobCheckStep){
        gu8JobCheckStep = 0xff;
    }
      
      break;
    }
  }
}

/*-----------------------------------------------------
  * @brief  Check Register S/W
  * @param  None
  * @retval None
-----------------------------------------------------*/
void CheckRegSw(void)
{
  switch(gu8JobCheckStep)
  {
    case 1:
    {
      
      gu16JobTmp1 = CheckKeyLed();
      if(gu16JobTmp1 & (KEY_LED_K5 | KEY_LED_K6)){ // KEY_LED_K4 | 
        gu8JobCheckStep = 0xff;
        gu16TimerJob = 0;
        break;
      }
      gu16JobTmp1 &= ~KEY_LED_K4;
      set_reg_sw();

      ++gu8JobCheckStep;
      gu16TimerJob = 0;
      break;
    }
    case 2:
    {
      gu16JobTmpStep1 = CheckKeyLed();
      gu16JobTmpStep1 &= ~gu16JobTmp1;
      if(gu16JobTmpStep1 == (KEY_LED_K4 | KEY_LED_K5 | KEY_LED_K6)){
        clr_reg_sw();

        gu8JobCheckStep = 0;
        gu16TimerJob = 0;
      }
      if(gu16TimerJob > 300){
        gu8JobCheckStep = 0xff;
        gu16TimerJob = 0;
      }
      
      break;
    }
    default:
    {
      if(gu8JobCheckStep){
        gu8JobCheckStep = 0xff;
      }
      
      break;
    }
  }
}

/*-----------------------------------------------------
  * @brief  Check UART PIN
  * @param  None
  * @retval None
-----------------------------------------------------*/
void CheckUartPin(void)
{
  switch(gu8JobCheckStep)
  {
    case 1:
    {
      gu16JobTmp1 = CheckKeyLed();
     if(gu16JobTmp1 & (KEY_LED_KSH | KEY_LED_K0 | KEY_LED_KST)){
        gu8JobCheckStep = 0xff;
        gu16TimerJob = 0;
        break;
      }
      set_rf_pin();

      ++gu8JobCheckStep;
      gu16TimerJob = 0;
      break;
    }
    case 2:
    {
      gu16JobTmpStep1 = CheckKeyLed();
      gu16JobTmpStep1 &= ~gu16JobTmp1;
      if(gu16JobTmpStep1 == (KEY_LED_KSH | KEY_LED_K0 | KEY_LED_KST)){//(KEY_LED_KSH | KEY_LED_K0 | KEY_LED_KST)
        clr_rf_pin();

        gu8JobCheckStep = 0;
        gu16TimerJob = 0;
      }
      if(gu16TimerJob > 300){
        gu8JobCheckStep = 0xff;
        gu16TimerJob = 0;
      }
      
      break;
    }
    default:
    {
      if(gu8JobCheckStep){
        gu8JobCheckStep = 0xff;
      }
      
      break;
    }
  }
}

/*-----------------------------------------------------
  * @brief  Check Wifi Pin
  * @param  None
  * @retval None
-----------------------------------------------------*/
void CheckWifiPin(void)
{
  switch(gu8JobCheckStep)
  {
    case 1:
    {
      if(!get_state_deadbolt()){
        gu16JobTmp1 = 0xff;
      }
      else{
        gu16JobTmp1 = 0;
      }
      gu16JobTmp1 = CheckKeyLed();
      if(gu16JobTmp1 & (KEY_LED_K7 | KEY_LED_K8 | KEY_LED_K9)){
        gu8JobCheckStep = 0xff;
        gu16TimerJob = 0;
        break;
      }
      set_wifi();

      ++gu8JobCheckStep;
      gu16TimerJob = 0;
      break;
    }
    case 2:
    {
      gu16JobTmpStep1 = CheckKeyLed();
      gu16JobTmpStep1 &= ~gu16JobTmp1;
      if(gu16JobTmpStep1 == (KEY_LED_K7 | KEY_LED_K8 | KEY_LED_K9)){
        clr_wifi();

        gu8JobCheckStep = 0;//++gu8JobCheckStep;
        gu16TimerJob = 0;
      }
      if(gu16TimerJob > 300){
        gu8JobCheckStep = 0xff;
        gu16TimerJob = 0;
      }
      
      break;
    }
    // case 3:
    // {
    //   if(!get_state_deadbolt()){
    //     gu16JobTmp1 = 0xff;
    //   }
    //   else{
    //     gu16JobTmp1 = 0;
    //   }
    //   gu8JobCheckStep = 0;
    //   gu16TimerJob = 0;
    //   break;
    // }
    default:
    {
      if(gu8JobCheckStep){
        gu8JobCheckStep = 0xff;
      }
      
      break;
    }
  }
}

/*-----------------------------------------------------
  * @brief  Check Mortise
  * @param  None
  * @retval None
-----------------------------------------------------*/
void CheckMortise(void)
{
  switch(gu8JobCheckStep)
  {
    case 1:
    {
      // if(!get_state_deadbolt()){
      //   gu8JobCheckStep = 0xff;
      // }
      // else{
      //   ++gu8JobCheckStep;
      // }
      if(gu16JobTmp1 == 0xff){
        gu8JobCheckStep = 0xff;
      }
      else{
        ++gu8JobCheckStep;
      }
      gu16TimerJob = 0;
      break;
    }
    case 2:
    {
      if(!get_state_deadbolt()){

        ++gu8JobCheckStep;
        gu16TimerJob = 0;
      }
      if(gu16TimerJob > 400){
        gu8JobCheckStep = 0xff;
        gu16TimerJob = 0;
      }
      
      break;
    }
    case 3:
    {
      if(get_state_deadbolt()){

        ++gu8JobCheckStep;
        gu16TimerJob = 0;
      }
      if(gu16TimerJob > 400){
        gu8JobCheckStep = 0xff;
        gu16TimerJob = 0;
      }
      
      break;
    }
    case 4:
    {
      
      if(get_state_deadbolt()){
        if(gu16TimerJob > 350){
          gu8JobCheckStep = 0;
          gu16TimerJob = 0;
        }
      }
      else{
        if(gu16TimerJob > 20){
          gu8JobCheckStep = 0xff;
          gu16TimerJob = 0;
        }
      }
      break;
    }
    default:
    {
      if(gu8JobCheckStep){
        gu8JobCheckStep = 0xff;
      }
      
      break;
    }
  }
}

//---------------------------------------------------
void TestCnt(void)
{
  //test
  if(gu16TimerJob > 100){
    gu8JobCheckStep = 0;
  }//test
}

//---------------------------------------------------
/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
*/
//----------------------------------------------------

/*
  * 함수에 대한 용도 설명
  * gu16TimerJob = 0; 
  * gu8JobCheckStep = 0;
  * FLAG_EVT_INPUT = 0;   // 모든 키(스위치 값) 초기화
  * gu8JobCheckNum = 0;   // 첫번째 작업 번호
  * gu8JobCheckStep = 1;   //반드시 다음 작업의 첫번째 스탭 설정
  * 
*/
void defaultJob(void *argument)
{
  osStatus_t osStateValue;
  
  // static uint16_t u16dTskTmpStep;
    // WaitReadyForStart();
  gu8JobStep = 0;
  /* Infinite loop */
  for(;;)
  {
    switch(gu8JobStep)
    {
        case JOB_STATE_INIT:
        {
          MX_GPIO_Init();
          HAL_UART_MspInit(&huart1);
          // osSemaphoreRelease(LcdTaskSemCntHandle);
          gu8TaskLcd = TASK_LCD_STEP_INITIAL;
#ifdef ENABLE_EVENT_FALG
          LcdTaskSemCntHandle = osSemaphoreNew(1, 1, &LcdTaskSemCnt_attributes);
#endif

          gu16TimerJob = 0;
          ++gu8JobStep;
          break;
        }
        case JOB_STATE_INIT1:
        {
          if(gu8TaskLcd != TASK_LCD_STEP_WAIT){
            break;
          }

          FLAG_EVT_INPUT = 0;
          gu16TimerJob = 0;
          ++gu8JobStep;
          break;
        }
        case JOB_STATE_WAIT:
        {
          if(!DEF_INPUT_SW1 & !DEF_INPUT_SW2){
            if(gu16TimerJob > 70){
              FLAG_EVT_INPUT = 0;
              ++gu8JobStep;
              gu16TimerJob = 0;
            }
          }
          else if(!DEF_INPUT_SW2){
            if(gu16TimerJob > 1000){
              FLAG_MANUAL_STEP ^= 1;
              
              gu8JobStep = JOB_STATE_INIT;
              gu16TimerJob = 0; 
            }
          }
          else{
              gu16TimerJob = 0;
          }
          
          break;
        }
        case JOB_STATE_READY:
        {
          if(gu8TaskLcd != TASK_LCD_STEP_WAIT){
            break;
          }
          osStateValue = osSemaphoreAcquire(LcdTaskSemCntHandle, osWaitForever);
          if(osStateValue != osOK){
            break;
          }
          memset(gu8LcdDisplayData, 0x00,1024);
          gu8TaskLcd = TASK_LCD_STEP_LOAD;
          
          FLAG_EVT_INPUT = 0;
          ++gu8JobStep;
          gu16TimerJob = 0;
          break;
        }
        case JOB_STATE_READY1:
        {
          if(!DEF_INPUT_SW1 & !DEF_INPUT_SW2){
            set_fix_pba();
            if(gu16TimerJob > 100){
              ++gu8JobStep;
              gu16TimerJob = 0;
            }

          }
          else{
            clr_fix_pba();
            //gu8JobStep = JOB_STATE_INIT;
#ifdef ENABLE_EVENT_FALG
            osSemaphoreDelete(LcdTaskSemCntHandle);
#endif
            gu8JobStep = JOB_STATE_EM_STOP;//JOB_STATE_INIT;
            gu16TimerJob = 0;
          }
          
          break;
        }
        case JOB_STATE_READY2:
        {
          
          if(!WriteDataOnLcd(POSITION_STATE_X,2,POSITION_STATE_Y,48,(const unsigned char*)&state_data[0][0][0])){
            break;
          } 
          if(!WriteDataOnLcd(1,1,POSITION_STEP_Y,119,(const unsigned char*)&step_data_line[0])){
            break;
          }
          gu8JobStep = JOB_STATE_RUN;
          gu16TimerJob = 0;
          break;
        }
        case JOB_STATE_RUN:
        {
          if(!DEF_INPUT_SW1){
            break;
          }
          if(!DEF_INPUT_SW2){
            break;
          }
          // osStateValue = osSemaphoreAcquire(LcdTaskSemCntHandle, osWaitForever);
          // if(osStateValue == osOK){
          //   memset(gu8LcdDisplayData, 0x00,1024);
          //   gu8TaskLcd = TASK_LCD_STEP_LOAD;
          // }
          // osDelay(700);
          if(!WriteDataOnLcd(POSITION_STATE_X,2,POSITION_STATE_Y,48,(const unsigned char*)&state_data[0][0][0])){
            break;
          }
          if(!WriteDataOnLcd(1,1,POSITION_STEP_Y,119,(const unsigned char*)&step_data_line[0])){
            break;
          }
          
          // GetCurentData();
          // osThreadFlagsWait( EVENT_FLAG_UART1_485 , osFlagsWaitAny, DEFAULT_OS_WAIT_TIME);// osWaitForever
          // CheckRxData();
          // set_fix_pba();
          FLAG_EVT_INPUT = 0;   // 모든 키(스위치 값) 초기화
          gu8JobCheckNum = 0;   // 첫번째 작업 번호
          //FLAG_MANUAL_STEP = 1; // test

          gu8JobStep = JOB_STATE_RUN1;
          gu16TimerJob = 0;
          break;
        }
        case JOB_STATE_RUN1:
        {
          if(FLAG_MANUAL_STEP){
            if(FLAG_EVT_INPUT){
              FLAG_EVT_INPUT = 0;
              if(!DEF_INPUT_SW2){
                gu8JobStep = JOB_STATE_CHECK;                
              }
            }
          }
          else{
            gu8JobStep = JOB_STATE_CHECK;
            
          }
          if(gu8JobCheckNum == 14){
            gu8JobStep = JOB_STATE_CHECK;
          }
          if(gu8JobStep == JOB_STATE_CHECK){
            if(!gu8JobCheckNum){
              if(!WriteDataOnLcd(POSITION_STEP_X2,1,POSITION_STEP_Y,119,(const unsigned char*)&step_data[0][0])){
                break;
              }
              if(!WriteDataOnLcd(POSITION_STEP_X3,1,POSITION_STEP_Y,119,(const unsigned char*)&step_data[1][0])){
                break;
              }
            }
            else if(gu8JobCheckNum >= MAX_STEP_DATA_CNT){
              //gu8JobStep = JOB_STATE_INIT;
#ifdef ENABLE_EVENT_FALG
              osSemaphoreDelete(LcdTaskSemCntHandle);
#endif
              gu8JobStep = JOB_STATE_EM_STOP;   // JOB_STATE_INIT;
              gu16TimerJob = 0;
              break;
            }
            else{
              if(!WriteDataOnLcd(POSITION_STEP_X1,1,POSITION_STEP_Y,119,(const unsigned char*)&step_data[gu8JobCheckNum-1][0])){
                break;
              }
              if(!WriteDataOnLcd(POSITION_STEP_X2,1,POSITION_STEP_Y,119,(const unsigned char*)&step_data[gu8JobCheckNum][0])){
                break;
              }     
              if(!WriteDataOnLcd(POSITION_STEP_X3,1,POSITION_STEP_Y,119,(const unsigned char*)&step_data[gu8JobCheckNum+1][0])){
                break;
              }

            }
            gu8JobCheckStep = 1;   // 반드시 다음 작업의 첫번째 스탭 설정
            gu16TimerJob = 0;
          }
          break;
        }
        case JOB_STATE_CHECK:
        {
          switch(gu8JobCheckNum)
          {
            case 0:
            {
              CheckFistSetting();         // 초기 근접센서(데드볼트) 상태 검사
              //if(gu8JobCheckStep){          //TestCnt();
              //   gu8JobCheckStep = 0;
              //}
              break;
            }

            case 1:
            {
               //CheckFactoryReset();            // 공장 초기화
               if(gu8JobCheckStep){          //TestCnt();
                 gu8JobCheckStep = 0;
               }
              break;
            }

            case 2:        
            {  
               //CheckCard();                     // CARD 인식 검사
          	   if(gu8JobCheckStep){           //TestCnt();
                  gu8JobCheckStep = 0;
               }
              break;
            }

            case 3:
            {              
               CheckPasswordOpen();             // Password Open
           	   //if(gu8JobCheckStep){           // TestCnt();
               //   gu8JobCheckStep = 0;
               //}
              break;
            }

            case 4:
            {
               //CheckTrigger();                  // 트리거 인식 검사
           	   if(gu8JobCheckStep){           // TestCnt();
                  gu8JobCheckStep = 0;
               }
              break;
            }

            case 5:
            {
               //Checkopenswitch1();             // open test
           	   if(gu8JobCheckStep){          // TestCnt();
                  gu8JobCheckStep = 0;
               }
              break;
            }

            case 6:
            {
               //Checkopenswitch2();              // Close test
           	   if(gu8JobCheckStep){           // TestCnt();
                  gu8JobCheckStep = 0;
                }
              break;
            }

            case 7:
            {
               //CheckSleep();                    // 대기 전류 측정
           	   if(gu8JobCheckStep){           // TestCnt();
                  gu8JobCheckStep = 0;
                }
              break;
            }

            case 8:
            {
               //Check9V();                  // 9V 인식 검사
               if(gu8JobCheckStep){         //TestCnt();
                  gu8JobCheckStep = 0;
               }
           	   break;
            }

/*----------------------------------------------------------------
// M9180 CARD TYPE은 사용 안함

            case 9:
            {
            // CheckPassWord();           
             if(gu8JobCheckStep){         //TestCnt();
                gu8JobCheckStep = 0;
              }
              break;
            }

            case 10:
            {
             // CheckTriggerSetting();
             if(gu8JobCheckStep){         //TestCnt();
                gu8JobCheckStep = 0;
              }
             //ChangeNormalVotage();
              break;
            }

            case 11:
            {
              //CheckRegSw();
              if(gu8JobCheckStep){        //TestCnt();
                 gu8JobCheckStep = 0;
              } 
              break;
            }

            case 12:
            {
              //CheckUartPin();
              if(gu8JobCheckStep){        //TestCnt();
                 gu8JobCheckStep = 0;
              } 
              break;
            }

            case 13:
            {
              //CheckWifiPin();
              if(gu8JobCheckStep){        //TestCnt();
                gu8JobCheckStep = 0;
              }
              break;
            }

            case 14:
            {
              //CheckMortise();
              if(gu8JobCheckStep){        //TestCnt();
                gu8JobCheckStep = 0;
              }
              break;
            }

            case 15:
            {
              //CheckSleep();
              if(gu8JobCheckStep){        //TestCnt();
                gu8JobCheckStep = 0;
              }
              break;
            }

            case 16:
            {
              //CheckLowBattery();          // 4.3V 인식 검사
              if(gu8JobCheckStep){        //TestCnt();
                gu8JobCheckStep = 0;
              }
              break;
            }

            case 17:
            {
           	 // CheckFactoryReset();
              if(gu8JobCheckStep){        //TestCnt();
                gu8JobCheckStep = 0;
              }
              break;
            }

//----------------------------------------------------------------------*/

            default:
            {
              gu8JobCheckStep = 0;
              break;
            }
          }
          
          if(gu8JobCheckStep == 0xff){                   //에러 처리
             gu16JobTmp1 = gu8JobCheckNum + 1;           //에러난 작업 번호
             gu8JobStep = JOB_STATE_ERROR;
             gu16TimerJob = 0;
          }
          if(gu8JobCheckStep == 0xfe){                  //에러시 작업 표시 없이 종료
            
             gu8JobStep = JOB_STATE_ERROR;
             gu16TimerJob = 0;
          }
          if(gu8JobCheckStep){                          //작업 종료 여부 감시, 값이 0이어도 작업 종료
            break;
          }
          // gu8JobCheckStep = 0;
          if(++gu8JobCheckNum  >= MAX_STEP_DATA_CNT){   // 다음 작업 번호로 변경 후 모든 작업이 종료되었는지 확인
            gu8JobStep = JOB_STATE_OK;
          }
          else{
            gu8JobStep = JOB_STATE_RUN1;                // 다음 작업이 있을 경우 이리로 감
          }
            gu16TimerJob = 0;
          
          break;
        }
        case JOB_STATE_OK:
        {
          ResetJigIoPin();
          FLAG_EVT_INPUT = 0;
          if(!WriteDataOnLcd(POSITION_STATE_X,2,POSITION_STATE_Y,48,(const unsigned char*)&state_data[1][0][0])){
            break;
          }
          if(!WriteDataOnLcd(POSITION_STEP_X1,1,POSITION_STEP_Y,119,(const unsigned char*)&step_empty[0])){
            break;
          }
          if(!WriteDataOnLcd(POSITION_STEP_X2,1,POSITION_STEP_Y,119,(const unsigned char*)&step_data[17][0])){
            break;
          }
          if(!WriteDataOnLcd(POSITION_STEP_X3,1,POSITION_STEP_Y,119,(const unsigned char*)&step_empty[0])){
            break;
          }

          gu8JobStep = JOB_STATE_FINISH;
          gu16TimerJob = 0;
          
          break;
        }
        case JOB_STATE_ERROR:
        {
          ResetJigIoPin();
          clr_sleep_pin();
          FLAG_EVT_INPUT = 0;
          if(!WriteDataOnLcd(POSITION_STATE_X,2,POSITION_STATE_Y,48,(const unsigned char*)&state_data[2][0][0])){
            break;
          }
          
          gu16JobTmpStep1 = gu16JobTmp1 / 10;
          if(gu16JobTmpStep1 > 9){
            gu16JobTmpStep1 = 9;
          }
          if(!WriteDataOnLcd(POSITION_STATE_X,1,(POSITION_STATE_Y+35),8,(const unsigned char*)&constLcdNum[gu16JobTmpStep1][0])){
            break;
          }
          gu16JobTmpStep1 = gu16JobTmp1 % 10;
          if(!WriteDataOnLcd(POSITION_STATE_X,1,(POSITION_STATE_Y+35+8),8,(const unsigned char*)&constLcdNum[gu16JobTmpStep1][0])){
            break;
          }
          gu8JobStep = JOB_STATE_WAIT; // Why? 
          gu16TimerJob = 0;
          
          break;
        }
        case JOB_STATE_FINISH:
        {
          if(gu16TimerJob < 300){
            break;
          }
          ResetJigIoPin();
          FLAG_EVT_INPUT = 0;
          ++gu8JobStep;
          gu16TimerJob = 0;
          
          break;
        }
        case JOB_STATE_FINISH_WAIT:
        {
          if(FLAG_EVT_INPUT){
            FLAG_EVT_INPUT = 0;
            gu8JobStep = JOB_STATE_INIT;
            gu16TimerJob = 0;
          }
          break;
        }
        case JOB_STATE_EM_STOP:
        {
          ResetJigIoPin();

          ++gu8JobStep;
          gu16TimerJob = 0;
          break;
        }
        case JOB_STATE_EM_STOP1:
        {
          if(!DEF_INPUT_SW1){
            break;
          }
          if(!DEF_INPUT_SW2){
            break;
          }
          if(gu8TaskLcd != TASK_LCD_STEP_WAIT){
            break;
          }
          gu8JobStep = JOB_STATE_INIT;
          gu16TimerJob = 0;
          break;
        }
        default:
        {
            gu8JobStep = 0;
            break;
        }
    }
    
  }
}

//----------------------------------------------------------
// * delay no operation
void delay_cycle(uint8_t u8DalayCnt)
{
  for(uint8_t i=0;i < u8DalayCnt ; i++){
    asm volatile("NOP");
  }
}
//---------------------------------------------------------
// Brief : Lcd Data
void WriteLcdData(uint8_t u8LcdData)
{
  uint16_t u16ReadyPort;
  u16ReadyPort = (uint16_t)(u8LcdData) & 0x00ff;
  HAL_GPIO_WritePin(GPIOD, u16ReadyPort, GPIO_PIN_SET);
  u16ReadyPort ^= 0x00ff;
  HAL_GPIO_WritePin(GPIOD, u16ReadyPort, GPIO_PIN_RESET);

}

//----------------------------------------------------------
// # Write Data to Graphic LCD
// #								
void GraphicLcdWriteData(uint8_t u8Cs, uint8_t u8LcdData)
{
	clr_lcd_en();
	delay_cycle(2);
	clr_lcd_cs1();
	clr_lcd_cs2();
	delay_cycle(2);
	if(!u8Cs){       // CS1
		set_lcd_cs1();
	}
	else{						//CS2
		set_lcd_cs2();
	}
	clr_lcd_rw();		// Set for writing
									// Put the data on the port
	WriteLcdData(u8LcdData);
	
	set_lcd_en();		// Pulse the enable pin
	delay_cycle(4);
	clr_lcd_en();
	delay_cycle(2);
  
  clr_lcd_cs1();
	clr_lcd_cs2();	// Reset Chip Select
	
}

//----------------------------------------------------------
// # read Data from Graphic LCD
// #

//--------------------------------------------
/* Write data on LCD */
uint16_t WriteDataOnLcd(uint8_t u8GLcdXStart,uint8_t u8GLcdXSize,uint8_t u8GLcdYStart,uint8_t u8GLcdYSize,const unsigned char* upConstAdd)
{

  osStatus_t osStateValue1;
  uint8_t i,j;
	uint8_t u8PointY;
#ifdef ENABLE_EVENT_FALG
  if(gu8JobStep >= JOB_STATE_EM_STOP){
    return 0;
  }

#endif
  osStateValue1 = osSemaphoreAcquire(LcdTaskSemCntHandle, DEFAULT_OS_WAIT_TIME);
  if(osStateValue1 != osOK){
    //return;
    gu16TimerLcd = 0;
    while((gu8TaskLcd != TASK_LCD_STEP_WAIT) && (gu16TimerLcd < 50)){
      
    };
  }
#ifdef ENABLE_EVENT_FALG
  if(gu8JobStep >= JOB_STATE_EM_STOP){
    return 0;
  }

#endif

  for(i=0;i<u8GLcdYSize;i++){
		for(j=0;j<u8GLcdXSize;j++){
			u8PointY = u8GLcdYStart+i;
			if(u8PointY < 64){
				gu8LcdDisplayData[0][u8PointY][u8GLcdXStart+j] = upConstAdd[i*u8GLcdXSize+j];
			}
			else{
				gu8LcdDisplayData[1][u8PointY-64][u8GLcdXStart+j] = upConstAdd[i*u8GLcdXSize+j];
			}
		}
	}
	gu8GLcdXStart = u8GLcdXStart;
	gu8GLcdXEnd = u8GLcdXStart+u8GLcdXSize-1;
	gu8TaskGraphicLcdY = gu8GLcdYStart = u8GLcdYStart;
	
	gu8GLcdYEnd = u8GLcdYStart+u8GLcdYSize-1;
  
	gu8TaskLcd = TASK_LCD_STEP_WRITE_XY;
  return 1;
}
//----------------------------
/* USER CODE END Header_StartTask02 */
void DisplayTask(void *argument)
{
  /* USER CODE BEGIN LcdTask */
  // LcdTaskSemCntHandle = osSemaphoreNew(1, 1, &LcdTaskSemCnt_attributes);

  /* Infinite loop */
  for(;;)
  {
     switch(gu8TaskLcd){
      case TASK_LCD_STEP_WAIT:
      {
        break;
      }
      case TASK_LCD_STEP_INITIAL:
      {
        clr_lcd_noe();
		set_lcd_dir();
		clr_lcd_res();

        osDelay(10);

        set_lcd_res();
        clr_lcd_en();
        clr_lcd_cs1();
        clr_lcd_cs2();
        lcd_rs_cmd();

        GraphicLcdWriteData(G_LCD_CS1,(CTRL_DISPLAY_START_LINE | 0x00)); //0xc0
		GraphicLcdWriteData(G_LCD_CS2,(CTRL_DISPLAY_START_LINE | 0x00));
		GraphicLcdWriteData(G_LCD_CS1,(CTRL_SET_Y_ADDRESS | 0x00));
		GraphicLcdWriteData(G_LCD_CS2,(CTRL_SET_Y_ADDRESS | 0x00));
		GraphicLcdWriteData(G_LCD_CS1,(CTRL_SET_X_ADDRESS | 0x00));
		GraphicLcdWriteData(G_LCD_CS2,(CTRL_SET_X_ADDRESS | 0x00));
				
		GraphicLcdWriteData(G_LCD_CS1,(CTRL_DISPLAY_ONOFF | CTRL_DISPLAY_ONOFF_ON));
		GraphicLcdWriteData(G_LCD_CS2,(CTRL_DISPLAY_ONOFF | CTRL_DISPLAY_ONOFF_ON));

        // set_lcd_led();
		memcpy(gu8LcdDisplayData, screen1,1024);  // gu8LcdDisplayData LCD에 데이터를 뿌리는 곳

        gu8TaskLcd = TASK_LCD_STEP_LOAD;

        break;
      }
      case TASK_LCD_STEP_LOAD:
      {
        gu8TaskGraphicLcdY = 63;
        while(gu8TaskGraphicLcdY < 64){
          for(gu8TaskGraphicLcdX = 0;gu8TaskGraphicLcdX < 8 ; gu8TaskGraphicLcdX++){
            lcd_rs_cmd();
            // CS1
            GraphicLcdWriteData(G_LCD_CS1,(CTRL_SET_Y_ADDRESS | gu8TaskGraphicLcdY));
            // CS2
            GraphicLcdWriteData(G_LCD_CS2,(CTRL_SET_Y_ADDRESS | (63-gu8TaskGraphicLcdY)));
            // CS1
            GraphicLcdWriteData(G_LCD_CS1,(CTRL_SET_X_ADDRESS | gu8TaskGraphicLcdX));
            // CS2
            GraphicLcdWriteData(G_LCD_CS2,(CTRL_SET_X_ADDRESS | gu8TaskGraphicLcdX));
            osDelay(1);
            lcd_rs_data();
            // CS1
             GraphicLcdWriteData(G_LCD_CS1,gu8LcdDisplayData[0][gu8TaskGraphicLcdY][7-gu8TaskGraphicLcdX]);
            // CS2
            GraphicLcdWriteData(G_LCD_CS2,gu8LcdDisplayData[1][63-gu8TaskGraphicLcdY][7-gu8TaskGraphicLcdX]);
            

          }
          osDelay(5);
          --gu8TaskGraphicLcdY;
        };
        set_lcd_led();
        gu8TaskLcd = TASK_LCD_STEP_WAIT;
        osSemaphoreRelease(LcdTaskSemCntHandle);
        break;
      }
      case TASK_LCD_STEP_WRITE_XY:
      {
        while(gu8TaskGraphicLcdY <= gu8GLcdYEnd){
          for(gu8TaskGraphicLcdX = gu8GLcdXStart;gu8TaskGraphicLcdX <= gu8GLcdXEnd;gu8TaskGraphicLcdX++){
            lcd_rs_cmd();
            if(gu8TaskGraphicLcdY > 63){
              // CS2
              GraphicLcdWriteData(G_LCD_CS2,(CTRL_SET_Y_ADDRESS | (gu8TaskGraphicLcdY-64)));
              GraphicLcdWriteData(G_LCD_CS2,(CTRL_SET_X_ADDRESS | (7-gu8TaskGraphicLcdX)));
            }
            else{
              // CS1
              GraphicLcdWriteData(G_LCD_CS1,(CTRL_SET_Y_ADDRESS | gu8TaskGraphicLcdY));
              GraphicLcdWriteData(G_LCD_CS1,(CTRL_SET_X_ADDRESS | (7-gu8TaskGraphicLcdX)));
            }
            lcd_rs_data();
            if(gu8TaskGraphicLcdY > 63){
              // CS2
              GraphicLcdWriteData(G_LCD_CS2,gu8LcdDisplayData[1][gu8TaskGraphicLcdY-64][gu8TaskGraphicLcdX]);
            }
            else{
              // CS1
              GraphicLcdWriteData(G_LCD_CS1,gu8LcdDisplayData[0][gu8TaskGraphicLcdY][gu8TaskGraphicLcdX]);
            }
          }
          ++gu8TaskGraphicLcdY;
        }
        gu8TaskLcd = TASK_LCD_STEP_WAIT;
        osSemaphoreRelease(LcdTaskSemCntHandle);

        break;
      }
      default:
      {
        gu8TaskLcd = TASK_LCD_STEP_WAIT;
        break;
      }
    }
    
  }
  /* USER CODE END StartTask02 */
}

//-------------------------------
/* void DeviceTask(void *argument) 목적
 * 스위치 입력 여부 감시
 * ADC 입력 값 감시
*/
//--------------------------------
void DeviceTask(void *argument)
{
  for(;;)
  {
    TaskInputThread();
    TaskAdc1Thread();
    if(FLAG_EVT_INPUT){
      if(!DEF_INPUT_SW1){
        if((gu8JobStep > JOB_STATE_RUN) && (gu8JobStep < JOB_STATE_FINISH)){
          FLAG_EVT_INPUT = 0;
#ifdef ENABLE_EVENT_FALG
          osSemaphoreDelete(LcdTaskSemCntHandle);
#endif
          gu8JobStep = JOB_STATE_EM_STOP;//JOB_STATE_INIT;
        }
      }
    }
    if(gu16TimerJob > 2000){
      //if(gu8JobStep >= JOB_STATE_RUN){
        FLAG_EVT_INPUT = 0;
#ifdef ENABLE_EVENT_FALG
        osSemaphoreDelete(LcdTaskSemCntHandle);
#endif
        gu8JobStep = JOB_STATE_EM_STOP;//JOB_STATE_INIT;
        gu16TimerJob = 0;
      //}
    }
  }
}

