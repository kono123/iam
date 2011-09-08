//#include <hardware.h>
#include <vcl.h>
#include <lcd.h>
#include <kbd.h>
#include <utils.h>

#include <stdio.h>
#include <string.h>

#define INPUTBOX_STR_POS 4

char _ibstr[MAX_COLS];
int  _ibpos;
const char _ibCancelStr[]="Отмена";
const char     _ibOkStr[]="  Ок  ";
///////////////////////////////////////////////////////////////////////////
//                                                            InputeBox  //
///////////////////////////////////////////////////////////////////////////
void InputBoxKbdHandler(unsigned char key)
{
  switch(key){

    case KBD_0:
    case KBD_1:
    case KBD_2:
    case KBD_3:
    case KBD_4:
    case KBD_5:
    case KBD_6:
    case KBD_7:
    case KBD_8:
    case KBD_9:
      if(_ibstr[_ibpos]){
        _ibstr[_ibpos]=key;
        _ibpos++;
        if(_ibstr[_ibpos]){
          if(_ibstr[_ibpos]!=0xBF) _ibpos++;
        }
        _TextXY(INPUTBOX_STR_POS,2,_ibstr,VATTR_NONE);
        _SetTextAttrXY(INPUTBOX_STR_POS+_ibpos,2,"0",MBATTR_INVERSE);
        LCD_Update(NULL);
      }
    break;

    case KBD_ARROW_H:
        if(_ibpos){
          _SetTextAttrXY(INPUTBOX_STR_POS+_ibpos,2,"0",VATTR_NONE);
          _ibpos--;
          if((_ibstr[_ibpos]<KBD_0) | (_ibstr[_ibpos]>KBD_9)){
//            if(_ibstr[_ibpos]!=0x20) _ibpos--;
            if(_ibpos) _ibpos--;
          }
          _ibstr[_ibpos]=0xBF;
          _TextXY(INPUTBOX_STR_POS,2,_ibstr,VATTR_NONE);
          _SetTextAttrXY(INPUTBOX_STR_POS+_ibpos,2,"0",MBATTR_INVERSE);
//          _TextXY(4,1,_ibstr,VATTR_NONE);
          LCD_Update(NULL);
        }
    break;

    case KBD_OK:
      VCL_SetRetVal(VCL_RET_OK);
      VCL_FreeMutex();
    break;

    case KBD_CANCEL:
      VCL_SetRetVal(VCL_RET_CANCEL);
      VCL_FreeMutex();
    break;

  }
}

int InputBox(const char *Title, char *iostr, int flag)
{
  char ss[MAX_COLS+1];
  int _vrmb[MAX_ROWS*MAX_COLS];
  FKBDHANDLER prvkbdh;
  int _mutex;
  int i,ret;

  _mutex=VCL_SetMutex();

  _ibpos=0;
  LCD_CopyVram(_vrmb);
  StrLimitLen((char*)Title,ss,MAX_COLS-2);
//  for(i=0;i<(MAX_COLS-2);i++){
//    ss[i]=Title[i];
//    if(Title[i]==0) break;
//  }
//  ss[i]=0;
  LCD_ScreenRect();
  _TextXY(1,0,ss,VATTR_NONE);
  StrToTemplate(_ibstr,iostr,flag);
  if(strlen(iostr)){
//    _ibstr[0]=0;
    _TextXY(INPUTBOX_STR_POS,1,iostr,VATTR_NONE);
    _TextXY(INPUTBOX_STR_POS,2,_ibstr,VATTR_NONE);
    _SetTextAttrXY(INPUTBOX_STR_POS,2,"0",MBATTR_INVERSE);
  }
  LCD_Update(NULL);
  prvkbdh=DISPATCH_SetKbdHandler(InputBoxKbdHandler);

  VCL_WaitMutex(_mutex);
  ret=VCL_GetRetVal();

  if(ret==VCL_RET_OK){
    for(i=0;i<14;i++){
      if(_ibstr[i]==0xBF){
        _ibstr[i]=0;
        break;
      }
    }
    strcpy(iostr,_ibstr);
  }
  LCD_Update(_vrmb);
  DISPATCH_SetKbdHandler(prvkbdh);
  return ret;
}
