//#include "hardware.h"
#include "rtc.h"
#include "ds3234.h"

#include <stdio.h>
/*
const char _monthstr[12][4]={
  "янв","фев","мар","апр","май","июн",
  "июл","авг","сен","окт","ноя","дек"
};

const char _monthstr[12][4]={
  "jen","feb","mar","apr","may","jun",
  "jul","aug","sen","оct","nov","dec"
};
*/
void RTC_Init(void)
{
  _ds1390_init();
}

void RTC_setDMY(U8 day,U8 month,U8 year)
{
  U8 buf;
  buf=((day / 10)<<4) | (day % 10);
  _ds1390_write(DS_DATE_OFFSET,1,&buf);
  buf=((month / 10)<<4) | (month % 10);
  _ds1390_write(DS_MONTH_OFFSET,1,&buf);
  buf=((year / 10)<<4) | (year % 10);
  _ds1390_write(DS_YEAR_OFFSET,1,&buf);
}

void RTC_setHMS(U8 hour,U8 minute,U8 sec)
{
  U8 buf;
  buf=((sec / 10)<<4) | (sec % 10);
  _ds1390_write(DS_SEC_OFFSET,1,&buf);
  buf=((minute / 10)<<4) | (minute % 10);
  _ds1390_write(DS_MIN_OFFSET,1,&buf);
  buf=((hour / 10)<<4) | (hour % 10);
  _ds1390_write(DS_HOUR_OFFSET,1,&buf);
}

U16 RTC_getYear(void)
{
  U8 ret;
  _ds1390_read(DS_YEAR_OFFSET,1,&ret);
  ret=((ret & 0xf0)>>4)*10+(ret & 0x0f);
  return 2000+ret;
}

U8 RTC_getMonth(void)
{
  U8 ret;
  _ds1390_read(DS_MONTH_OFFSET,1,&ret);
  ret=((ret & 0x10)>>4)*10+(ret & 0x0f);
  return ret;
}

U8 RTC_getDay(void)
{
  U8 ret;
  _ds1390_read(DS_DATE_OFFSET,1,&ret);
  ret=((ret & 0x30)>>4)*10+(ret & 0x0f);
  return ret;
}

void RTC_WriteDword(U32 val)
{
  _ds1390_write(8,4,(U8*)&val);
}

U32 RTC_ReadDword(void)
{
  U32 ret;
  _ds1390_read(8,4,(U8*)&ret);
  return ret;
}

void RTC_getDateStr(char* str)
{
  U8 d,m,y;
  Tds1390 t;
  _ds1390_read(DS_SEC_OFFSET,sizeof(t),(U8*)&t);
  d=((t.Date & 0xf0)>>4)*10+(t.Date & 0x0f);
  m=((t.Month & 0xf0)>>4)*10+(t.Month & 0x0f);
  y=((t.Year & 0xf0)>>4)*10+(t.Year & 0x0f);
  sprintf(str,"%02d-%02d-%02d",d,m,y);
//  sprintf(str,"%02d%s%02d",d,_monthstr[m],y);
}

U8 RTC_getSecond(void)
{
  U8 ret;
  _ds1390_read(DS_SEC_OFFSET,1,&ret);
  ret=((ret & 0xf0)>>4)*10+(ret & 0x0f);
  return ret;
}

U8 RTC_getMinute(void)
{
  U8 ret;
  _ds1390_read(DS_MIN_OFFSET,1,&ret);
  ret=((ret & 0xf0)>>4)*10+(ret & 0x0f);
  return ret;
}

U8 RTC_getHour(void)
{
  U8 ret;
  _ds1390_read(DS_HOUR_OFFSET,1,&ret);
  ret=((ret & 0x30)>>4)*10+(ret & 0x0f);
  return ret;
}

void RTC_getTimeStr(char* str)
{
  U8 sec,m,h;
  Tds1390 t;
  _ds1390_read(DS_SEC_OFFSET,sizeof(t),(U8*)&t);
  sec=((t.Second & 0xf0)>>4)*10+(t.Second & 0x0f);
  m=((t.Minute & 0xf0)>>4)*10+(t.Minute & 0x0f);
  h=((t.Hour & 0xf0)>>4)*10+(t.Hour & 0x0f);
  sprintf(str,"%02d:%02d:%02d",h,m,sec);
}

void RTC_getTimeStrHM(char* str)
{
  U8 m,h;
  Tds1390 t;
  _ds1390_read(DS_SEC_OFFSET,sizeof(t),(U8*)&t);
  m=((t.Minute & 0xf0)>>4)*10+(t.Minute & 0x0f);
  h=((t.Hour & 0xf0)>>4)*10+(t.Hour & 0x0f);
  sprintf(str,"%02d:%02d",h,m);
}

const U8 dayperyear[30]=
{
  0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,7
};

const U16 daypermonth[12]=
{
  0,31,59,90,120,151,181,212,243,273,304,334
};

const U16 daypermonth_[12]=
{
  31,28,31,30,31,30,31,31,30,31,30,31
};

const U16 daypermonth4[12]=
{
  0,31,60,91,121,152,182,213,244,274,305,335
};

U32 RTC_getDateTime(void)
{
//  U16 y;
  U8 y,m,d,h,mi,s;
  U32 ret;
  Tds1390 t;

  _ds1390_read(DS_SEC_OFFSET,sizeof(t),(U8*)&t);
  s=((t.Second & 0xf0)>>4)*10+(t.Second & 0x0f);
  mi=((t.Minute & 0xf0)>>4)*10+(t.Minute & 0x0f);
  h=((t.Hour & 0xf0)>>4)*10+(t.Hour & 0x0f);
  d=((t.Date & 0xf0)>>4)*10+(t.Date & 0x0f);
  m=((t.Month & 0xf0)>>4)*10+(t.Month & 0x0f);
  y=((t.Year & 0xf0)>>4)*10+(t.Year & 0x0f);

  ret  = y;
  ret *= 365;
  ret += dayperyear[y];
  if(y % 4){
    ret += daypermonth4[m-1];
  }else{
    ret += daypermonth[m-1];
  }
  ret += d-1;
  ret *= 24;
  ret += h;
  ret *= 60;
  ret += mi;
  ret *= 60;
  ret += s;
  return ret;
}

void RTC_DateTimeDMYToStr(U32 dt,char* str)
{
  U32 d;
//  U32 tt;
  U8 y=0;
  U8 m=0;
  U8 dd;

  d = dt / (3600L*24L);
  dd=0;
  while(d>(365+dd)){
    d -=365;
    if((y!=0) && ((y % 4)==0)) d--;
    y++;
    dd=0;
    if((y!=0) && ((y % 4)==0)) dd=1;
  }
  dd=0;
  while(d>(daypermonth_[m]+dd)){
    d -= daypermonth_[m];
    if((m==1) && ((y % 4)==0)) d--;
    m++;
    if(m==1) dd=((y % 4)==0)?1:0;
  }
  m++;
  sprintf(str,"%02d-%02d-%02d",(U8)d,m,y);
}

U8 RTC_GetDaysPerMonth(U8 m,U8 y)
{
  if((y!=0) && ((y % 4)==0) && (m==2)){
    return daypermonth_[m-1]+1;
  }else{
    return daypermonth_[m-1];
  }
}
/*
U8 RTC_GetDaysPerMonth_(U32 dt)
{
  U32 d;
//  U32 tt;
  U8 y=0;
  U8 m=0;
  U8 dd;

  d = dt / (3600L*24L);
  dd=0;
  while(d>(365+dd)){
    d -=365;
    if((y!=0) && ((y % 4)==0)) d--;
    y++;
    dd=0;
    if((y!=0) && ((y % 4)==0)) dd=1;
  }
  dd=0;
  while(d>(daypermonth_[m]+dd)){
    d -= daypermonth_[m];
    if((m==1) && ((y % 4)==0)) d--;
    m++;
    if(m==1) dd=((y % 4)==0)?1:0;
  }
  m++;
  return daypermonth_[m];
}
*/
