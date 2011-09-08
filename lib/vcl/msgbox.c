//#include <hardware.h>
#include <vcl.h>
#include <kbd.h>

#include <stdio.h>

///////////////////////////////////////////////////////////////////////////
//                                                           MessageBox  //
///////////////////////////////////////////////////////////////////////////

static int _mbindex;
const char _mbCancelStr[]="Отмена";
const char     _mbOkStr[]="Ок";

void MessageBoxKbdHandlerOkCancel(U8 key)
{
  switch(key){

    case KBD_ARROW_H:
      if(_mbindex){
        _mbindex=0;
        VCL_SetRetVal(VCL_RET_CANCEL);
        _SetTextAttrXY(9,2,(char*)_mbCancelStr,MBATTR_INVERSE);
        _SetTextAttrXY(4,2,(char*)_mbOkStr,VATTR_NONE);
      }else{
        _mbindex=1;
        VCL_SetRetVal(VCL_RET_OK);
        _SetTextAttrXY(9,2,(char*)_mbCancelStr,VATTR_NONE);
        _SetTextAttrXY(4,2,(char*)_mbOkStr,MBATTR_INVERSE);
      }
      LCD_Update(NULL);
    break;

    case KBD_OK:
      VCL_FreeMutex();
    break;

    case 0:
    case KBD_CANCEL:
      VCL_SetRetVal(VCL_RET_CANCEL);
      VCL_FreeMutex();
    break;
  }
}

int MessageBox(char *Title,int flag)
{
  char ss[MAX_COLS+1];
  int _vrmb[MAX_ROWS*MAX_COLS];
  FKBDHANDLER prvkbdh;
  int i;
  int _mutex;
  int ret;

  _mutex=VCL_SetMutex();
  LCD_CopyVram(_vrmb);
  _mbindex=0;
//  _mbretval = MSGBOX_CANCEL;
  VCL_SetRetVal(VCL_RET_CANCEL);
  for(i=0;i<15;i++){
    ss[i]=Title[i];
    if(Title[i]==0) break;
  }
  ss[i+1]=0;
  LCD_ScreenRect();
  _TextXY(1,1,ss,0);
  if(flag){
    _TextXY(1,0,MSGBOX_OKCANCEL_ICON,VATTR_NONE);
    _TextXY(4,2,(char*)_mbOkStr,VATTR_NONE);
    _TextXY(9,2,(char*)_mbCancelStr,MBATTR_INVERSE);
    LCD_Update(NULL);
    prvkbdh=DISPATCH_SetKbdHandler(MessageBoxKbdHandlerOkCancel);
  }else{
    _TextXY(1,0,MSGBOX_OK_ICON,VATTR_NONE);
    _TextXY(7,2,(char*)_mbOkStr,MBATTR_INVERSE);
    LCD_Update(NULL);
    prvkbdh=DISPATCH_SetKbdHandler(OkKbdHandler);
  }
  VCL_WaitMutex(_mutex);
  LCD_Update(_vrmb);
  ret=VCL_GetRetVal();
  DISPATCH_SetKbdHandler(prvkbdh);
  return ret;
}

int MessageBoxEx(char *Title,char *str2,int flag)
{
  char ss[MAX_COLS+1];
  int _vrmb[MAX_ROWS*MAX_COLS];
  FKBDHANDLER prvkbdh;
  int i;
  int _mutex;
  int ret;

  _mutex=VCL_SetMutex();
  LCD_CopyVram(_vrmb);
  LCD_ScreenRect();
  if(flag){
    _TextXY(1,0,MSGBOX_OKCANCEL_ICON,VATTR_NONE);
  }else{
    _TextXY(1,0,MSGBOX_OK_ICON,VATTR_NONE);
  }

  for(i=0;i<16;i++){
    ss[i]=Title[i];
    if(Title[i]==0) break;
  }
  ss[i+1]=0;
  _TextXY(1,1,ss,0);
  for(i=0;i<16;i++){
    ss[i]=str2[i];
    if(str2[i]==0) break;
  }
  ss[i+1]=0;
  _TextXY(1,2,ss,0);
  prvkbdh=DISPATCH_SetKbdHandler(OkCancelKbdHandler);
  LCD_Update(NULL);
  VCL_WaitMutex(_mutex);
  LCD_Update(_vrmb);
  ret=VCL_GetRetVal();
  DISPATCH_SetKbdHandler(prvkbdh);
  return ret;
}
///////////////////////////////////////////////////////////////////////////
//                                                             TimerBox  //
///////////////////////////////////////////////////////////////////////////

static int _sec,_secbegin;
//const char _mbCancelStr[]="??????";
//const char     _mbOkStr[]="??";

void TimerBoxKbdHandler(U8 key)
{
  char ss[18];

  switch(key){

    case KBD_TIMER:
      if(_secbegin<_sec){
        VCL_SetRetVal(VCL_RET_OK);
        VCL_FreeMutex();
      }else{
        sprintf(ss,"Ждем %03d сек ",_secbegin-_sec);
        _TextXY(0,2,ss,0);
        LCD_Update(NULL);
        _sec++;
      }
    break;
//    case 0:
//      VCL_SetRetVal(VCL_RET_OK);
//      VCL_FreeMutex();
//    break;
    case KBD_CANCEL:
      VCL_SetRetVal(VCL_RET_CANCEL);
      VCL_FreeMutex();
    break;
  }
}

int WaitTimerBox(char *Title,int sec)
{
  char ss[MAX_COLS+1];
  int _vrmb[MAX_ROWS*MAX_COLS];
  FKBDHANDLER prvkbdh;
  int i;
  int _mutex;
  int ret;

  _sec=0;
  _secbegin=sec;

  _mutex=VCL_SetMutex();
  LCD_CopyVram(_vrmb);

  _ClearScreen();

  for(i=0;i<16;i++){
    ss[i]=Title[i];
    if(Title[i]==0) break;
  }
  ss[i+1]=0;
  _TextXY(0,1,ss,0);
  prvkbdh=DISPATCH_SetKbdHandler(TimerBoxKbdHandler);
  LCD_Update(NULL);
  VCL_WaitMutex(_mutex);
  LCD_Update(_vrmb);
  ret=VCL_GetRetVal();
  DISPATCH_SetKbdHandler(prvkbdh);
  if(ret!=VCL_RET_OK){
    MessageBoxEx("Прерванно","пользователем",MBF_OK);
  }
  return ret;
}

