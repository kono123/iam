#include <hardware.h>
#include <df25.h>

#include <stdio.h>
#include <string.h>

int JOURNAL_Open(void)
{
  int ret;
  U8 tag;
  U16 addr=0;

  EE_CS_Init();
  ret=DF25_GetStatus();
  DF25_Read(addr,&tag,1);
  printf("EE %x %x\n",ret,tag);
  tag=0x45;
  DF25_Write(addr,&tag,1);
  DF25_Read(addr,&tag,1);
  printf("EE %x %x\n",ret,tag);
  return 1;
}

