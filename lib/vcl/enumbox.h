#ifndef	_ENUMBOX_H
#define	_ENUMBOX_H

//#include <hw_r.h>
#include <vcl.h>

#define ENUM_BOX_OK       1
#define ENUM_BOX_CANCEL   -1

#define ENATTR_INVERSE  VATTR_INVERSE
//#define MBATTR_INVERSE  VATTR_UNDERLINE

#define MAX_ENUM_LIST_ITEMS 8

int EnumBox(char *Title,U8 *initval,U8 items, ...);

#endif
