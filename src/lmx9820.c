#include <hardware.h>
#include <lmx9820.h>

#include <stdio.h>

#define bitSCL 1
#define bitSI 0
#define bitCS 6
#define bitA0 7

#define LCD_CS_Activate()          PORT_LO(5,bitCS)
#define LCD_CS_Diactivate()        PORT_HI(5,bitCS)
#define LCD_A0_LO()                PORT_LO(5,bitA0)
#define LCD_A0_HI()                PORT_HI(5,bitA0)


void BT_Init(void)
{
//  P4DIR |= 0x03;                        // Set P1.5 to output direction TP1
//  P5DIR |= 0x0F;                        // Set P6.5-7 to output direction TP2,TP3,TP4


  printf("BlueTooth Init!!\n");

}
