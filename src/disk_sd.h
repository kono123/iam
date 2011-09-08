#ifndef _DISK_SD_H_
#define _DISK_SD_H_

#ifdef IAM_EMU
  #include <stdio.h>
#else
  #include <efs.h>
#endif

#include "errors.h"

//flags
#define DISK_MOUNT                1
#define NEED_UPDATE_FREE_CLUSTER  2

void DISK_Init(void);

unsigned int DISK_Mount(void);
void DISK_Unmount(void);

void DISK_UpdateFreeClusterCount(void);
U32 DISK_GetFreeSpaceMb(void);
U32 DISK_GetTotalSpaceMb(void);

EFS_STATUS FS_FileOpen(FILE* file, char *filename, char mode);
EFS_STATUS FS_FileClose(FILE *file);
EFS_STATUS FS_FileClose2(FILE *file,unsigned int size);
EFS_STATUS FS_FileWrite(FILE *file,unsigned int size,unsigned int *write_size,U8 *buff);
EFS_STATUS FS_FileRead(FILE *file,unsigned int size,unsigned int *read_size,U8 *buff);
EFS_STATUS FS_Flush(void);
EFS_STATUS FS_DeleteFile(char* filename);
EFS_STATUS FS_FindFileFirst(char *buff,char *dir);
EFS_STATUS FS_FindFile(char *buff);
EFS_STATUS FS_MakeDir(char *dirname);

#endif  /* _DISK_SD_H_*/
