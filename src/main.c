//******************************************************************************
//******************************************************************************
#include <hardware.h>
#include <iam.h>
#include <vcl.h>
#include <journal.h>
#include <ts_func.h>
#include <disk_sd.h>

#include <stdio.h>
#include <string.h>

U8 UART_dummy;

volatile U8 _counter;

extern TGscanConfig _gconf;
extern TCalibrRRecord _gcalibr;
extern TIamResearchDataConfig _gdconf;

U8 fstest(void);
void BlueTooth_Init(void);
void BlueTooth_PowerOn(void);

const char _FwVersion[]="02.01";
const char _NameDevice[]="»¿Ã-01";

void Update(void)
{
  TCalibrRRecord cr;
  TGscanConfig __gconf;
  int flag=0;

  memcpy(&__gconf,&_gconf,sizeof(TGscanConfig));

  if(_gcalibr.Tag!=TAG_IAM_CALIBR){
    memcpy(&cr,&_gcalibr,sizeof(_gcalibr));
    cr.Tag=TAG_IAM_CALIBR;
    cr.Bytes=60;
    write_SegD((U8*)&cr,0,sizeof(_gcalibr));
printf("Update calibr - %x\n",_gcalibr.Tag);
  }

  if(_gconf.Tag!=TAG_GSCAN_CONFIG){
    __gconf.Tag=TAG_GSCAN_CONFIG;
    __gconf.Bytes=sizeof(__gconf)-4;
    strcpy(__gconf.NameDevice,_NameDevice);
    strcpy(__gconf.FwVersion,_FwVersion);
    __gconf.IoVersion=1;
    strcpy(__gconf.SerNumDevice,"000B000");
    write_SegC((U8*)&__gconf,0,sizeof(__gconf));
printf("Update TAG 0x%x\n",__gconf.Tag);
  }else{
    if(strcmp(_gconf.FwVersion,_FwVersion)!=0){
      strcpy(__gconf.FwVersion,_FwVersion);
printf("Update version - %s\n",__gconf.FwVersion);
      flag=1;
    }
    if(strcmp(_gconf.NameDevice,_NameDevice)!=0){
      strcpy(__gconf.NameDevice,_NameDevice);
printf("Update name - %s\n",__gconf.FwVersion);
      flag=1;
    }
    if(flag){
      write_SegC((U8*)&__gconf,0,sizeof(__gconf));
    }
  }
}
/*
unsigned int testconst[16] = {
  0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
  0x101, 0x102, 0x103, 0x104, 0x105, 0x106, 0x107, 0x108
};
*/
int main(void)
{
//  int i,b[8],s[8];

//  __disable_interrupt();
  U8 data;

  InitHardware();

  Update();

  DISK_Init();

  __enable_interrupt();

  DISPATCH_Open();

  SleepDevice();

  DISPATCH_Task();

//  while(1){
//    DISPATCH_Task();
//  }
}

