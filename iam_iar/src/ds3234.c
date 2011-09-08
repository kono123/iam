#include "hardware.h"

void _ds1390_read(U8 addr, U8 bytes, U8 *buff)
{
//  U8 ret;
  RTC_CS_Activate();
  SPI_PROC(addr);
  while(bytes--){
    *buff++=SPI_PROC(0xff);
  }
  RTC_CS_Diactivate();
}

void _ds1390_write(U8 addr, U8 bytes, U8 *buff)
{
//  U8 ret;
  RTC_CS_Activate();
  SPI_PROC(addr | 0x80);
  while(bytes--){
    SPI_PROC(*buff++);
  }
  RTC_CS_Diactivate();
}

void _ds3234_SRAM_write(U8 addr, U8 bytes, U8 *buff)
{
//  U8 ret;
  RTC_CS_Activate();
  SPI_PROC(0x98);
  SPI_PROC(addr);
  RTC_CS_Diactivate();
  while(bytes--){
    RTC_CS_Activate();
    SPI_PROC(0x99);
    SPI_PROC(*buff++);
    RTC_CS_Diactivate();
  }
}

void _ds3234_SRAM_read(U8 addr, U8 bytes, U8 *buff)
{
//  U8 ret;
  RTC_CS_Activate();
  SPI_PROC(0x98);
  SPI_PROC(addr);
  RTC_CS_Diactivate();
  while(bytes--){
    RTC_CS_Activate();
    SPI_PROC(0x19);
    *buff++=SPI_PROC(0xff);
    RTC_CS_Diactivate();
  }
}

int _ds3234_Temperature(void)
{
  int ret;
  RTC_CS_Activate();
  SPI_PROC(0x11);
  ret = SPI_PROC(0xff)<<8;
  SPI_PROC(0x12);
  ret |= SPI_PROC(0xff);
  RTC_CS_Diactivate();
  return ret;
}

void _ds1390_setCtrl(U8 data)
{
  RTC_CS_Activate();
  SPI_PROC(0x8e);
  SPI_PROC(data);
  RTC_CS_Diactivate();
}

U8 _ds1390_getCtrl(void)
{
  U8 data;
  RTC_CS_Activate();
  SPI_PROC(0x0e);
  data = SPI_PROC(0xff);
  RTC_CS_Diactivate();
  return data;
}

void _ds1390_init(void)
{
  RTC_CS_OUTPUT();
  RTC_CS_Diactivate();
}

void _ds1390_close(void)
{
  RTC_CS_INPUT();
}
