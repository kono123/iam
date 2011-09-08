#include "vcl.h"
#include "kbd.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////
//                                                              EnumBox  //
///////////////////////////////////////////////////////////////////////////

#define ENUMBOX_STR_POS 2

static int _enretval;
//static int _enretval,_enindex;
static int _enlistindex,_enlistitems;
const char _enCancelStr[]="Отмена";
const char     _enOkStr[]="Ок";
const char     _enTitle[]="Выбор";
unsigned char _max_len_items,x_pos;
char *enlist[MAX_ENUM_LIST_ITEMS];

void EnumBoxKbdHandler(unsigned char key)
{
  switch(key){

    case KBD_ARROW_V:
      _enlistindex++;
      if(_enlistindex>=_enlistitems) _enlistindex=0;
//      _TextXY(2,1,"              ",VATTR_NONE);
      _TextXY(x_pos,2,enlist[_enlistindex],ENATTR_INVERSE);
      LCD_Update(NULL);
    break;
/*
    case KBD_ARROW_H:
      if(_enindex){
        _enindex=0;
        _enretval = ENUM_BOX_CANCEL;
        _SetTextAttrXY(9,2,(char*)_enCancelStr,ENATTR_INVERSE);
        _SetTextAttrXY(4,2,(char*)_enOkStr,VATTR_NONE);
      }else{
        _enindex=1;
        _enretval = ENUM_BOX_OK;
        _SetTextAttrXY(9,2,(char*)_enCancelStr,VATTR_NONE);
        _SetTextAttrXY(4,2,(char*)_enOkStr,ENATTR_INVERSE);
      }
      LCD_Update(NULL);
    break;
*/
    case KBD_OK:
      _enretval = ENUM_BOX_OK;
      VCL_FreeMutex();
    break;

    case KBD_CANCEL:
      _enretval = ENUM_BOX_CANCEL;
      VCL_FreeMutex();
    break;
  }
}

int EnumBox(char *Title,U8 *initval,U8 items, ...)
{
  char ss[MAX_COLS+1];
  int _vrmb[MAX_ROWS*MAX_COLS];
  FKBDHANDLER prvkbdh;
  int i;
  int _mutex;
  U8 len,x;
  va_list ap;

  _mutex=VCL_SetMutex();
  LCD_CopyVram(_vrmb);
  _enlistindex=*initval;
  _enretval = VCL_RET_CANCEL;
  _enlistitems=items;
  _max_len_items=0;
  va_start(ap, items);      // Variable argument begin
  for(i=0;i<_enlistitems;i++){
    enlist[i]=va_arg(ap,char*);
    len=strlen(enlist[i]);
    if(_max_len_items<len){
      _max_len_items=len;
    }
  }
  va_end(ap);                // Variable argument end

  for(i=0;i<(MAX_COLS-2);i++){
    ss[i]=Title[i];
    if(Title[i]==0) break;
  }
  ss[i]=0;
  LCD_ScreenRect();
  _TextXY(1,0,(char*)_enTitle,0);
  _TextXY(1,1,ss,0);
//  _SetTextAttrXY(2,2,enlist[ii],ENATTR_INVERSE);
  x_pos=(MAX_COLS-_max_len_items-1) >> 1;
  _TextXY(x_pos,2,enlist[_enlistindex],ENATTR_INVERSE);
  _TextXY(x_pos+_max_len_items+1,2,"\x8E",VATTR_NONE);
/*  if(_max_len_items<10){
    _TextXY(2,2,enlist[_enlistindex],ENATTR_INVERSE);
    _TextXY(2+_max_len_items+1,2,"\x8E",VATTR_NONE);
  }else{
    _TextXY(1,2,enlist[_enlistindex],ENATTR_INVERSE);
    _TextXY(1+_max_len_items+1,2,"\x8E",VATTR_NONE);
  }*/
  prvkbdh=DISPATCH_SetKbdHandler(EnumBoxKbdHandler);
  LCD_Update(NULL);
  VCL_WaitMutex(_mutex);
  LCD_Update(_vrmb);
  if(_enretval==ENUM_BOX_OK) *initval=_enlistindex;
  DISPATCH_SetKbdHandler(prvkbdh);
  return _enretval;
}
