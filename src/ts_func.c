#include <hardware.h>
#include <iam.h>
#include <timeslot.h>
#include <dispatch.h>

#include <stdio.h>

extern U8 kbd_scan_code;
//U16 repeatcount;
U16 Batery = 0x0FFF;

void tsReserv(PStageRec sr)
{
  return;
}

void tsViewTime(PStageRec sr)
{
  U8 sec,m,h;
  char ss[32];
  sec=RTC_getSecond();
  m=RTC_getMinute();
  h=RTC_getHour();
  sprintf(ss,"%02d:%02d:%02d",h,m,sec);
  LCD_Text(3,ss);
//  _TextXY(sr->Param1,sr->Param2,ss,0);
}

void tsBatery(PStageRec sr)
{
  Batery=(ADC_Batery()+3*Batery)>>2;
//  Batery=ADC_Batery();
  if(Batery<POWER_ALARM_CODE)
    SendMsg(ALARM_POWER_EVENT,Batery,0);
}


