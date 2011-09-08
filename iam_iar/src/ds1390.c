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

void _ds1390_setCtrl(U8 data)
{
  _ds1390_write(0x0d,1,&data);
}

U8 _ds1390_getCtrl(void)
{
  U8 buf;
  _ds1390_write(0x0d,1,&buf);
  return buf;
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
