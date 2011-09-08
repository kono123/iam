//#include <lvtypes.h>
#include <vcl.h>
#include <kbd.h>

#include <stdio.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////
//                                                             Kbdblock  //
///////////////////////////////////////////////////////////////////////////

void KbdBlockKbdHandler(U8 key)
{
  switch(key){

//    case KBD_1:
    case KBD_OK:
      VCL_SetRetVal(VCL_RET_OK);
      VCL_FreeMutex();
    break;

    case 0:
    case KBD_CANCEL:
      SetSilenceFlag(1);
//      VCL_SetRetVal(VCL_RET_CANCEL);
      VCL_FreeMutex();
    break;
  }
}

int Kbdblock(U8 key)
{
  char ss[18];
  int _vrmb[MAX_ROWS*MAX_COLS];
  FKBDHANDLER prvkbdh;
  int _mutex,ret;

  _mutex=VCL_SetMutex();
  LCD_CopyVram(_vrmb);
  prvkbdh=DISPATCH_SetKbdHandler(KbdBlockKbdHandler);

  SetSilenceDone();
  LCD_ScreenRect();
  _TextXY(1,0,"Клавиатура",VATTR_NONE);
  _TextXY(1,1,"заблокирована",VATTR_NONE);
//  sprintf(ss,"нажмите <1> %x",key);
  sprintf(ss,"нажмите <Ввод>");
  _TextXY(1,2,ss,VATTR_NONE);
  LCD_Update(NULL);
  VCL_SetRetVal(VCL_RET_CANCEL);
  VCL_SetTimer(40);

  VCL_WaitMutex(_mutex);

  ret=VCL_GetRetVal();

  DISPATCH_SetKbdHandler(prvkbdh);
  LCD_Update(_vrmb);
  return ret;
}
