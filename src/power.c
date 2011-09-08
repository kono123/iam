#include <hardware.h>

#include <stdio.h>

//PORT4
#define portSI 4
#define portSCL 4
#define bitSI 0
#define bitSCL 1

//PORT5
#define bitCS 6
#define bitA0 7

#define PORT_LO(port,bit)      {P ## port ## OUT &= ~(1<<bit);}
#define PORT_HI(port,bit)      {P ## port ## OUT |=  (1<<bit);}
#define PORT_SWITCH(port,bit)  {P ## port ## OUT ^=  (1<<bit);}



void Power_Init(void)
{
//  P4DIR |= 0x03;
//  P5DIR |= 0x0F;
//  P4SEL = 0x0;
//  P5SEL = 0x0;

//  PORT_HI(5,4);
//  PORT_HI(5,5);
//  LCD_CS_Diactivate();
//  LCD_A0_LO();

  printf("Hello LCD !!\n");
  LCD_WriteData(0xa5);
  LCD_WriteData(0x0F);
}
