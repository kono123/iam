#include <hardware.h>

#include <stdio.h>

void ADC_ConnectR(U8 ch1,U8 ch2)
{
  R_CSPWR_Activate();
  SPI_PROC(0);
  R_CSPWR_Diactivate();

  R_CSGND_Activate();
  SPI_PROC(ch2);
  R_CSGND_Diactivate();

  R_CSPWR_Activate();
  SPI_PROC(ch1);
  R_CSPWR_Diactivate();
}

U16 ADC_R(U8 ch1,U8 ch2)
{
  U16 ret;

  R_CSPWR_Activate();
  SPI_PROC(0);
  R_CSPWR_Diactivate();

  R_CSGND_Activate();
  SPI_PROC(ch2);
  R_CSGND_Diactivate();

  R_CSPWR_Activate();
  SPI_PROC(ch1);
  R_CSPWR_Diactivate();

  delay(4*8+48);

//  ADC12CTL0 &= ~ENC; //Disable Conversion
  TP2_LO();
  ADC12CTL1 |= CSTARTADD_3;
  ADC12CTL0 |= ADC12SC | ENC; //Start Conversion
  while(ADC12CTL1 & ADC12BUSY);
  ADC12CTL1 &= ~CSTARTADD_3;
  TP2_HI();
  ret=ADC12MEM3;
//  R_Reset_Activate();
//  R_Reset_Diactivate();
//  ADC12CTL0 &= ~ENC; //Disable Conversion

//  R_CSPWR_Activate(); //
//  SPI_PROC(0);
//  R_CSPWR_Diactivate();

  return ret;
}

U16 ADC_R_(void)
{
  U16 ret;

  TP2_LO();
//  ADC12CTL0 &= ~ENC; //Disable Conversion
  ADC12CTL1 |= CSTARTADD_3;
  ADC12CTL0 |= ADC12SC | ENC; //Start Conversion
  __no_operation();
  while(ADC12CTL1 & ADC12BUSY);
//  while ((ADC12IFG & 0x08)==0);
  ADC12CTL0 &= ~ENC; //Disable Conversion
  ADC12CTL1 &= ~CSTARTADD_3;
  TP2_HI();
  ret=ADC12MEM3;
  return ret;
//  return ADC12MEM3;
}

U16 ADC_UBase(void)
{
  U16 ret;
  ADC12CTL1 |= CSTARTADD_3;
  ADC12CTL0 |= ADC12SC | ENC; //Start Conversion
  __no_operation();
  while(ADC12CTL1 & ADC12BUSY);
  ADC12CTL0 &= ~ENC; //Disable Conversion
  ADC12CTL1 &= ~CSTARTADD_3;
  ret=ADC12MEM5;
  return ret;
}

U16 ADC_Batery(void)
{
  U16 ret;
//  ADC12CTL0 &= ~ENC; //Disable Conversion
  ADC12CTL1 |= CSTARTADD_4;
  ADC12CTL0 |= ADC12SC | ENC; //Start Conversion
  __no_operation();
  while(ADC12CTL1 & ADC12BUSY);
  ADC12CTL0 &= ~ENC; //Disable Conversion
  ADC12CTL1 &= ~CSTARTADD_4;
  ret=ADC12MEM4;
  return ret;
//  return ADC12MEM4;
}

U16 ADC_pH(U8 ch)
{
  U16 ret;
//  ADC12CTL0 &= ~ENC; //Disable Conversion
  switch(ch){
    case 1:
      ADC12CTL1 |= CSTARTADD_0;
    break;
    case 2:
      ADC12CTL1 |= CSTARTADD_1;
    break;
    case 3:
      ADC12CTL1 |= CSTARTADD_2;
    break;
  }
//  ADC12CTL1 |= CSTARTADD_0;
  ADC12CTL0 |= ADC12SC | ENC; //Start Conversion
  __no_operation();
  while(ADC12CTL1 & ADC12BUSY);
  ADC12CTL0 &= ~ENC; //Disable Conversion
//  ADC12CTL1 &= ~CSTARTADD_0;
  switch(ch){
    case 1:
      ADC12CTL1 &= ~CSTARTADD_0;
      ret=ADC12MEM0;
    break;
    case 2:
      ADC12CTL1 &= ~CSTARTADD_1;
      ret=ADC12MEM1;
    break;
    case 3:
      ADC12CTL1 &= ~CSTARTADD_2;
      ret=ADC12MEM2;
    break;
  }
  return ret;
//  return ADC12MEM4;
}

U16 ADC_Temperature(void)
{
  U16 ret;
//  ADC12CTL0 &= ~ENC; //Disable Conversion
  ADC12CTL1 |= CSTARTADD_5;
  ADC12CTL0 |= ADC12SC | ENC; //Start Conversion
  __no_operation();
  while(ADC12CTL1 & ADC12BUSY);
  ADC12CTL0 &= ~ENC; //Disable Conversion
  ADC12CTL1 &= ~CSTARTADD_5;
  ret=ADC12MEM5;
  return ret;
}

U16 ADC_GetHold(void)
{
  U16 tmp;
//  ADC12CTL0 &= ~ENC; //Disable Conversion
  tmp = ADC12CTL0;
  tmp = (tmp >> 8) & 0x000F;
  return tmp;
}

void ADC_SetHold(U8 hold)
{
  U16 tmp;
//  ADC12CTL0 &= ~ENC; //Disable Conversion
  tmp = ADC12CTL0;
  tmp = tmp & 0xF0FF;
  ADC12CTL0 = tmp | ((hold & 0x0f)<<8);
}

