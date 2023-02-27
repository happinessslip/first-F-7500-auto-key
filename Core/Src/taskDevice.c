/**
  ******************************************************************************
  * @file           : taskDevice.c
  * @brief          : Default H/W driver program
  ******************************************************************************
  * @attention
  *
  * 
  *
  ******************************************************************************
*/
/* USER CODE Header */

#include "main.h"
#include "my_generic.h"
#include "define_consts.h"
#include "taskDeviceh.h"
/* USER CODE END Includes */
extern void MX_ADC1_Init(void);

/* Define variables */
extern ADC_HandleTypeDef hadc1;
uint8_t gu8TaskAdc1, gu8Adc1Channel, gu8Adc1Data, gu8Adc1TmpCnt;
uint32_t gu32Adc1Tmp[ADC_DATA_MAX][MAX_ADC_CAL_AVG];
uint32_t gu32Adc1Data[ADC_DATA_MAX];

TaskInputPortTypeDef StructInputPort[3] = {
  {SW1_GPIO_Port,SW1_Pin, 3},
  {SW2_GPIO_Port,SW2_Pin, 3},
  {Spare_In1_GPIO_Port,Spare_In1_Pin, 3}
};
uint16_t gu16NumOfInput;
uint16_t gu16AddrBits[SW_vMAX] =
{
  vINPUT_SW_1,
  vINPUT_SW_2,
  vINPUT_SPARE1
};
uint16_t gu16TimerInput[SW_vMAX];
WORD_VAL gu16IoBits;
extern WORD_VAL gu16Flag1; 


/* Define constants */

#define ADC_SWITCH_UL2            ADC_DATA_K3

/*-----------------------------------------------------
  * @brief  checkiing switch or logical siganl
  * @param  None
  * @retval None
-----------------------------------------------------*/
 void TaskInputThread(void)
 {
  
  for(gu16NumOfInput = SW_1; gu16NumOfInput < SW_SPARE1 ; gu16NumOfInput++){
    if(StructInputPort[gu16NumOfInput].GPIOx->IDR & StructInputPort[gu16NumOfInput].GPIO_Pin){
      if(!(gu16IoBits.Val & gu16AddrBits[gu16NumOfInput])){
        if(gu16TimerInput[gu16NumOfInput] > StructInputPort[gu16NumOfInput].vChatTime){
          gu16TimerInput[gu16NumOfInput] = 0;
          gu16IoBits.Val |= (gu16AddrBits[gu16NumOfInput]);
        }
      }
      else{
        gu16TimerInput[gu16NumOfInput] = 0;
      }
    }
    else{
      if(gu16IoBits.Val & gu16AddrBits[gu16NumOfInput]){
        if(gu16TimerInput[gu16NumOfInput] > StructInputPort[gu16NumOfInput].vChatTime){
          gu16TimerInput[gu16NumOfInput] = 0;
          gu16IoBits.Val &= ~(gu16AddrBits[gu16NumOfInput]);
          FLAG_EVT_INPUT = 1;
        }
      }
      else{
        gu16TimerInput[gu16NumOfInput] = 0;
      }
    }
  }
 }
/*-----------------------------------------------------
  * @brief  Reset Jig Port Pin
  * @param  uint8_t gu8Adc1Channel
  * @retval None
-----------------------------------------------------*/
void ResetJigIoPin(void)
{
  clr_fix_pba();
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, Relay_Wifi_Pin|Relay_Temp_Pin|Sol_SetSw_Pin|Sol_OpClSw_Pin
                          |Sol_HandleSw_Pin|Reset485_Pin|Sol_FixPbaOn_Pin|Sol_OpClSw_Pin2
                          |Sol_TriggerOn_Pin|Sol_TriggerOff_Pin|SpareOut1_Pin|SpareOut2_Pin
                          |SpareOut3_Pin|SpareOut4_Pin|Relay_HA_Pin|Relay_RF_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2
                          |GPIO_PIN_3|RS485_de_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, Change_LowVol_Pin|En_VBAT_Pin|En_VEMG_Pin|SpareOut5_Pin
                          |SpareOut6_Pin|SpareOut7_Pin|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15|Test_Pin
                          |GPIO_PIN_8, GPIO_PIN_RESET);

}
/*-----------------------------------------------------
  * @brief  Reset ADC Control data
  * @param  uint8_t gu8Adc1Channel
  * @retval None
-----------------------------------------------------*/
void ResetADCData(void)
{
  gu8Adc1Data = 0;
  gu8Adc1Channel = 0;
  gu8Adc1TmpCnt = 0;
  gu8TaskAdc1 = TASK_ADC_STEP_SET_START;
}
/*-----------------------------------------------------
  * @brief  Calculate average
  * @param  None
  * @retval None
-----------------------------------------------------*/
uint32_t CalculateAverageForAdc(uint8_t u8AdcNum)
{
  uint16_t u16TmpCnt;//, u16TmpBuff[MAX_ADC_CAL_AVG];
  uint32_t u32ReturnValue=0;
  if(u8AdcNum >= ADC_DATA_MAX){
    return 0;
  }
  for(u16TmpCnt=0; u16TmpCnt < MAX_ADC_CAL_AVG; u16TmpCnt++){
    // u16TmpBuff[u16TmpCnt] = (uint16_t)(gu32Adc1Tmp[u8AdcNum][u16TmpCnt] >> 8);
    // u32ReturnValue += (uint32_t)(u16TmpBuff[u16TmpCnt]);
    u32ReturnValue += gu32Adc1Tmp[u8AdcNum][u16TmpCnt];
  } 
  return (u32ReturnValue/MAX_ADC_CAL_AVG);
}
/*-----------------------------------------------------
  * @brief  Configure Regular Channel
  * @param  uint8_t gu8Adc1Channel
  * @retval None
-----------------------------------------------------*/
uint16_t ChangeAdc1Channel(uint8_t u8ChannelNum)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  sConfig.Channel = (uint32_t)u8ChannelNum;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    return 0;
  }
  return 1;
}
/*-----------------------------------------------------
  * @brief  checkiing analog siganal
  * @param  None
  * @retval None
-----------------------------------------------------*/
void TaskAdc1Thread(void)
{
  switch(gu8TaskAdc1)
  {
    
    case TASK_ADC_STEP_SET_START:
    {
      // if(gu8Adc1Channel > MAX_ADC_CHANNEL){
      //    gu8Adc1Channel = 0;
      //    gu8Adc1TmpCnt = 0;
      // }
      if(gu8Adc1Data < ADC_SWITCH_UL2){
        HAL_GPIO_WritePin(En_UL1_GPIO_Port, En_UL1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(En_UL2_GPIO_Port, En_UL2_Pin, GPIO_PIN_RESET);
        gu8Adc1Channel = gu8Adc1Data;
      }
      else{
        HAL_GPIO_WritePin(En_UL2_GPIO_Port, En_UL2_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(En_UL1_GPIO_Port, En_UL1_Pin, GPIO_PIN_RESET);
        gu8Adc1Channel = gu8Adc1Data - ADC_SWITCH_UL2;
      }
      if(ChangeAdc1Channel(gu8Adc1Channel)){
        HAL_ADC_Start(&hadc1);
        ++gu8TaskAdc1;
      }
      else{
        gu8TaskAdc1 = TASK_ADC_STEP_INITIAL;
      }
      break;
    }
    case TASK_ADC_STEP_WAIT_CONVERSION:
    {
      if(HAL_IS_BIT_CLR(hadc1.Instance->SR, ADC_FLAG_EOC)){
        break;
      }
      HAL_ADC_Stop(&hadc1);
      gu8TaskAdc1 = TASK_ADC_STEP_SET_START;
      if(gu8Adc1TmpCnt >= MAX_ADC_CAL_AVG){
        gu8Adc1TmpCnt = 0;
        break;
      }
      if(gu8Adc1Data >= ADC_DATA_MAX){
        gu8Adc1Data = 0;
        break;
      }
      gu32Adc1Tmp[gu8Adc1Data][gu8Adc1TmpCnt] = HAL_ADC_GetValue(&hadc1);
      gu32Adc1Data[gu8Adc1Data] = CalculateAverageForAdc(gu8Adc1Data);

      if(++gu8Adc1Data < ADC_DATA_MAX){
        break;
      }
      gu8Adc1Data = 0;
      if(FLAG_ADC_START){
        FLAG_ADC_FINISH = 1;
      }
      FLAG_ADC_START = 1;
      if(++gu8Adc1TmpCnt < MAX_ADC_CAL_AVG){
        break;
      }
      gu8Adc1TmpCnt = 0;
      
      break;
    }
    case TASK_ADC_STEP_INITIAL:
    {
      MX_ADC1_Init();
      HAL_ADC_MspInit(&hadc1);
      ResetADCData();
      break;
    }
    default:
    {
      break;
    }
  }
}

uint16_t CheckKeyLed(void)
{
  uint16_t u16KeyLedBits=0;
  uint16_t u16PositionBit, u16TmpCnt;

  for(u16TmpCnt = 0; u16TmpCnt < ADC_DATA_MAX; u16TmpCnt++){
    if(gu32Adc1Data[u16TmpCnt] > 500){        // gu32Adc1Data -> READ한 LED 값을 번지에 저장
      u16PositionBit = (0x0001 << u16TmpCnt);
      u16KeyLedBits |= u16PositionBit;
    }
  }
  u16KeyLedBits &= ~(KEY_LED_CD | KEY_LED_LD);
  return u16KeyLedBits;
}

/*-----------------------------------------------------
  * @brief  Get Key LED Value
  * @param  None
  * @retval None
-----------------------------------------------------*/
uint16_t GetKeyLedValue(void)
{
  uint16_t u16GetCnt=0;
  uint16_t u16TmpCnt, u16KeyLedPst;

  for(u16TmpCnt = 0; u16TmpCnt < ADC_DATA_MAX; u16TmpCnt++){
    if(gu32Adc1Data[u16TmpCnt] > 500){
      
      if((u16TmpCnt != ADC_DATA_CD) || (u16TmpCnt != ADC_DATA_LD)){
         u16KeyLedPst = u16TmpCnt;
       ++u16GetCnt;
      }
    }
  }
  if(u16GetCnt == 1){
    switch(u16KeyLedPst)
    {
      case ADC_DATA_K1:
      {
        u16KeyLedPst = 1;
        break;
      }
      case ADC_DATA_K2:
      {
        u16KeyLedPst = 2;
        break;
      }
      case ADC_DATA_K3:
      {
        u16KeyLedPst = 3;
        break;
      }
      case ADC_DATA_K4:
      {
        u16KeyLedPst = 4;   // u16KeyLedPst = 4;
        break;
      }
      case ADC_DATA_K5:
      {
        u16KeyLedPst = 5;
        break;
      }
      case ADC_DATA_K6:
      {
        u16KeyLedPst = 6;
        break;
      }
      case ADC_DATA_K7:
      {
        u16KeyLedPst = 7;
        break;
      }
      case ADC_DATA_K8:
      {
        u16KeyLedPst = 8;
        break;
      }
      case ADC_DATA_K9:
      {
        u16KeyLedPst = 9;
        break;
      }
      case ADC_DATA_K0:
      {
        u16KeyLedPst = 0;
        break;
      }
      case ADC_DATA_KST:
      {
        u16KeyLedPst = 10;
        break;
      }
      case ADC_DATA_KSH:
      {
        u16KeyLedPst = 11;
        break;
      }
      case ADC_DATA_KK:
      {
        u16KeyLedPst = 12;
        break;
      }
      case ADC_DATA_RD:   // LOW POWER LED
      {
        u16KeyLedPst = 13;
        break;
      }
      case ADC_DATA_CD:   // 예비1 LED
      {
        u16KeyLedPst = 14;
        break;
      }
      case ADC_DATA_LD:   // 예비2 LED
      {
        u16KeyLedPst = 15;
        break;
      }
      default:
      {
        u16KeyLedPst = 0xffff;
        break;
      }
    }
    return u16KeyLedPst;
  }
  return 0xffff;
}

