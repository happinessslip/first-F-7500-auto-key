/**************************************************************
*																*
*	Module   	: my_generic.h									*
*	Compile	 	: cmsis os ( GCC )								*
*																*
*	CPU			 : STM32F103C8				    				*
*																*
*	Date	  	: 2012.	03.02	- Ver0.0						*
*				  2020. 12.06.  - 								*
*	                  											*
*	Designer 	: Peace 										*
***************************************************************/
/*
	For compiler
*/

#ifndef _GENERIC_HEADER
#define _GENERIC_HEADER

// typedef unsigned char BOOL;
//     #define FALSE 0x00
//     #define TRUE 0x01

// typedef signed long      sDWORD;
// typedef signed short     sWORD;
// typedef signed char      sBYTE;

// typedef uint8_t BYTE;                 // 8-bit

// typedef uint16_t WORD;            // 16-bit

// typedef uint32_t DWORD;                    // 32-bit

typedef union _BYTE_VAL
{
    struct
    {
        unsigned int b0:1;
        unsigned int b1:1;
        unsigned int b2:1;
        unsigned int b3:1;
        unsigned int b4:1;
        unsigned int b5:1;
        unsigned int b6:1;
        unsigned int b7:1;
    } bits;
    uint8_t Val;
} BYTE_VAL;

typedef union _WORD_VAL
{
    uint8_t v[2];
    uint16_t Val;
    //int Vint;
    struct
    {
        uint8_t LSB;
        uint8_t MSB;
    } byte;
    struct
    {
        unsigned int b0:1;
        unsigned int b1:1;
        unsigned int b2:1;
        unsigned int b3:1;
        unsigned int b4:1;
        unsigned int b5:1;
        unsigned int b6:1;
        unsigned int b7:1;
        unsigned int b8:1;
        unsigned int b9:1;
        unsigned int b10:1;
        unsigned int b11:1;
        unsigned int b12:1;
        unsigned int b13:1;
        unsigned int b14:1;
        unsigned int b15:1;
     } bits;
} WORD_VAL;

#define LSB(a)          ((a).v[0])
#define MSB(a)          ((a).v[1])

//======================================
//: Define struct
//======================================


typedef union _FLOAT_VAL
{
    float Val;
    uint8_t v[4];
} FLOAT_VAL;


typedef union _DWORD_VAL
{
    uint32_t Val;
    
    struct
    {
        uint8_t LOLSB;
        uint8_t LOMSB;
        uint8_t HILSB;
        uint8_t HIMSB;
    } byte;
    struct
    {
        uint16_t LSW;
        uint16_t MSW;
    } word;
    uint8_t v[4];
    struct
    {
        unsigned int b0:1;
        unsigned int b1:1;
        unsigned int b2:1;
        unsigned int b3:1;
        unsigned int b4:1;
        unsigned int b5:1;
        unsigned int b6:1;
        unsigned int b7:1;
        unsigned int b8:1;
        unsigned int b9:1;
        unsigned int b10:1;
        unsigned int b11:1;
        unsigned int b12:1;
        unsigned int b13:1;
        unsigned int b14:1;
        unsigned int b15:1;
        unsigned int b16:1;
        unsigned int b17:1;
        unsigned int b18:1;
        unsigned int b19:1;
        unsigned int b20:1;
        unsigned int b21:1;
        unsigned int b22:1;
        unsigned int b23:1;
        unsigned int b24:1;
        unsigned int b25:1;
        unsigned int b26:1;
        unsigned int b27:1;
        unsigned int b28:1;
        unsigned int b29:1;
        unsigned int b30:1;
        unsigned int b31:1;
    } bits;
} DWORD_VAL;
/*
typedef union _DWORD_VAL
{
    unint32_t Val;
    uint16_t W[2];
    uint8_t v[4];
} DWORD_VAL;
*/
#define LOWER_LSB(a)    ((a).v[0])
#define LOWER_MSB(a)    ((a).v[1])
#define UPPER_LSB(a)    ((a).v[2])
#define UPPER_MSB(a)    ((a).v[3])



#endif
