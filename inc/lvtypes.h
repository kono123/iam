#ifndef	_LVTYPES_H_
#define	_LVTYPES_H_

#ifdef	__ICCAVR__
 #if __HAS_ELPM__
  #define	CODESPACE __farflash
 #else
  #define	CODESPACE __flash
 #endif
 #include <pgmspace.h>
#else
 #define	CODESPACE
 #define strcmp_P strcmp
 #define memcmp_P memcmp
 #define memcpy_P memcpy
#endif

#ifndef BASETYPES

typedef char CHAR;
typedef CHAR * PCHAR;

typedef unsigned char       BYTE;
typedef unsigned short      WORD;

typedef unsigned char UCHAR;
typedef UCHAR * PUCHAR;

#define PUCHAR_P UCHAR CODESPACE *

typedef unsigned short USHORT;
typedef USHORT * PUSHORT;

typedef unsigned long ULONG;
typedef ULONG * PULONG;

typedef ULONG DWORD;

typedef void VOID;
typedef VOID * PVOID;

#endif

#ifndef	__ICCAVR__
#define PGM_P PCHAR
#endif

#ifndef SU_MULTITASKING
typedef unsigned char BOOLEAN;
#endif

#ifndef FALSE
#define	FALSE 0
#define	TRUE 1
#endif

#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))
#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))

#ifndef MAKELONG
#define MAKELONG(a, b)      ((ULONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#endif

#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define SWAP(w)             (MAKEWORD(HIBYTE(w), LOBYTE(w)))

#define LTOBE(x) MAKELONG(MAKEWORD(HIBYTE(HIWORD(x)), LOBYTE(HIWORD(x))), MAKEWORD(HIBYTE(LOWORD(x)), LOBYTE(LOWORD(x))))
#define WTOBE(x) MAKEWORD(HIBYTE(x), LOBYTE(x))

#ifndef	NULL
#define NULL (0)
#endif

typedef unsigned char U8;
typedef signed char I8;
typedef unsigned short U16;
typedef signed short I16;
typedef unsigned long U32;
typedef signed long I32;

#endif

