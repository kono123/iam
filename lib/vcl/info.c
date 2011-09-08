//#include <lvtypes.h>
#include <vcl.h>
#include <kbd.h>

#include <stdio.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////
//                                                                 Info  //
///////////////////////////////////////////////////////////////////////////

int Info(char *str1,char *str2,char *str3,char *str4)
{
  int _vrmb[MAX_ROWS*MAX_COLS];
  FKBDHANDLER prvkbdh;
  int _mutex;

  _mutex=VCL_SetMutex();
  LCD_CopyVram(_vrmb);

  _ClearScreen();
  _TextXY(0,0,str1,VATTR_NONE);
  _TextXY(0,1,str2,VATTR_NONE);
  _TextXY(0,2,str3,VATTR_NONE);
  _TextXY(0,3,str4,VATTR_NONE);
  LCD_Update(NULL);

  prvkbdh=DISPATCH_SetKbdHandler(OkKbdHandler);
  VCL_WaitMutex(_mutex);
  DISPATCH_SetKbdHandler(prvkbdh);
  LCD_Update(_vrmb);
  return VCL_RET_OK;
}
