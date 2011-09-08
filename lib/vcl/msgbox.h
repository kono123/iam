#ifndef	_MSGBOX_H
#define	_MSGBOX_H

//#include <hw_r.h>
#include <vcl.h>

#define MBF_OKCANCEL    1
#define MBF_OK          0

//#define MSGBOX_OK       1
//#define MSGBOX_CANCEL   2

#define MBATTR_INVERSE  VATTR_INVERSE
//#define MBATTR_INVERSE  VATTR_UNDERLINE

int MsgBox(char *Title,int flag);
int MessageBoxEx(char *Title,char *str2,int flag);
int WaitTimerBox(char *Title,int sec);
#endif
