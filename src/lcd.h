#ifndef	_LCD_H
#define	_LCD_H

#include "hardware.h"

#define MAX_ROWS 4
#define MAX_COLS 18

#define VATTR_NONE       0
#define VATTR_INVERSE    1
#define VATTR_UNDERLINE  2
//#define VATTR_BLINK      3

//void _LCD_SendCmd(unsigned char data);

void LCD_Text(U8 page,char *pstr);
void LCD_TextXY(U8 x,U8 y,char *pstr,U8 attr);

void _TextXY(U8 x,U8 y,char *pstr,U8 attr);
void _SetTextAttrXY(U8 x,U8 y,char *pstr,U8 attr);

void LCD_Update(void *ptr);
void LCD_CopyVram(void *ptr);
void LCD_CopyVramAndClear(void *ptr);
void LCD_Rect(U8 x,U8 y, U8 h, U8 w);
void LCD_ScreenRect(void);
void LCD_MemToScreen(U8 *ptr, U8 page);

void LCD_Clear(void);
void _ClearScreen(void);


//void LCD_PowerOn(void);
U8 LCD_PowerOn(void);
void LCD_PowerOff(void);
void LCD_Open(void);
void LCD_Close(void);
void LCD_StartLine(U8 line);

void LCD_SetPixel(U8 x,U8 y,U8 val);
void LCD_PaintVram(void);

#endif
