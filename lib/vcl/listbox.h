#ifndef	_VCL_LISTBOX_H
#define	_VCL_LISTBOX_H

#include <vcl.h>

#define MBF_OKCANCEL    1
#define MBF_OK          0

#define MSGBOX_OK       1
#define MSGBOX_CANCEL   2


#define MBATTR_INVERSE  VATTR_INVERSE
//#define MBATTR_INVERSE  VATTR_UNDERLINE

int ListBox(const char *text,int flag);

#endif
