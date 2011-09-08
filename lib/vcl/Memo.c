//#include <lvtypes.h>
#include <vcl.h>
#include <kbd.h>

#include <stdio.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////
//                                                                 Memo  //
///////////////////////////////////////////////////////////////////////////

static const char *ptext;

void Memo_Show(void)
{
  int i;
  const char *pp;
  _ClearScreen();
  pp=ptext;
  for(i=0;i<4; i++){
    _TextXY(0,i,MEMO_ICON,VATTR_NONE);
    _TextXY(1,i,(char*)pp,VATTR_NONE);
    pp+=strlen(pp)+1;
  }
  LCD_Update(NULL);
}


void MemoKbdHandler(unsigned char key)
{
  switch(key){

    case KBD_ARROW_V:
      ptext+=strlen(ptext)+1;
      Memo_Show();
    break;

    case KBD_CANCEL:
    case KBD_OK:
      VCL_FreeMutex();
    break;

  }
}

int Memo(const char *text,int flag)
{
  int _vrmb[MAX_ROWS*MAX_COLS];
  FKBDHANDLER prvkbdh;
  int _mutex;

  _mutex=VCL_SetMutex();
  LCD_CopyVram(_vrmb);
  ptext=text;
  prvkbdh=DISPATCH_SetKbdHandler(MemoKbdHandler);
  Memo_Show();
  VCL_WaitMutex(_mutex);
  LCD_Update(_vrmb);
  DISPATCH_SetKbdHandler(prvkbdh);
  return VCL_RET_OK;
}
