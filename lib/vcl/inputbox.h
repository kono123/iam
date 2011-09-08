#ifndef	_INPUTBOX_H
#define	_INPUTBOX_H

//#include <hw_r.h>

#define INPUTBOX_STR       1
#define INPUTBOX_NUM       2
#define INPUTBOX_TIME      3
#define INPUTBOX_DATE      4

int InputBox(const char *Title, char *iostr, int flag);

#endif
