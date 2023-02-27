/*
******************************************************************************
  * @file           : define_consts.h
  * @brief          : Default H/W driver program
  ******************************************************************************
  * @attention
  *
  * 
  *
  ******************************************************************************
*/

#define MAX_RX_DATA 16
#define MAX_ADC_CHANNEL 7
#define MAX_ADC_CAL_AVG 5
#define DEFAULT_OS_WAIT_TIME 1000u
#define SIZE_OF_gu16JobBuff  10
#define ENABLE_EVENT_FALG

enum vSwType {
  SW_1=0,
  SW_2,
  SW_SPARE1,
  SW_vMAX
};

enum _ADC_DATA_NAME{
  ADC_DATA_K7=0, //UL1 //0
  //ADC_DATA_CD=0, //UL1 //0

  ADC_DATA_KSH, //1
  //ADC_DATA_K1, //1

  ADC_DATA_K0,  //2
  //ADC_DATA_K2,  //2

  ADC_DATA_KST, //3
  //ADC_DATA_K3, //3

  ADC_DATA_KK,  //4   ADC_DATA_4
  //ADC_DATA_K4,  //4   ADC_DATA_4

  ADC_DATA_RD,  //5
  //ADC_DATA_K5,  //5

  ADC_DATA_CD,  //6
  //ADC_DATA_K6,  //6

  ADC_DATA_LD,  //7
  //ADC_DATA_K7,  //7

  ADC_DATA_K3,  //UL2 //8
  //ADC_DATA_K8,  //UL2 //8

  ADC_DATA_K2,  //9
  //ADC_DATA_K9,  //9

  ADC_DATA_K1,  //10
  //ADC_DATA_K0,  //10

  ADC_DATA_K6,  //11
  //ADC_DATA_KST,  //11

  ADC_DATA_K5,  //12
  //ADC_DATA_KSO,  //12

  ADC_DATA_K4,  //13
  //ADC_DATA_RD,  //13

  ADC_DATA_K9,  //14
  //ADC_DATA_S1,  //14

  ADC_DATA_K8,  //15
  //ADC_DATA_S2,  //15

  ADC_DATA_MAX  //16
  //ADC_DATA_MAX  //16

};
enum _TASK_ADC_STEP{
  TASK_ADC_STEP_SET_START=0,
  TASK_ADC_STEP_WAIT_CONVERSION,
  TASK_ADC_STEP_INITIAL

};

enum _JOB_FUNCTION_STEP{
  JOB_STATE_INIT=0,
  JOB_STATE_INIT1,
  JOB_STATE_WAIT,
  JOB_STATE_READY,
  JOB_STATE_READY1,
  JOB_STATE_READY2,
  JOB_STATE_RUN,
  JOB_STATE_RUN1,
  JOB_STATE_CHECK,
  JOB_STATE_OK,
  JOB_STATE_ERROR,
  JOB_STATE_FINISH,
  JOB_STATE_FINISH_WAIT,
  JOB_STATE_EM_STOP,
  JOB_STATE_EM_STOP1

};
/*======================================
//: Define Variables For LCD
//======================================*/
//RS=0,RW=0
enum _GLCD_SELECT_CS
{
  G_LCD_CS1=0,
  G_LCD_CS2
};
#define CTRL_DISPLAY_START_LINE     0xc0
#define CTRL_SET_Y_ADDRESS          0x40
#define CTRL_SET_X_ADDRESS          0xb8
#define CTRL_DISPLAY_ONOFF          0x3e
#define CTRL_DISPLAY_ONOFF_ON       0x01
#define CTRL_DISPLAY_ONOFF_OFF      0x00


#define TASK_LCD_STEP_WAIT 		      0
#define TASK_LCD_STEP_INITIAL 		  1
#define TASK_LCD_STEP_LOAD		      2
#define TASK_LCD_STEP_WRITE_XY  	  3


// define Input Port Flag
#define vINPUT_SW_1                 0x00000001
#define vINPUT_SW_2                 0x00000002
#define vINPUT_SPARE1               0x00000004

#define DEF_INPUT_SW1             gu16IoBits.bits.b0
#define DEF_INPUT_SW2             gu16IoBits.bits.b1
#define DEF_INPUT_SPARE1          gu16IoBits.bits.b2
#define DEF_FALG_EVT_SW           gu16IoBits.bits.b3

// Key LED 
// 숫자 KEY
//     = 1 2 3 4 5 6 7 8 9 0 * #
//     = KEY_LED_K2 | KEY_LED_K7 | KEY_LED_K6 | KEY_LED_K5 | KEY_LED_K4 | KEY_LED_K9 | KEY_LED_K8 | KEY_LED_K1 | KEY_LED_KSH | KEY_LED_K0 | KEY_LED_KST | KEY_LED_KK
// 기타 KEY
//       = 저전압 경보
//       = KEY_LED_RD
//       = 닫힘 LED
//       = KEY_LED_K3
#define KEY_LED_K1                  0x0001 // KEY8
#define KEY_LED_KSH                 0x0002 // KEY9
#define KEY_LED_K0                  0x0004 // KEY*
#define KEY_LED_KST                 0x0008 // KEY0
#define KEY_LED_KK                  0x0010 // KEY#
#define KEY_LED_RD                  0x0020 // KEY 저전압
#define KEY_LED_CD                  0x0040 // KEY 사용X
#define KEY_LED_LD                  0x0080 // KEY 사용X
#define KEY_LED_K3                  0x0100 // KEY 닫힘
#define KEY_LED_K2                  0x0200 // KEY1
#define KEY_LED_K7                  0x0400 // KEY2
#define KEY_LED_K6                  0x0800 // KEY3
#define KEY_LED_K5                  0x1000 // KEY4
#define KEY_LED_K4                  0x2000 // KEY5
#define KEY_LED_K9                  0x4000 // KEY6
#define KEY_LED_K8                  0x8000 // KEY7
// 숫자 및 기타 포함
#define KEY_LED_ALL      (KEY_LED_K3 | KEY_LED_K2 | KEY_LED_K7 | KEY_LED_K6 | KEY_LED_K5 | KEY_LED_K4 | KEY_LED_K9 | KEY_LED_K8 | KEY_LED_K1 | KEY_LED_KSH | KEY_LED_K0 | KEY_LED_KST | KEY_LED_KK | KEY_LED_RD)
// 숫자만
#define KEY_LED_NUMALL                (KEY_LED_K2 | KEY_LED_K7 | KEY_LED_K6 | KEY_LED_K5 | KEY_LED_K4 | KEY_LED_K9 | KEY_LED_K8 | KEY_LED_K1 | KEY_LED_KSH | KEY_LED_K0 | KEY_LED_KST | KEY_LED_KK)


/*
  ADC_DATA_K5,
  ADC_DATA_K4,
  ADC_DATA_K9,
  ADC_DATA_K8,
*/

// define FlagEvent
#define EVENT_FLAG_UART1_485        0x00000001

// define flag for gu16Flag1
#define FLAG1                       gu16Flag1.Val
#define FLAG_EVT_UART_RX1           gu16Flag1.bits.b0
#define FLAG_UART_RX1               gu16Flag1.bits.b1
#define FLAG_UART_TX1               gu16Flag1.bits.b2
#define FLAG_EVT_INPUT              gu16Flag1.bits.b3
#define FLAG_MANUAL_STEP            gu16Flag1.bits.b4
#define FLAG_ADC_START              gu16Flag1.bits.b5
#define FLAG_ADC_FINISH             gu16Flag1.bits.b6


