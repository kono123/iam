#ifndef	_HW_IAM_H
#define	_HW_IAM_H

#include "msp430x26x.h"
//#include <io430.h>

typedef unsigned char   uint8_t;
typedef unsigned int    uint16_t;
typedef unsigned long   uint32_t;
typedef signed char     int8_t;
typedef signed int      int16_t;
typedef signed long     int32_t;

typedef float		float_t;

#define PORT_LO(port,bit)          {P ## port ## OUT &= ~(1<<bit);}
#define PORT_HI(port,bit)          {P ## port ## OUT |=  (1<<bit);}
#define PORT_SWITCH(port,bit)      {P ## port ## OUT ^=  (1<<bit);}
#define PORT_SET_INPUT(port,bit)   {P ## port ## DIR &= ~(1<<bit);}
#define PORT_SET_OUTPUT(port,bit)  {P ## port ## DIR |= (1<<bit);}

#define SM_CLK 8000000L
#define A_CLK  12000L

#define StopSysTimer() TACCTL0 &= ~CCIE
#define BT_POWER_ON() {P1DIR |= (1<<6);}

#ifdef TP_ENA
//#define TP1_HI() PORT_HI(3,5)
//#define TP1_LO() PORT_LO(3,5)
#define TP1_HI()
#define TP1_LO()
#define TP2_HI() PORT_HI(6,5)
#define TP2_LO() PORT_LO(6,5)
#define TP3_HI() PORT_HI(6,6)
#define TP3_LO() PORT_LO(6,6)
#define TP4_HI() PORT_HI(6,7)
#define TP4_LO() PORT_LO(6,7)
#else
#define TP1_HI()
#define TP1_LO()
#define TP2_HI()
#define TP2_LO()
#define TP3_HI()
#define TP3_LO()
#define TP4_HI()
#define TP4_LO()
#endif
#define PH_ENABLE() PORT_LO(3,5)
#define PH_DISABLE() PORT_HI(3,5)
#define R_DISABLE() ADC_ConnectR(0,0)

// LCD
//PORT5
#define bitCS 6
#define bitA0 7
#define bitLIGTH 5

//PORT4
#define portSI 4
#define bitSI 0
#define portSCL 4
#define bitSCL 1

#define LCD_4_HI()                 PORT_HI(5,4)
#define LCD_A0_LO()                PORT_LO(5,bitA0)
#define LCD_A0_HI()                PORT_HI(5,bitA0)
#define LCD_LIGTH_ON()             PORT_HI(5,bitLIGTH)
#define LCD_LIGTH_OFF()            PORT_LO(5,bitLIGTH)
#define LCD_CS_Activate()          PORT_LO(5,bitCS)
#define LCD_CS_Diactivate()        PORT_HI(5,bitCS)
#define LCD_SI_OUTPUT()            PORT_SET_OUTPUT(4,bitSI)
#define LCD_SI_LO()                PORT_LO(4,bitSI)
#define LCD_SI_HI()                PORT_HI(4,bitSI)
#define LCD_SCL_OUTPUT()            PORT_SET_OUTPUT(4,bitSCL)
#define LCD_SCL_LO()               PORT_LO(4,bitSCL)
#define LCD_SCL_HI()               PORT_HI(4,bitSCL)

// ADC
#define bitCS1 2
#define bitCS2 3
#define bitReset 4

#define R_Reset_Diactivate()       PORT_HI(1,bitReset)
#define R_Reset_Activate()         PORT_LO(1,bitReset)
#define R_CSPWR_Activate()         PORT_LO(1,bitCS1)
#define R_CSPWR_Diactivate()       PORT_HI(1,bitCS1)
#define R_CSGND_Activate()         PORT_LO(1,bitCS2)
#define R_CSGND_Diactivate()       PORT_HI(1,bitCS2)

// RTC
#define bitRTCCS 0

#define RTC_CS_Activate()          PORT_LO(1,bitRTCCS)
#define RTC_CS_Diactivate()        PORT_HI(1,bitRTCCS)
#define RTC_CS_OUTPUT()            PORT_SET_OUTPUT(1,bitRTCCS)
#define RTC_CS_INPUT()             PORT_SET_INPUT(1,bitRTCCS)

// EEPROM
#define EE_CS_Init()               {P5DIR |= 0x01; P5OUT |= 0x01;}
#define EE_CS_Activate()           PORT_LO(5,0)
#define EE_CS_Diactivate()         PORT_HI(5,0)

// SD
#define SD_CS_Init()              {P3DIR |= 0x01; P3OUT |= 0x01;}
#define SD_POWER_ON()              {PORT_HI(3,0); P1DIR |= 0x80; P3DIR |= 0x01; P1OUT |= 0x80; P3OUT |= 0x01;}
#define SD_POWER_OFF()             {P1OUT &= ~0x80; PORT_LO(3,0); P1DIR &= ~0x80; P3DIR &= ~0x01;}
#define SD_CS_Activate()           PORT_LO(3,0)
#define SD_CS_Diactivate()         PORT_HI(3,0)

// POWER3V
#define POWER_3V_ON()              {PORT_SET_OUTPUT(3,4); PORT_SET_OUTPUT(3,5);  PORT_HI(3,4);  PORT_HI(3,5);}
#define POWER_3V_OFF()             {PORT_LO(3,5);PORT_LO(3,4); PORT_SET_INPUT(3,4);}

void StartSysTimer(U16 cr0);

#endif
