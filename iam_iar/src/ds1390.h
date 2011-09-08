#ifndef	_DS1390_H
#define	_DS1390_H

#include <hardware.h>

#define DS_SEC_OFFSET     1
#define DS_MIN_OFFSET     2
#define DS_HOUR_OFFSET    3
#define DS_DATE_OFFSET    5
#define DS_MONTH_OFFSET   6
#define DS_YEAR_OFFSET    7

typedef struct _buff_ds1390_tag {
  U8 Second;
  U8 Minute;
  U8 Hour;
  U8 Day;
  U8 Date;
  U8 Month;
  U8 Year;
} Tds1390, *Pds1390;

typedef struct _alrm_buff_ds1390_tag {
  U8 Second;
  U8 Minute;
  U8 Hour;
  U8 Day;
  U8 Date;
  U8 Month;
  U8 Year;
  char SerNumDevice[8];
} Tds1390Alarm, *Pds1390Alarm;


void _ds1390_init(void);

void _ds1390_read(U8 addr, U8 bytes, U8 *buff);
void _ds1390_write(U8 addr, U8 bytes, U8 *buff);

void _ds1390_setCtrl(U8 data);
void _ds1390_getCtrl(U8 data);

#endif
