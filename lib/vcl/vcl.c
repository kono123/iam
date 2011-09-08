//#include <hardware.h>
#include <vcl.h>
#include <lcd.h>
#include <kbd.h>

#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include <iam.h>

#define MSGBOX_OK_ICON        "\x21\x21"
#define MSGBOX_OKCANCEL_ICON  "\x3F\x3F"
#define ARROW_V_ICON          "\x8E"

extern U8 videoram[];
volatile int gvclmutex;
//volatile int gvclmutexval;
int gvclretval;
volatile int gvcltimer;

const char _VclCancelStr[]="Нет";
const char     _VclOkStr[]="Да";

//const char _mbCancelStr[]="Отмена";
//const char     _mbOkStr[]="Ок";

extern U16 Batery;

int VCL_Open(void)
{
  gvclmutex = 0;
  gvcltimer = 0;
//  gvclmutexval = 0;
  return 0;
}

typedef signed short TTimer;

int VCL_Close(void)
{
  return 0;
}
// Timer

extern U16 _gtimer;

void VCL_SetTimer(int val)
{
  _gtimer = val;
}
/*
void VCL_WaitTimer(void)
{
  while(gvcltimer)
    DISPATCH_Message();
}

void VCL_DecTimer(TTimer *pt)
{
  if(*pt) *pt--;
}
*/
void VCL_SetRetVal(int val)
{
  gvclretval = val;
}

int VCL_GetRetVal(void)
{
  return gvclretval;
}

int VCL_SetMutex(void)
{
  int tt;
  tt=gvclmutex++;
  return tt;
}

int VCL_FreeMutex(void)
{
  if(gvclmutex){
//    DISPATCH_SetKbdHandler(0);
    gvclmutex--;
  }
  return gvclmutex;
}

void VCL_WaitMutex(int mutex)
{
  while(gvclmutex>mutex)
    DISPATCH_Message();
}

void OkCancelKbdHandler(U8 key)
{
  switch(key){

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

void OkKbdHandler(U8 key)
{
  switch(key){

    case KBD_OK:
      VCL_SetRetVal(VCL_RET_OK);
      VCL_FreeMutex();
    break;
  }
}

void CancelKbdHandler(U8 key)
{
  if(key==KBD_CANCEL){
    VCL_SetRetVal(VCL_RET_CANCEL);
    VCL_FreeMutex();
  }
}


void IntProgressBarToStr(char *str,U8 len,U16 val,U16 maxval)
{
  U16 i;
  U32 d;
  str[0]=25;
  for(i=1;i<len;i++) str[i]=24;
  str[len-1]=179;
  str[len]=0;
  i=0;
  d=val;
  d=d*(7*len-2);
  d=d/maxval;
  while(d>=7){
    str[i++]=31;
    d -= 7;
  }
  d = d % 7;
  if(i<(len-1)){
    if(d){
      str[i]=d+24;
    }else{
      str[i]=25;
    }
  }else{
    str[i]=d+179;
  }
}

int IntProgressBar(const char *Title,U8 len)
{
  int i;
  float b;
  char ss[19];
  int _vrmb[MAX_ROWS*MAX_COLS];
  FKBDHANDLER prvkbdh;
  int _mutex;

  _mutex=VCL_SetMutex();
  LCD_CopyVram(_vrmb);

  for(i=0;i<(MAX_COLS);i++){
    ss[i]=Title[i];
    if(Title[i]==0) break;
  }
  ss[i+1]=0;
  _ClearScreen();
  _TextXY(0,1,ss,VATTR_NONE);

  b=Batery;
  sprintf(ss,"%4.2fB",b*2.5/0x1000);
  IntProgressBarToStr(ss+strlen(ss),len,Batery-POWER_ALARM_CODE,0x0FFF-POWER_ALARM_CODE);
  _TextXY(0,2,ss,VATTR_NONE);
  LCD_Update(NULL);

  prvkbdh=DISPATCH_SetKbdHandler(OkKbdHandler);

  VCL_WaitMutex(_mutex);
  DISPATCH_SetKbdHandler(prvkbdh);
  LCD_Update(_vrmb);
  return VCL_RET_OK;
}

///////////////////////////////////////////////////////////////////////////
//                                                           Wizard      //
///////////////////////////////////////////////////////////////////////////
static int _wiretval,_wiindex,_wiretvalParam;
int _wiitem;
TWizard const *_wiptr;

const char _wiStr[3][9]={
  ">>","Изменить","<<"
};

const U8 _xy[3][2]={
  {15,3},
  {6,3},
  {2,3},
};

void WizardKbdHandler(U8 key)
{
  char ss[MAX_COLS+1];

  switch(key){

    case KBD_ARROW_H:
      _SetTextAttrXY(_xy[_wiindex][0],_xy[_wiindex][1],(char*)_wiStr[_wiindex],VATTR_NONE);
      _wiindex++;
      if(_wiindex>=3) _wiindex=0;
      if(_wiindex==1){
        if(!_wiptr->Item[_wiitem].EditParam){
          _wiindex++;
        }
      }
      _SetTextAttrXY(_xy[_wiindex][0],_xy[_wiindex][1],(char*)_wiStr[_wiindex],MEMU_ATTR_INVERSE);
      LCD_Update(NULL);
    break;

    case KBD_OK:
      if(_wiindex==0){
        _wiretval = 1;
        VCL_FreeMutex();
      }else if(_wiindex==2){
        _wiretval = 0;
        VCL_FreeMutex();
      }else if(_wiindex==1){
        if(_wiptr->Item[_wiitem].EditParam(0)){
          _wiretvalParam=_wiptr->Item[_wiitem].GetParam(ss);
          _TextXY(0,2,"             ",0);
          _TextXY(0,2,ss,0);
          _SetTextAttrXY(_xy[_wiindex][0],_xy[_wiindex][1],(char*)_wiStr[_wiindex],VATTR_NONE);
          _wiindex=0;
          _SetTextAttrXY(_xy[_wiindex][0],_xy[_wiindex][1],(char*)_wiStr[_wiindex],MEMU_ATTR_INVERSE);
          LCD_Update(NULL);
        }
      }
    break;

    case KBD_CANCEL:
      _wiretval = WIZARD_CANCEL;
      VCL_FreeMutex();
    break;
  }
}

int Wizard(TWizard const *ptr,int item)
{
//  int _item;
  char ss[MAX_COLS+1];
  int _vrmb[MAX_ROWS*MAX_COLS];
  FKBDHANDLER prvkbdh;
  int _mutex;

  _mutex=VCL_SetMutex();
  prvkbdh=DISPATCH_SetKbdHandler(WizardKbdHandler);
  LCD_CopyVramAndClear(_vrmb);

  ShowHourglass();
  _ClearScreen();

  _wiitem=item;
  _wiptr=ptr;
  _wiindex=0;
  _wiretval=0;
  _wiretvalParam=1;

  _TextXY(0,0,(char*)(ptr->Item[item].Caption),0);
  _TextXY(0,1,(char*)(ptr->Item[item].Help),0);
  if(ptr->Item[item].GetParam){
    _wiretvalParam=ptr->Item[item].GetParam(ss);
    _TextXY(0,2,ss,0);
  }
  _TextXY(_xy[0][0],_xy[0][1],(char*)_wiStr[0],1);
  if(ptr->Item[_wiitem].EditParam){
    _TextXY(_xy[1][0],_xy[1][1],(char*)_wiStr[1],0);
//  }else{
//    _TextXY(_xy[1][0],_xy[1][1],"        ",0);
  }
  _TextXY(_xy[2][0],_xy[2][1],(char*)_wiStr[2],0);
  LCD_Update(NULL);

  VCL_WaitMutex(_mutex);

  DISPATCH_SetKbdHandler(prvkbdh);
  LCD_Update(_vrmb);

  if(_wiretval==1){
    if(_wiretvalParam){
      if(ptr->Item[item].ErrorParam){
        ptr->Item[item].ErrorParam();
        return item;
      }
      return item+1;
    }else{
      return item+1;
    }
  }
  if(_wiretval==0){
    return item-1;
  }
  return _wiretval;
}
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

///////////////////////////////////////////////////////////////////////////
//                                                            InputeBox  //
///////////////////////////////////////////////////////////////////////////
#define INPUTBOX_STR_POS 4

unsigned char _ibstr[MAX_COLS];
int  _ibpos;

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
          if(_ibstr[_ibpos]!=0xBF)
			  _ibpos++;
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

///////////////////////////////////////////////////////////////////////////
//                                                           MessageBox  //
///////////////////////////////////////////////////////////////////////////

static int _mbindex;

void MessageBoxKbdHandlerOkCancel(U8 key)
{
  switch(key){

    case KBD_ARROW_H:
      if(_mbindex){
        _mbindex=0;
        VCL_SetRetVal(VCL_RET_CANCEL);
        _SetTextAttrXY(9,2,(char*)_VclCancelStr,MBATTR_INVERSE);
        _SetTextAttrXY(4,2,(char*)_VclOkStr,VATTR_NONE);
      }else{
        _mbindex=1;
        VCL_SetRetVal(VCL_RET_OK);
        _SetTextAttrXY(9,2,(char*)_VclCancelStr,VATTR_NONE);
        _SetTextAttrXY(4,2,(char*)_VclOkStr,MBATTR_INVERSE);
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

int MsgBox(char *Title,int flag)
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
    _TextXY(4,2,(char*)_VclOkStr,VATTR_NONE);
    _TextXY(9,2,(char*)_VclCancelStr,MBATTR_INVERSE);
    LCD_Update(NULL);
    prvkbdh=DISPATCH_SetKbdHandler(MessageBoxKbdHandlerOkCancel);
  }else{
    _TextXY(1,0,MSGBOX_OK_ICON,VATTR_NONE);
    _TextXY(6,2,(char*)_VclOkStr,MBATTR_INVERSE);
    LCD_Update(NULL);
    prvkbdh=DISPATCH_SetKbdHandler(OkKbdHandler);
  }
  VCL_WaitMutex(_mutex);
  LCD_Update(_vrmb);
  ret=VCL_GetRetVal();
  DISPATCH_SetKbdHandler(prvkbdh);
  return ret;
}

///////////////////////////////////////////////////////////////////////////
//                                                               MsgBox  //
///////////////////////////////////////////////////////////////////////////
int MsgBoxEx(char *Title,char *str2,int flag)
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
    MsgBoxEx("Прерванно","пользователем",MBF_OK);
  }
  return ret;
}

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

///////////////////////////////////////////////////////////////////////////
//                                                              EnumBox  //
///////////////////////////////////////////////////////////////////////////

#define ENUMBOX_STR_POS 2

static int _enretval;
//static int _enretval,_enindex;
static int _enlistindex,_enlistitems;
//const char _enCancelStr[]="Отмена";
//const char     _enOkStr[]="Ок";
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
  _TextXY(x_pos+_max_len_items+1,2,ARROW_V_ICON,VATTR_NONE);
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

///////////////////////////////////////////////////////////////////////////
//                                                            Hourglass  //
///////////////////////////////////////////////////////////////////////////
void ShowHourglass(void)
{
  _TextXY(8,2,"\xBA\xBB",VATTR_NONE);
  _TextXY(8,1,"\xBD\xBC",VATTR_NONE);
  LCD_Update(NULL);
}

void HideHourglass(void)
{
}
