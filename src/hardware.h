#ifndef	_HARDWARE_H
#define	_HARDWARE_H

#include <lvtypes.h>

#ifdef IAM_EMU
#include <hw_iam_emu.h>
#else
#include <hw_iam2.h>
#endif

#include <lcd.h>
#include <flash.h>
#include <kbd.h>
#include <adc.h>
#include <rtc.h>

uint16_t UART1_TX_WriteByte(uint8_t ucDataByte);

void dmemcpy(unsigned int *dist,unsigned int *src,unsigned int words);

int InitHardware(void);
void delay_ms(U16 msec);
U8 POWER_3V_On(void);
void POWER_3V_Off(void);
void StartKbdTimer(void);
void SleepDevice(void);
void PinPowerOff(void);
void PinPowerOn(void);

void _LCD_SendData(unsigned char data);
void _LCD_SendCmd(unsigned char data);
void _LCD_Connect(void);
void _LCD_Disconnect(void);

void KBD_Init(void);
void KBD_EnableInterrupt(void);
void KBD_DisableInterrupt(void);

void SPI_Init(void);
void SPI_Close(void);
unsigned char SPI_PROC(unsigned char data);

void MUX_Open(void);
void MUX_Close(void);

void ADC_Open(void);
void ADC_Close(void);

#endif
