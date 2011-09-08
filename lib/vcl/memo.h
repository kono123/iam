#ifndef	_MEMO_H
#define	_MEMO_H

//#include <hw_r.h>
#include <vcl.h>

#define MBF_OKCANCEL    1
#define MBF_OK          0

#define MSGBOX_OK       1
#define MSGBOX_CANCEL   2


#define MBATTR_INVERSE  VATTR_INVERSE
//#define MBATTR_INVERSE  VATTR_UNDERLINE

int Memo(const char *text,int flag);

#endif
