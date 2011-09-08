//#include <hardware.h>
#include <lvtypes.h>
#include <vcl.h>
#include <kbd.h>

#include <stdio.h>

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
///////////////////////////////////////////////////////////////////////////
//                                                           Wizard      //
///////////////////////////////////////////////////////////////////////////
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
