/******************************************************************************
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : mkfs.c                                                           *
* Description : These functions are used for creating an empty filesystem.    *
*                                                                             *
* This program is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU General Public License                 *
* as published by the Free Software Foundation; version 2                     *
* of the License.                                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* As a special exception, if other files instantiate templates or             *
* use macros or inline functions from this file, or you compile this          *
* file and link it with other works to produce a work based on this file,     *
* this file does not by itself cause the resulting work to be covered         *
* by the GNU General Public License. However the source code for this         *
* file must still be made available in accordance with section (3) of         *
* the GNU General Public License.                                             *
*                                                                             *
* This exception does not invalidate any other reasons why a work based       *
* on this file might be covered by the GNU General Public License.            *
*                                                                             *
*                                                    (c)2006 Lennart Yseboodt *
*                                                    (c)2006 Michael De Nil   *
*                                                                             *
* mthomas: testing: alignment modifications for arm-elf-gcc                   *
*          - should be portable                                               *
* function to create FAT12, w o lower size limit, by jra                      *
\*****************************************************************************/

#define MT_MOD

/*****************************************************************************/
#include "mkfs.h"
#include <stdio.h>
/*****************************************************************************/

#ifndef MT_MOD
#warning "possible alignement problems - define MT_MOD for improved code"
signed short mkfs_makevfat(Partition *part)
{
	unsigned long c,cc,ret;
	unsigned long ns,fs,ds,dc;
	unsigned char buf[512];
	
	ns=part->disc->partitions[part->activePartition].numSectors;
	
	if( ns < 66581 ){
		DBG((TXT("This is not possible due to insufficient sectors. Sorry\n")));
		return(MKFS_ERR_TOOLITTLESECTORS);
	}
	
	ret=0;
	
	for(c=1<<6;c>=1;c>>=1){
		
		/* First guess */
		ds = ns - 32;
		fs = ((ds/c)+127)/128;
		/* ds was guess too large, so fs is too large now too. */
		
		for(cc=0;cc<2;cc++){
		
			/* Round 2, error round */
			ds = ns - 32 - 2*fs;
			fs = ((ds/c)+127)/128;
			/* Since fs was too large, ds became too small. So the fs for this small ds is too small as well. */
			
			/* Round 3, correction round */
			ds = ns - 32 - 2*fs;
			fs = ((ds/c)+127)/128;
			/* The fs was too small, so ds was too large. The calculated fs should be slightly too large. */
		
		}
		
		/* Round 4, finalise */
		ds = ns - 32 - 2*fs;

		dc = ds / c;
		if(ret<(fs*128-dc)/128)ret=(fs*128-dc)/128;
		
		/* Check if with current setting we have a valid fat ? */
		
		if(dc >= 65525 + 16){
			break;
		}
	}

	/* Generate BPB */
	memClr(buf,512);
	
	/* Boot code */
	*(buf+0)=0xE9;	*(buf+1)=0x00;	*(buf+2)=0x00; /* RESET */
	
	/* OEM name */
	memCpy("DSCOSMSH",buf+3,8);
	
	/* Bytes/Sector */
	*((unsigned short*)(buf+11)) = 512;
	
	/* Sectors/Cluster */
	*(buf+13) = c;
	
	/* Reserved Sectors */
	*((unsigned short*)(buf+14)) = 32;
	
	/* Number of FAT Tables */
	*(buf+16) = 2;
	
	/* RootEntryCount */
	*((unsigned short*)(buf+17)) = 0;
	
	/* Total Sector Count __16 */
	*((unsigned short*)(buf+19)) = 0;
	
	/* Media (crap) */
	*(buf+21) = 0xF8;
	
	/* FAT size 16 */
	*((unsigned short*)(buf+22)) = 0;
	
	/* Total Sector Count __32 */
	*((unsigned long*)(buf+32)) = ns;
	
	/* Fat Size 32 */
	*((unsigned long*)(buf+36)) = fs;
	
	/* First Cluster Root Dir */
	*((unsigned long*)(buf+44)) = 2;
	
	/* VolumeID */
	*((unsigned long*)(buf+67)) = 0x13371337;
	
	/* Volume Label */
	memCpy("DISCOSMASH!",buf+71,11);
	
	/* Filesystemtype */
	memCpy("FAT32   ",buf+82,8);
	
	/* Magic */
	*(buf+510) = 0x55; *(buf+511) = 0xAA;
	
	part_writeBuf(part,0,buf);
	
	memClr(buf,512);
	for(c=32;c<(32+2*fs);c++){
		part_writeBuf(part,c,buf);
	}
	*(((unsigned long*)buf)  )=0x0FFFFFF8;
	*(((unsigned long*)buf)+1)=0x0FFFFFFF;
	*(((unsigned long*)buf)+2)=0x0FFFFFF8;
	part_writeBuf(part,32,buf);
	part_writeBuf(part,32+fs,buf);
	
	return(0);
}


#else
/* #warning "mthomas - modifications alignment" */

signed short mkfs_makevfat(Partition *part)
{
	unsigned long c,cc,ret;
	unsigned long ns,fs,ds,dc;
	unsigned char buf[512];

	ns=part->disc->partitions[part->activePartition].numSectors;
	
	if( ns < 66581 ){
		DBG((TXT("This is not possible due to insufficient sectors. Sorry\n")));
		return(MKFS_ERR_TOOLITTLESECTORS);
	}
	
	ret=0;
	
	for(c=1<<6;c>=1;c>>=1){
		
		/* First guess */
		ds = ns - 32;
		fs = ((ds/c)+127)/128;
		/* ds was guess too large, so fs is too large now too. */
		
		for(cc=0;cc<2;cc++){
		
			/* Round 2, error round */
			ds = ns - 32 - 2*fs;
			fs = ((ds/c)+127)/128;
			/* Since fs was too large, ds became too small. So the fs for this small ds is too small as well. */
			
			/* Round 3, correction round */
			ds = ns - 32 - 2*fs;
			fs = ((ds/c)+127)/128;
			/* The fs was too small, so ds was too large. The calculated fs should be slightly too large. */
		
		}
		
		/* Round 4, finalise */
		ds = ns - 32 - 2*fs;

		dc = ds / c;
		if(ret<(fs*128-dc)/128)ret=(fs*128-dc)/128;
		
		/* Check if with current setting we have a valid fat ? */
		
		if(dc >= 65525 + 16){
			break;
		}
	}

	/* Generate BPB */
	memClr(buf,512);
	
	/* Boot code */
	*(buf+0)=0xE9;	*(buf+1)=0x00;	*(buf+2)=0x00; /* RESET */
	
	/* OEM name */
	memCpy("DSCOSMSH",buf+3,8);
	
	/* Bytes/Sector */
	// mtmt *((unsigned short*)(buf+11)) = 512;
	ex_setb16(buf, 11, 512);
	
	/* Sectors/Cluster */
	*(buf+13) = c;
	
	/* Reserved Sectors */
	// mtmt *((unsigned short*)(buf+14)) = 32;
	ex_setb16(buf, 14, 32);
	
	/* Number of FAT Tables */
	*(buf+16) = 2;
	
	/* RootEntryCount */
	// mtmt *((unsigned short*)(buf+17)) = 0;
	ex_setb16(buf, 17, 0);
	
	/* Total Sector Count __16 */
	// mtmt *((unsigned short*)(buf+19)) = 0;
	ex_setb16(buf, 19, 0);
	
	/* Media (crap) */
	*(buf+21) = 0xF8;
	
	/* FAT size 16 */
	// mtmt *((unsigned short*)(buf+22)) = 0;
	ex_setb16(buf, 22, 0);
	
	/* Total Sector Count __32 */
	// mtmt *((unsigned long*)(buf+32)) = ns;
	ex_setb32(buf, 32, ns);
	
	/* Fat Size 32 */
	// mtmt *((unsigned long*)(buf+36)) = fs;
	ex_setb32(buf, 36, fs);
	
	/* First Cluster Root Dir */
	// mtmt *((unsigned long*)(buf+44)) = 2;
	ex_setb32(buf, 44, 2);
	
	/* VolumeID */
	// mtmt *((unsigned long*)(buf+67)) = 0x13371337;
	ex_setb32(buf, 67, 0x13371337);
	
	/* Volume Label */
	memCpy("DISCOSMASH!",buf+71,11);
	
	/* Filesystemtype */
	memCpy("FAT32   ",buf+82,8);
	
	/* Magic */
	*(buf+510) = 0x55; *(buf+511) = 0xAA;
	
	part_writeBuf(part,0,buf);
	
	memClr(buf,512);
	for(c=32;c<(32+2*fs);c++){
		part_writeBuf(part,c,buf);
	}
	
	/* mtmt
	*(((unsigned long*)buf)  )=0x0FFFFFF8;
	*(((unsigned long*)buf)+1)=0x0FFFFFFF;
	*(((unsigned long*)buf)+2)=0x0FFFFFF8;
	*/
	ex_setb32(buf, 0, 0x0FFFFFF8);
	ex_setb32(buf, 4, 0x0FFFFFFF);
	ex_setb32(buf, 8, 0x0FFFFFF8);
	
	
	part_writeBuf(part,32,buf);
	part_writeBuf(part,32+fs,buf);
	
	return(0);
}


signed short mkfs_makevfat12(Partition *part)
{
	unsigned long c,cc;
	unsigned long ns,fs,ds,dc,cs;
	unsigned char buf[512];
	
	ns=part->disc->partitions[part->activePartition].numSectors;
	
#if 0		// FAT32
	if( ns < 66581 ){
		DBG((TXT("This is not possible due to insufficient sectors. Sorry\n")));
		return(MKFS_ERR_TOOLITTLESECTORS);
	}
#endif // 0		// FAT32

	// optimize cluster size:	
	for(c=1<<6;c>=1;c>>=1){
		
		/* First guess */
		ds = ns - 2;
		//fs = ((ds/c)+127)/128;		// FAT32
		fs = ((ds/c)+340)/341;		// FAT12, 341 fat entries/sector
		/* ds was guess too large, so fs is too large now too. */
		
		for(cc=0;cc<2;cc++){
		
			/* Round 2, error round */
			ds = ns - 2 - 2*fs;
			fs = ((ds/c)+340)/341;
			/* Since fs was too large, ds became too small. So the fs for this small ds is too small as well. */
			
			/* Round 3, correction round */
			ds = ns - 2 - 2*fs;
			fs = ((ds/c)+340)/341;
			/* The fs was too small, so ds was too large. The calculated fs should be slightly too large. */
		
		}
		
		/* Round 4, finalise */
		ds = ns - 2 - 2*fs;

		dc = ds / c;
		cs = c;
		/* Check if with current setting we have a valid fat ? */
		
		//if(dc >= 65525 + 16){
		if(dc >= 2048){		// Limit for how many data clusters can be defined by 12 bits
			break;
		}
	}

	/* Generate BPB */
	memClr(buf,512);
	
	/* Boot code */
	*(buf+0)=0xE9;	*(buf+1)=0x00;	*(buf+2)=0x00; /* RESET */
	
	/* OEM name */
	memCpy("DOS5.0  ",buf+3,8);
	
	/* Bytes/Sector */
	//*((unsigned short*)(buf+11)) = 512;
	ex_setb16(buf, 11, 512);
	
	/* Sectors/Cluster */
	*(buf+13) = cs;
	
	/* Reserved Sectors */
	ex_setb16(buf, 14, 2);
	//*((unsigned short*)(buf+14)) = 2;
	
	/* Number of FAT Tables */
	*(buf+16) = 2;
	
	/* RootEntryCount */
	//*((unsigned short*)(buf+17)) = 32;
	ex_setb16(buf, 17, 64);			// 16 entries per sector used
	
	/* Total Sector Count __16 */
	//*((unsigned short*)(buf+19)) = ns;
	ex_setb16(buf, 19, (short) ns);
	
	/* Media (crap) */
	*(buf+21) = 0xF8;
	
	/* FAT size 16 */
	ex_setb16(buf, 22, fs);
	//*((unsigned short*)(buf+22)) = fs;
	
	/* Total Sector Count __32 */
	//*((unsigned long*)(buf+32)) = ns;
	ex_setb32(buf, 32, 0);
	//*((unsigned long*)(buf+32)) = 0;

// FAT12/16 specific:	
	/* VolumeID */
	//*((unsigned long*)(buf+39)) = 0x13371337;
	ex_setb32(buf, 39, 0x13371337);
	
	/* Volume Label */
	memCpy("DISCOSMASH!",buf+43,11);
	
	/* Filesystemtype */
	memCpy("FAT12   ",buf+54,8);
	
	/* Magic */
	*(buf+510) = 0x55; *(buf+511) = 0xAA;
	
	part_writeBuf(part,0,buf);
	
	memClr(buf,512);
	// clear root directory, size 4 sectors with 64 entries:
	for(c=(2+2*fs);c<(6+2*fs);c++){
		part_writeBuf(part,c,buf);
	}
	// two fat tables
	// clear all:
	for(c=2;c<(2+2*fs);c++){
		part_writeBuf(part,c,buf);
	}
	ex_setb32(buf, 0, 0xF8FFFFF8);
	//*(((unsigned long*)buf)  )=0xF8FFFFF8;
	ex_setb32(buf, 4, 0xF);
	//*(((unsigned long*)buf)+1)=0xF;
	part_writeBuf(part,2,buf);
	part_writeBuf(part,2+fs,buf);
	
	return(0);
}

#endif /* MT_MOD */

