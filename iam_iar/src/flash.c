#include <hardware.h>

void write_SegC(U8 *ptr, U8 offset, U8 bytes)
{
  char *Flash_ptr;                          // Flash pointer
/*
  unsigned int *FlashPtr,*RamPtr;                // Flash pointer
  unsigned int i;
  FlashPtr =__segment_begin("INFOC");    // Initialize Flash pointer
  RamPtr =__segment_begin("CopyFlash");    // Initialize Flash pointer
  for (i=0; i<(end_copy_seg); i++) *RamPtr++=*FlashPtr++;
*/
  Flash_ptr = (char *)0x1040;               // Initialize Flash pointer
  FCTL3 = FWKEY;                            // Clear Lock bit
  FCTL1 = FWKEY + ERASE;                    // Set Erase bit
  *Flash_ptr = 0;                           // Dummy write to erase Flash seg
  FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

  if((bytes+offset)>64) bytes=64 - offset;

  Flash_ptr += offset;
  while(bytes--){
    *Flash_ptr++ = *ptr++;                   // Write to flash
  }

  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}

void write_SegD(U8 *ptr, U8 offset, U8 bytes)
{
  char *Flash_ptr;                          // Flash pointer

  Flash_ptr = (char *)0x1000;               // Initialize Flash pointer
  FCTL3 = FWKEY;                            // Clear Lock bit
  FCTL1 = FWKEY + ERASE;                    // Set Erase bit
  *Flash_ptr = 0;                           // Dummy write to erase Flash seg
  FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

  if((bytes+offset)>64) bytes=64 - offset;

  Flash_ptr += offset;
  while(bytes--){
    *Flash_ptr++ = *ptr++;                   // Write to flash
  }

  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}

void write_SegB(U8 *ptr, U8 offset, U8 bytes)
{
  char *Flash_ptr;                          // Flash pointer

  Flash_ptr = (char *)0x1080;               // Initialize Flash pointer
  FCTL3 = FWKEY;                            // Clear Lock bit
  FCTL1 = FWKEY + ERASE;                    // Set Erase bit
  *Flash_ptr = 0;                           // Dummy write to erase Flash seg
  FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

  if((bytes+offset)>64) bytes=64 - offset;

  Flash_ptr += offset;
  while(bytes--){
    *Flash_ptr++ = *ptr++;                   // Write to flash
  }

  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}
/*
void copy_C2D(void)
{
  char *Flash_ptrC;                         // Segment C pointer
  char *Flash_ptrD;                         // Segment D pointer
  unsigned int i;

  Flash_ptrC = (char *)0x1040;              // Initialize Flash segment C ptr
  Flash_ptrD = (char *)0x1000;              // Initialize Flash segment D ptr
  FCTL3 = FWKEY;                            // Clear Lock bit
  FCTL1 = FWKEY + ERASE;                    // Set Erase bit
  *Flash_ptrD = 0;                          // Dummy write to erase Flash seg D
  FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

  for (i = 0; i < 64; i++)
  {
    *Flash_ptrD++ = *Flash_ptrC++;          // copy value segment C to seg D
  }

  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}
*/
void write_flash(U8 *ptr, U32 addr_flash, U16 bytes)
{
//  char __data20 *Flash_ptr;                          // Flash pointer
  char *Flash_ptr;                          // Flash pointer

//  Flash_ptr = (char __data20 *)addr_flash;               // Initialize Flash pointer
  Flash_ptr = (char *)addr_flash;               // Initialize Flash pointer
  FCTL3 = FWKEY;                            // Clear Lock bit
  FCTL1 = FWKEY + ERASE;                    // Set Erase bit
  *Flash_ptr = 0;                           // Dummy write to erase Flash seg
  FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

  while(bytes--){
    *Flash_ptr++ = *ptr++;                   // Write to flash
  }

  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}

