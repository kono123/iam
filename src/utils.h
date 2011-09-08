#ifndef	_UTILS_H
#define	_UTILS_H

#include <lvtypes.h>

U8  StrToU8(char *str);
U16 StrToU16(char *str);
void StrToTemplate(char *d,char *s,int param);
int GetStrToDelimiter(char *s,char *head,char *tail);
void StrLimitLen(char *source, char *dist, U8 len);

void WriteToAddBuff(unsigned int *ptr, unsigned int words);
void ClearAddBuff(void);
unsigned int GetAddBuff(int index);
U16 GetCounterAddBuff(void);

#endif
