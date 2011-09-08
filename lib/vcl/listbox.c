#include <vcl.h>
#include <kbd.h>

#include <stdio.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////
//                                                              ListBox  //
///////////////////////////////////////////////////////////////////////////
char **_ptext;
unsigned char _indx;
unsigned char _indpos;
unsigned char _lines;

void ListBox_Show(void)
{
  int i,ii;
  _ClearScreen();
  ii=MAX_ROWS;
  if(_lines<ii) ii=_lines;
  for(i=0; i<ii; i++){
    _TextXY(0,i,LISTBOX_ICON,VATTR_NONE);
    if(i!=_indpos){
      _TextXY(1,i,(char*)_ptext[_indx+i],VATTR_NONE);
    }else{
      _TextXY(1,i,(char*)_ptext[_indx+i],VATTR_INVERSE);
    }
  }
  LCD_Update(NULL);
}

void ListBoxKbdHandler(unsigned char key)
{
  switch(key){

    case KBD_ARROW_V:
      _SetTextAttrXY(1,_indpos,(char*)_ptext[_indx+_indpos],VATTR_NONE);
      _indpos++;
      if(_lines<MAX_ROWS){
        if(_indpos>=_lines){
          _indx=_indpos=0;
        }
      }else{
        if(_indpos>=MAX_ROWS){
          _indpos--;
          _indx++;
          if((_indx+_indpos)>=_lines) _indx=_indpos=0;
        }
      }
      ListBox_Show();
    break;

    case KBD_CANCEL:
      VCL_SetRetVal(VCL_RET_CANCEL);
      VCL_FreeMutex();
    break;

    case KBD_OK:
      VCL_SetRetVal(VCL_RET_OK);
      VCL_FreeMutex();
    break;

  }
}

char *ListBox(char **text,U8 lines)
{
  int _vrmb[MAX_ROWS*MAX_COLS];
  FKBDHANDLER prvkbdh;
  int _mutex,ret;

  _mutex=VCL_SetMutex();
  LCD_CopyVram(_vrmb);
  _ptext=text;
  _indx=0;
  _lines=lines;
  _indpos=0;
  ListBox_Show();
  prvkbdh=DISPATCH_SetKbdHandler(ListBoxKbdHandler);
  VCL_WaitMutex(_mutex);
  ret=VCL_GetRetVal();
  LCD_Update(_vrmb);
  DISPATCH_SetKbdHandler(prvkbdh);
  if(ret==VCL_RET_CANCEL){
    return 0;
  }else{
    return _ptext[_indx+_indpos];
  }
}
