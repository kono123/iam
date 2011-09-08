#include "hardware.h"
#include "iam.h"
#include "kbd.h"
#include <lcd.h>

#include <stdio.h>

#define MSECOND_CYCLE 8000

__no_init TGscanConfig _gconf @ 0x1040;      //Segment C
__no_init TCalibrRRecord _gcalibr @ 0x1000;  //Segment D
__no_init TCalibrPHRecord _gphcalibrbd[MAX_RECORD_PH_PROBE] @ 0x7C00; //Segment B

///////////////////////////////////////////////////////////////////
//          KBD
///////////////////////////////////////////////////////////////////
void KBD_Init(void)
{
  P2DIR |= 0x0F;
  P2OUT |= 0x0F;

//  P2IE |= 0x0F0;                            // interrupt enabled
//  P2IES |= 0x0F0;                           // Hi/Lo edge
//  P2IFG &= ~0x0F0;                          // IFG cleared
}

void KBD_Detach(void)
{
  P2OUT &= ~0x0F;
  P2DIR &= ~0x0F;
}

void KBD_EnableInterrupt(void)
{
  P2IE  |= 0x0F0;                            // interrupt enabled
  P2IES |= 0x0F0;                           // Hi/Lo edge
  P2IFG &= ~0x0F0;                          // IFG cleared
}

void KBD_DisableInterrupt(void)
{
  P2IE  &= ~0x0F0;
}

void KBD_ClrInterrupt(void)
{
  P2IE  &= ~0x0F0;
}

U8 KBD_scan(void)
{
  U8 line,i;

  for(i=0;i<4;i++){
    P2OUT &= ~(1<<i);
    if(P2IN & 16){
      if(P2IN & 32){
        if(P2IN & 64){
          if(P2IN & 128){
            line=0;
          }else{
            line=4;
          }
        }else{
          line=3;
        }
      }else{
        line=2;
      }
    }else{
      line=1;
    }
    P2OUT |= 1<<i;
    if(line){
      return i*4+line;
    }
  }
  return 0;
}

///////////////////////////////////////////////////////////////////
//          SPI
///////////////////////////////////////////////////////////////////

#define B1_MOSI 1
#define B1_MISO 2
#define B1_SCK  3

void SPI_Init(void)
{
  P5SEL |= 0x0E;                    //Primary peripheral
  P5DIR |= (1<<B1_MOSI) | (1<<B1_SCK);
  P5DIR &= ~(1<<B1_MISO);

  UCB1CTL1 |= UCSWRST; // reset

  UCB1CTL0 = UCMSB + UCMST + UCSYNC; //+ UCCKPL+ UCCKPH
  UCB1CTL1 |= 0xC0;                 // SMCLK

  UCB1BR0 = 0;
  UCB1BR1 = 0;

  UCB1CTL1 &= ~UCSWRST;
}

void SPI_Close(void)
{
  P3SEL &= ~0x0E;                    //Primary peripheral
//  P5DIR &= ~((1<<B1_MOSI) | (1<<B1_SCK));
  P3DIR &= ~((1<<1) | (1<<3));
}

unsigned char SPI_PROC(unsigned char data)
{
  UCB1TXBUF = data;
  while(UCB1STAT & UCBUSY);
  return UCB1RXBUF;
}

#ifndef SD_Present
int SD_Present(void)
{
  if(P2IN & 0x40){
    return 0;
  }else{
    return 1;
  }
}
#endif
///////////////////////////////////////////////////////////////////
//          MUX
///////////////////////////////////////////////////////////////////
void MUX_Open(void)
{
  P1DIR |= 0x1C;
  P6SEL = 0x1F; //0x3F;
  R_Reset_Diactivate();
  R_CSPWR_Activate(); //
  SPI_PROC(0);
  R_CSPWR_Diactivate();
  R_CSGND_Activate(); //
  SPI_PROC(0);
  R_CSGND_Diactivate();
}

void MUX_Close(void)
{
  P1DIR &= ~0x1C;
}

///////////////////////////////////////////////////////////////////
//          ADC
///////////////////////////////////////////////////////////////////

void ADC_Open(void)
{
//  P1DIR |= 0x1C;
//  P6SEL = 0x1F; //0x3F;

  ADC12CTL0 &= ~ENC; //Disable Conversion
  ADC12CTL0 = ADC12ON | SHT0_5 | REF2_5V | REFON;
  ADC12CTL1 = ADC12SSEL0 | CONSEQ_0 | ADC12SSEL1 | SHP | ADC12DIV_0; // SMCLK
  ADC12MCTL0 = SREF_1 | 0;             // pH1
  ADC12MCTL1 = SREF_1 | INCH0;         // pH2
  ADC12MCTL2 = SREF_1 | INCH1;         // pH3
  ADC12MCTL3 = SREF_1 | INCH0 | INCH1; // R
  ADC12MCTL4 = SREF_1 | INCH2;         // Bat
  ADC12MCTL5 = SREF_1 | INCH0 | INCH2; // Base
  ADC12MCTL6 = SREF_1 | INCH1 | INCH3; //Temperature diode

//  ADC12CTL0 |= ENC; //Enable Conversion

//  R_Reset_Diactivate();
//  R_CSPWR_Activate(); //
//  SPI_PROC(0);
//  R_CSPWR_Diactivate();
//  R_CSGND_Activate(); //
//  SPI_PROC(0);
//  R_CSGND_Diactivate();
}

void ADC_Close(void)
{
  ADC12CTL0 &= ~ADC12ON; //ADC Off
  ADC12CTL0 &= ~ENC; //Disable Conversion
//  P1DIR &= ~0x1C;
}

///////////////////////////////////////////////////////////////////
//          LCD
///////////////////////////////////////////////////////////////////

void _LCD_SendData(unsigned char data)
{
  for(register int i=0;i<8;i++){
    if(data & 0x80){
      LCD_SI_HI();
    }else{
      LCD_SI_LO();
    }
    LCD_SCL_LO();
    data <<= 1;
    LCD_SCL_HI();
  }
}

void _LCD_SendCmd(unsigned char data)
{
  LCD_CS_Activate();
  _LCD_SendData(data);
  LCD_CS_Diactivate();
}

void _LCD_Disconnect(void)
{
  P4DIR &= ~0x03;  //SI,SCL
  P5DIR &= ~0xF0;
}

void _LCD_Connect(void)
{
  P4DIR |= 0x03;  //SI,SCL
  P5DIR |= 0xF0;
}

///////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////

void delay_ms(U16 msec)
{
  while(msec--) __delay_cycles(MSECOND_CYCLE);
}

void dmemcpy(unsigned int *dist,unsigned int *src,unsigned int words)
{
  while(DMA0CTL & DMAREQ);
  DMA0SA = (void __data20*)src;                  // Start block address
  DMA0DA = (void __data20*)dist;                       // Destination block address
  DMA0SZ = words;                          // Block size
  DMA0CTL = DMADT_3 + DMASRCINCR_3 + DMADSTINCR_3; // Rpt, inc
  DMA0CTL |= DMAEN;                         // Enable DMA0

  DMA0CTL |= DMAREQ;                      // Trigger block transfer
}

void StartSysTimer(U16 cr0)
{
//  TACTL = TASSEL_2 + MC_1;    // Run to CCR0 with SMCLK, Up mode
  TACTL = TASSEL_2 + MC_1 + ID_3;    // Run to CCR0 with SMCLK, Up mode /8
  TACCR0 = (SM_CLK >> 3) / cr0;
  TACCTL0 = CCIE;             // Enable CCR0 int.
}

void StartKbdTimer(void)
{
  TBCTL = TBSSEL_2 + MC_1 + ID_3;    // Run to CCR0 with ACLK, Up mode /8
//  TBCCR0 = (A_CLK >> 3) / 100;
  TBCCR0 = (SM_CLK >> 3) / 16;
  TBCCTL0 = CCIE;             // Enable CCR0 int.
}

#define R_ERROR_POWER_3V 0x01

#define _POWER_3V_TIMEOUT 2000

U8 POWER_3V_On(void)
{
  U16 timeout=_POWER_3V_TIMEOUT;
  POWER_3V_ON();
  delay_ms(100);
  while(timeout--){
    if(P1IN & (1<<5)){
      delay_ms(500);
      return 0;
    }
    delay_ms(50);
  }
  POWER_3V_OFF();
  return R_ERROR_POWER_3V;
}

void POWER_3V_Off(void)
{
  POWER_3V_OFF();
}

void SleepDevice(void)
{
  P2DIR |= 0x0F;                        // Set P1.0-3 to output direction
  P2OUT &= 0xF0;

  __disable_interrupt();
  KBD_EnableInterrupt();
  __enable_interrupt();
  __low_power_mode_4();  // Enter LPM4
}

//#define INIT_PORT_1() {P1SEL = (unsigned char)0x0; P1OUT = (unsigned char)0x40; P1DIR = (unsigned char)0xFF;}

void PinPowerOff(void)
{
  P4DIR &= ~0x03;
  P5DIR &= ~0xF0;
}

void PinPowerOn(void)
{
  P4DIR |= 0x03;
  P5DIR |= 0xF0;
}

int InitHardware(void)
{
  int i;

  WDTCTL = WDTPW + WDTHOLD;             // Stop watchdog timer

  P1DIR |= 0x01;                        // CS RTC

  P3SEL = 0x0;
//  P3OUT &= 0x20;
  P3OUT &= ~0x10;
  P3DIR |= 0x10;                        // ON1
  P3OUT &= ~0x20;
  P3DIR |= 0x20;                        // Set P3.5 to output direction TP1

  P4SEL = 0x0;
  P5SEL = 0x0;

//  P1OUT |= 0x18;
//  P1DIR |= 0x18; // CS_MX1 CS_MX2

  P5OUT |= 0x01;
  P5DIR |= 0x01; // CS_MEM

//Clock init

//  P5DIR |= 0x10;                            // P5.4= output direction
//  P5SEL |= 0x10;                            // P5.4= MCLK option select

  asm("BIS.W #0x0020,SR"); //OSCOFF, XT1 - off

  BCSCTL1 &= ~XT2OFF;                       // Activate XT2 high freq xtal
  BCSCTL3 |= XT2S_2;                        // 3 – 16MHz crystal or resonator
  BCSCTL3 |= LFXT1S_2;                      // VLO
//  IE1 |= OFIE;                              // Enable osc fault interrupt

// Wait until XT2 is stable
  do{
    IFG1 &= ~OFIFG;                         // Clear OSCFault flag
    for (i = 0xFF; i > 0; i--);             // Time for flag to set
  }while (IFG1 & OFIFG);                    // OSCFault flag still set?

  BCSCTL2 |= SELM_2 + SELS;                 // MCLK = XT2 HF XTAL (safe),SMCLK -> XT2

#ifdef TP_ENA
  // Test Points
//  P6DIR |= 0xE0;                        // Set P6.5-7 to output direction TP2,TP3,TP4
//  P6OUT |= 0xE0;

  P3OUT |= 0x20;
  P3DIR |= 0x20;// TP1
  P6OUT |= 0xc0;
  P6DIR |= 0xc0;// TP2,TP3,TP4
#endif

  SPI_Init();
  _gconf.Tag;
  _gcalibr.Tag;
  _gphcalibrbd[0].Type;

  return 0;
}
/*
#pragma vector=NMI_VECTOR
__interrupt void NMI_ISR(void)
{
  volatile unsigned int i;

  BCSCTL2 &= ~SELM_3;                       // Ensure MCLK runs from DCO
  do
  {
    IFG1 &= ~OFIFG;                         // Clear OSCFault flag
    for (i = 0xFF; i > 0; i--);             // Time for flag to set
  }
  while (IFG1 & OFIFG);                     // OSCFault flag still set?

  BCSCTL2 |= SELM_2 + SELS;                 // MCLK = XT2 HF XTAL (safe),SMCLK -> XT2

}
*/