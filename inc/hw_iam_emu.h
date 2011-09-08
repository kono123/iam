#ifndef	_HW_IAM_EMU_H
#define	_HW_IAM_EMU_H

#define _WIN32_WINNT 0x0400 //У меня 2000
#include <windows.h>
#include <process.h>
#include <windows.h>
#include <stdio.h>
#include <conio.h>

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

#define __disable_interrupt()
#define __enable_interrupt()

#define SM_CLK 8000000L
#define A_CLK  12000L

#define StopSysTimer() //TACCTL0 &= ~CCIE
#define BT_POWER_ON() //{P1DIR |= (1<<6);}

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
#define PH_ENABLE() 
#define PH_DISABLE() 
#define R_DISABLE() 

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

#define LCD_4_HI()
#define LCD_LIGTH_ON()             
#define LCD_LIGTH_OFF()            
#define LCD_CS_Activate()          
#define LCD_CS_Diactivate()        
#define LCD_SI_OUTPUT()            
#define LCD_SI_LO()                
#define LCD_SI_HI()                
#define LCD_SCL_OUTPUT()           
#define LCD_SCL_LO()               
#define LCD_SCL_HI()
void LCD_A0_LO(void);
void LCD_A0_HI(void);

// ADC
#define bitCS1 2
#define bitCS2 3
#define bitReset 4

#define R_Reset_Diactivate()       
#define R_Reset_Activate()         
#define R_CSPWR_Activate()         
#define R_CSPWR_Diactivate()       
#define R_CSGND_Activate()         
#define R_CSGND_Diactivate()       

// RTC
#define bitRTCCS 0

//#define RTC_CS_Activate()          
//#define RTC_CS_Diactivate()        
#define RTC_CS_INPUT()        
#define RTC_CS_OUTPUT()        

// EEPROM
#define EE_CS_Init()               
#define EE_CS_Activate()           
#define EE_CS_Diactivate()         

// SD
#define SD_CS_Init()              
#define SD_POWER_ON()              
#define SD_POWER_OFF()             
#define SD_CS_Activate()           
#define SD_CS_Diactivate()         

// POWER3V
#define POWER_3V_ON()              
#define POWER_3V_OFF()             

void StartSysTimer(U16 cr0);
void __low_power_mode_off_on_exit(void);

void IamWinProc(void);
unsigned IamWinInit(void);

#endif
