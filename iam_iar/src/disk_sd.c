#include "hardware.h"
#include <efs.h>
#include <ls.h>
#include <disk_sd.h>
#include "errors.h"

#include <stdio.h>
#include <string.h>

#if __IAR_SYSTEMS_ICC__
//#if __ICC430__

EmbeddedFileSystem efs;
DirList FileList;

void DISK_UpdateFreeClusterCount(void)
{
  if(efs.Flags & NEED_UPDATE_FREE_CLUSTER){
    efs.myFs.FreeClusterCount=fat_countFreeClusters(&efs.myFs);
    efs.Flags &= ~NEED_UPDATE_FREE_CLUSTER;
  }
}

U32 DISK_GetFreeSpaceMb(void)
{
  U32 ret;
  DISK_UpdateFreeClusterCount();
  ret=efs.myFs.FreeClusterCount;
  ret=ret*efs.myFs.volumeId.SectorsPerCluster;
  ret=ret/2048;
  return(ret);
}

U32 DISK_GetTotalSpaceMb(void)
{
  U32 ret;
  ret=efs.myFs.DataClusterCount;
  ret=ret*efs.myFs.volumeId.SectorsPerCluster;
  ret=ret/2048;
  return(ret);
}

void DISK_Init(void)
{
  efs.Flags = NEED_UPDATE_FREE_CLUSTER;
}

unsigned int DISK_Mount(void)
{
  if(efs.Flags & DISK_MOUNT) return 0;
  SD_POWER_ON();
  if(efs_init(&efs,0)!=0){
//    printf("DISK_Mount Error!!!\n");
    SD_POWER_OFF();
    return(EFS_INIT_ERROR);
  }
  efs.Flags |= DISK_MOUNT;
//  efs.myFs.FreeClusterCount=fat_countFreeClusters(&efs.myFs);
  return(0);
}

void DISK_Unmount(void)
{
  if(efs.Flags & DISK_MOUNT){
    fs_umount(&efs.myFs);
    efs.Flags &= ~DISK_MOUNT;
    efs.Flags |= NEED_UPDATE_FREE_CLUSTER;
    SD_POWER_OFF();
  }
}

/*
unsigned int FS_GetFreeSpace(void)
{
  unsigned int _all,_free,per;

  _all=efs.myFs.DataClusterCount;
  _free=efs.myFs.FreeClusterCount;
//  _free=fat_countFreeClusters(&efs.myFs);
  printf("free %d from %d\n",_free,_all);
  printf("-- %d \n",_free,_free*100/_all);
  return(efs.myFs.FreeClusterCount);
}
*/

EFS_STATUS FS_FileOpen(File* file, char * filename, char mode)
{
  esint8 err;
  err=file_fopen(file,&efs.myFs,filename,mode);
  if(err){
    return(err);
  }else{
    return(0);
  }
}

EFS_STATUS FS_FileClose(File *file)
{
  return(file_fclose(file));
}

EFS_STATUS FS_FileClose2(File *file,unsigned int size)
{
  return(file_fclose2(file,size));
}

EFS_STATUS FS_Flush(void)
{
  return(fs_flushFs(&efs.myFs));
}

EFS_STATUS FS_FileWrite(File *file,unsigned int size,unsigned int *write_size,U8 *buff)
{
  if(size){
    *write_size=file_write(file,size,buff);
    if(*write_size==0){
      return(EFS_FILE_WRITE_ERROR);
    }
    efs.Flags |= NEED_UPDATE_FREE_CLUSTER;
  }else{
    *write_size=0;
  }
  return(0);
}

EFS_STATUS FS_FileRead(File *file,unsigned int size,unsigned int *read_size,U8 *buff)
{
  if(size){
    *read_size=file_read(file,size,buff);
    if(*read_size==0){
      return(EFS_FILE_WRITE_ERROR);
    }
  }else{
    *read_size=0;
  }
  return(0);
}

EFS_STATUS FS_DeleteFile(char *filename)
{
  if(rmfile(&efs.myFs,filename)!=0){
    return(EFS_FILE_DELETE_ERROR);
  }else{
    efs.Flags |= NEED_UPDATE_FREE_CLUSTER;
    return(0);
  }
}

EFS_STATUS FS_FindFileFirst(char *buff, char *dir)
{
  if(ls_openDir(&FileList,&efs.myFs,dir)){
    printf("ls_openDir ERR!!!\n");
    return(EFS_OPEN_DIR_ERROR);
  }
  if(ls_getNext(&FileList)==0){
    strcpy(buff,FileList.currentEntry.FileName);
//    printf("%s, size=%d\n",list.currentEntry.FileName,list.currentEntry.FileSize);
    return(0);
  }else{
    return(EFS_NO_FILE_FIND);
  }
}

EFS_STATUS FS_FindFile(char *buff)
{
  if(ls_getNext(&FileList)==0){
    strcpy(buff,FileList.currentEntry.FileName);
//    printf("%s, size=%d\n",list.currentEntry.FileName,list.currentEntry.FileSize);
    return(0);
  }else{
    return(EFS_NO_FILE_FIND);
  }
}

EFS_STATUS FS_MakeDir(char *dirname)
{
  U8 ret;
  ret=mkdir(&efs.myFs,dirname);
  if(ret){
      return(EFS_MAKE_DIR_ERROR);
  }
  return(0);
}

U8 fstest(void)
{
  EmbeddedFile file_r, file_w;
  EFS_STATUS fserr;
  unsigned short e;
  unsigned int writebytes;
  char buf[512];

  if(DISK_Mount()!=0){
    printf("efs_init error!\n");
    return(1);
  }

  if(FS_DeleteFile("proba1.txt")!=0){
    printf("file_delete error!\n");
  }

  if(FS_FileOpen(&file_w,"proba1.txt",'w')!=0){
    printf("file_fopen_w error!\n");
    return(3);
  }
  buf[0]=0xa5;
  buf[1]=0x66;
  buf[2]=0x77;
  buf[3]=0x88;
  e=4;
  fserr=FS_FileWrite(&file_w,e,&writebytes,buf);
  if(fserr){
    printf("write error %x\n",fserr);
  }else{
    printf("write %d bytes\n",writebytes);
  }

  FS_FileClose(&file_w);

  DISK_Unmount();

  printf("The End\n");

  return(0);
}
////////////////////////////////////////////////////////////////////
#else

void DISK_Init(void)
{
//  efs.Flags = NEED_UPDATE_FREE_CLUSTER;
}

unsigned int DISK_Mount(void)
{
//  if(efs.Flags & DISK_MOUNT) return 0;
//  SD_POWER_ON();
//  if(efs_init(&efs,0)!=0){
//    printf("DISK_Mount Error!!!\n");
//    SD_POWER_OFF();
//    return(EFS_INIT_ERROR);
//  }
//  efs.Flags |= DISK_MOUNT;
  return(0);
}
#endif
