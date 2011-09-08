//#include <hardware.h>
#include <vcl.h>
#include <lcd.h>
#include <kbd.h>
#include <design.h>

#include <stdio.h>

///////////////////////////////////////////////////////////////////////////
//                                                                 Menu  //
///////////////////////////////////////////////////////////////////////////
TMENU const *_curmenu;
TMENU const *_prevmenu;
int        *_mindex;

void MENU_Show(void)
{
  int i;
  _ClearScreen();
  for(i=0;i<_curmenu->Items; i++){
    _TextXY(0,i,MENU_ICON,VATTR_NONE);
    if(*_mindex==i){
      _TextXY(1,i,(char*)_curmenu->Item[i].Caption,MEMU_ATTR_INVERSE);
    }else{
      _TextXY(1,i,(char*)_curmenu->Item[i].Caption,VATTR_NONE);
    }
  }
  LCD_Update(NULL);
}

void MenuKbdHandler(U8 key)
{
  switch(key){

    case KBD_ARROW_V:
      _SetTextAttrXY(1,*_mindex,(char*)_curmenu->Item[*_mindex].Caption,VATTR_NONE);
      *_mindex +=1;
      if(*_mindex>=_curmenu->Items) *_mindex=0;
      _SetTextAttrXY(1,*_mindex,(char*)_curmenu->Item[*_mindex].Caption,MEMU_ATTR_INVERSE);
      LCD_Update(NULL);
    break;

    case KBD_ARROW_H:
    case KBD_OK:
      if(_curmenu->Item[*_mindex].MenuHandler){
        _curmenu->Item[*_mindex].MenuHandler();
      }
    break;

    case KBD_CANCEL:
      VCL_FreeMutex();
    break;
  }
}

int Menu(TMENU const *pmenu)
{
  FKBDHANDLER prvkbdh;
  int _mutex;
  int _index;
  int *previndex;
  TMENU const *prevmenu;

  _mutex=VCL_SetMutex();
  prevmenu=_curmenu;
  previndex=_mindex;
  _curmenu = pmenu;
  _mindex=&_index;
  _index=0;
  MENU_Show();
  prvkbdh=DISPATCH_SetKbdHandler(MenuKbdHandler);
  VCL_WaitMutex(_mutex);
  _mindex=previndex;
  _curmenu=prevmenu;
  if(_curmenu) MENU_Show();
  DISPATCH_SetKbdHandler(prvkbdh);
  return VCL_GetRetVal();
}

void MainMenuKbdHandler(U8 key)
{
  switch(key){

    case KBD_ARROW_V:
      _SetTextAttrXY(1,*_mindex,(char*)_curmenu->Item[*_mindex].Caption,VATTR_NONE);
      *_mindex +=1;
      if(*_mindex>=_curmenu->Items) *_mindex=0;
      _SetTextAttrXY(1,*_mindex,(char*)_curmenu->Item[*_mindex].Caption,MEMU_ATTR_INVERSE);
      LCD_Update(NULL);
    break;

    case KBD_ARROW_H:
    case KBD_OK:
      if(_curmenu->Item[*_mindex].MenuHandler){
        _curmenu->Item[*_mindex].MenuHandler();
      }
    break;

    case KBD_ON_REP:
      VCL_FreeMutex();
    break;
  }
}

int StartMainMenu(TMENU const *pmenu)
{
  FKBDHANDLER prvkbdh;
  int _mutex;
  int _index;

  _mutex=VCL_SetMutex();
  _curmenu = pmenu;
  _prevmenu=0;
  _mindex=&_index;
  _index=0;
  MENU_Show();
  prvkbdh=DISPATCH_SetKbdHandler(MainMenuKbdHandler);
  VCL_WaitMutex(_mutex);
  DISPATCH_SetKbdHandler(prvkbdh);
  return VCL_GetRetVal();
}

