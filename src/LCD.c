#include "hardware.h"
//#include "hwlcd.h"
#include "lcd.h"

#include <stdio.h>
#include <string.h>

#include <font.c>

//rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr
//rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr

// LCD commands
#define LCD_Reset()                  _LCD_SendCmd(0xE2)
#define LCD_AllPointsOn()            _LCD_SendCmd(0xC5)
#define LCD_AllPointsOff()           _LCD_SendCmd(0xC4)
#define LCD_DisplayOn()              _LCD_SendCmd(0xAF)
#define LCD_DisplayOff()             _LCD_SendCmd(0xAE)

#define LCD_SetStartLine(addr)       _LCD_SendCmd(0x40 | (addr & 0x3F))
#define LCD_SetRamPage(addr)         _LCD_SendCmd(0xB0 | (addr & 0xF))
#define LCD_SetRamColLo(addr)        _LCD_SendCmd(0x10 | (addr & 0xF))
#define LCD_SetRamColHi(addr)        _LCD_SendCmd(0x00 | (addr & 0xF))
#define LCD_SetRamCol(addr)         {_LCD_SendCmd(0x00 | ((addr >>4) & 0xF));_LCD_SendCmd(0x10 | (addr & 0xF));}

#define LCD_Bias7()                  _LCD_SendCmd(0xA2)
#define LCD_Bias9()                  _LCD_SendCmd(0xA3)
#define LCD_ComOutputNormal()        _LCD_SendCmd(0xC0)
#define LCD_ComOutputReverse()       _LCD_SendCmd(0xC8)

#define LCD_BoosterRatio(mode)      {_LCD_SendCmd(0xF8);_LCD_SendCmd(mode & 0x03);}
#define LCD_PowerControl(mode)       _LCD_SendCmd(0x28 | (mode & 0x07))

#define LCD_StaticIndicatorOn(mode) {_LCD_SendCmd(0xAD);_LCD_SendCmd(mode);}
#define LCD_StaticIndicatorOff()     _LCD_SendCmd(0xAC)
#define LCD_ElectronicVolume(mode)  {_LCD_SendCmd(0x81);_LCD_SendCmd((mode & 0x3F));}
#define LCD_VoltageRegulator(val)    _LCD_SendCmd(0x20 | (val & 0x3))

#define LCD_WriteData(data)      _LCD_SendCmd(1,data)

#define LCD_AdcSelectNormal()        _LCD_SendCmd(0xA0)
#define LCD_AdcSelectReverse()       _LCD_SendCmd(0xA1)
#define LCD_DisplayNormal()          _LCD_SendCmd(0xA6)
#define LCD_DisplayReverse()         _LCD_SendCmd(0xA7)

U8 videoram[MAX_ROWS][MAX_COLS<<1];

/*
void _LCD_WriteDataEx(unsigned int bytes,unsigned char *pdata)
{
  U8 data;

  LCD_CS_Activate();
  PORT_HI(5,bitA0);

  while(bytes--){
    data=*pdata++;
    _LCD_SendData(data);
  }

  PORT_LO(5,bitA0);
  LCD_CS_Diactivate();
}
*/
void LCD_DrawTextXY(U8 x,U8 y,char *pstr,U8 attr)
{
//  int i,j,w,h,n;
  int j,w,n;
  U8 data;

  LCD_SetRamCol(fontData[0]*x);
  LCD_SetRamPage(y);

  LCD_CS_Activate();
  LCD_A0_HI();
  w=fontData[0];
  while(*pstr){
    n=*pstr;
    for(j=0;j<w;j++){
      data=fontData[n*w+j+1];
        if(attr==VATTR_INVERSE){
          data=~data;
        }else if(attr==VATTR_UNDERLINE){
          data |= 0x80;
        }
      _LCD_SendData(data);
    }
    pstr++;
  }

  LCD_A0_LO();
  LCD_CS_Diactivate();
}

void LCD_Text(U8 page,char *pstr)
{
  LCD_DrawTextXY(0,page,pstr,0);
}


void _TextXY(U8 x,U8 y,char *pstr,U8 attr)
{
  U8 *p;
  p=(U8*)&videoram[y][x<<1];
  while(*pstr){
    *p++=attr;
    *p++=*pstr++;
  }
}

void _ClearScreen(void)
{
  memset(videoram,0x20,sizeof(videoram));
}

void _SetTextAttrXY(U8 x,U8 y,char *pstr,U8 attr)
{
  U8 *p;
  p=(U8*)&videoram[y][x<<1];
  while(*pstr++){
    *p++=attr;
    p++;
  }
}

void LCD_CopyVram(void *ptr)
{
  memcpy(ptr,videoram,sizeof(videoram));
}

void LCD_CopyVramAndClear(void *ptr)
{
  memcpy(ptr,videoram,sizeof(videoram));
  memset(videoram,0x20,sizeof(videoram));
}

void LCD_MemToScreen(U8 *ptr, U8 page)
{
  int i,j;

  for(i=0;i<4;i++){
    LCD_SetRamCol(0);
    LCD_SetRamPage(i+page);
    LCD_CS_Activate();
    LCD_A0_HI();
//    for(j=0;j<132;j++){
    for(j=0;j<128;j++){
        _LCD_SendData(*ptr++);
    }
    LCD_A0_LO();
    LCD_CS_Diactivate();
  }
}

void LCD_Update(void *ptr)
{
  int j,w,n,attr;
  int row,col;
  U8 data;

  if(ptr) memcpy(videoram,ptr,sizeof(videoram));

  w=fontData[0];
  for(row=0;row<4;row++){
    LCD_SetRamCol(0);
    LCD_SetRamPage(row);
    LCD_CS_Activate();
    LCD_A0_HI();
    for(col=0;col<36;col+=2){
      attr=videoram[row][col];
      n=videoram[row][col+1]*w+1;
      for(j=0;j<w;j++){
        data=fontData[n+j];
        if(attr==VATTR_INVERSE){
          data=~data;
        }else if(attr==VATTR_UNDERLINE){
          data |= 0x80;
        }
        _LCD_SendData(data);
      }
    }
    LCD_A0_LO();
    LCD_CS_Diactivate();
  }
}

void LCD_Rect(U8 x,U8 y, U8 h, U8 w)
{
}

void LCD_ScreenRect(void)
{
  _TextXY(0,0,"\x10\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x12",0);
  _TextXY(0,1,"\x17\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x13",0);
  _TextXY(0,2,"\x17\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x13",0);
  _TextXY(0,3,"\x16\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x14",0);
}
/*
void LCD_PaintVram(void)
{
  int i,j;
  U8 data,page;

  LCD_SetRamCol(0);
  LCD_SetRamPage(0);
  LCD_CS_Activate();

  PORT_HI(5,bitA0);
//  PORT_LO(4,bitSCL);
  for(i=0;i<VIDEO_RAM_SIZE;i++){
    data=videoram[i];
    for(j=0;j<8;j++){
      if(data & 0x80){
        PORT_HI(4,bitSI);
      }else{
        PORT_LO(4,bitSI);
      }
      PORT_LO(4,bitSCL);
      data <<= 1;
      PORT_HI(4,bitSCL);
    }
  }
  PORT_LO(5,bitA0);
  LCD_CS_Diactivate();
}

void LCD_SetPixel(U8 x,U8 y,U8 val)
{
  U8 page;
  if(x>=132) return;
  if(y>=32) return;
  page=y >> 3;
  if(val){
    videoram[page*132+x] |= 1<<(y & 0x7);
  }else{
    videoram[page*132+x] &= ~(1<<(y & 0x7));
  }
}
*/
void _LCD_ClearPage(U8 page)
{
  int bytes=132*8;
  LCD_SetRamPage(page);
  LCD_SetRamCol(0);
  LCD_CS_Activate();
  LCD_A0_HI();
  LCD_SI_LO();
  while(bytes--){
    LCD_SCL_LO();
    LCD_SCL_HI();
  }
  LCD_A0_LO();
  LCD_CS_Diactivate();
}

void LCD_Clear(void)
{
  int i;
  for(i=0;i<8;i++)
    _LCD_ClearPage(i);
}

U8 LCD_PowerOn(void)
{
  return POWER_3V_On();
}

void LCD_PowerOff(void)
{
  POWER_3V_Off();
}

void LCD_StartLine(U8 line)
{
  LCD_SetStartLine(line);
}

void LCD_Close(void)
{
  _LCD_Disconnect();
}

void LCD_Open(void)
{
  _LCD_Connect();

//  LCD_PowerOn();

  LCD_4_HI();
  LCD_LIGTH_ON();
  LCD_CS_Diactivate();
  LCD_A0_LO();

  LCD_Reset();
  delay_ms(10);
  LCD_SetStartLine(0);

  LCD_AdcSelectNormal();    // Hello
//  LCD_AdcSelectReverse();   // olleH
//  LCD_ComOutputNormal();
  LCD_ComOutputReverse();

  LCD_DisplayNormal();
  LCD_Bias7();
  LCD_PowerControl(7);
  LCD_BoosterRatio(0);
  LCD_VoltageRegulator(3);
  LCD_ElectronicVolume(0x1F);
  LCD_StaticIndicatorOn(1);
//  LCD_StaticIndicatorOff();

  LCD_Clear();
  LCD_DisplayOn();
  LCD_StartLine(0);

  memset(videoram,0,sizeof(videoram));
}
