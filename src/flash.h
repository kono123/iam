#ifndef	_FLASH_H
#define	_FLASH_H

#include <lvtypes.h>

void read_SegB(U8 *ptr, U8 offset, U8 bytes);
void read_SegC(U8 *ptr, U8 offset, U8 bytes);
void read_SegD(U8 *ptr, U8 offset, U8 bytes);
void write_SegB(U8 *ptr, U8 offset, U8 bytes);
void write_SegC(U8 *ptr, U8 offset, U8 bytes);
void write_SegD(U8 *ptr, U8 offset, U8 bytes);
void write_flash(U8 *ptr, U32 addr_flash, U16 bytes);
void read_flash(U8 *ptr, U32 addr_flash, U16 bytes);

#endif
