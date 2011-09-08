#ifndef	_RTC_H
#define	_RTC_H

#include <lvtypes.h>

void RTC_Init(void);
U8 RTC_getSecond(void);
U8 RTC_getMinute(void);
U8 RTC_getHour(void);
U16 RTC_getYear(void);
U8 RTC_getMonth(void);
U8 RTC_getDay(void);

void RTC_setDMY(U8 day,U8 month,U8 year);
void RTC_setHMS(U8 hour,U8 minute,U8 sec);

void RTC_getDateStr(char* str);
void RTC_getTimeStr(char* str);
void RTC_getTimeStrHM(char* str);

U32 RTC_getDateTime(void);
void RTC_DateTimeDMYToStr(U32 dt,char* str);

void RTC_WriteDword(U32 val);
U32 RTC_ReadDword(void);
U8 RTC_GetDaysPerMonth(U8 m,U8 y);

#endif
