#include <vcl.h>
#include <lcd.h>
#include <kbd.h>

#include <stdio.h>

void ShowHourglass(void)
{
  _TextXY(8,2,"\xBA\xBB",VATTR_NONE);
  _TextXY(8,1,"\xBD\xBC",VATTR_NONE);
  LCD_Update(NULL);
}

void HideHourglass(void)
{
}

