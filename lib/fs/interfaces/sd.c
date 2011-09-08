#define LOOP_CNT_50      (1500)
#define LOOP_CNT_100     (LOOP_CNT_50*2)
#define LOOP_CNT_1000    (LOOP_CNT_100*10);

/*****************************************************************************/
#include "sd.h"
//#define  DEBUG
//#define  HW_ENDPOINT_MSP_SD
#include <debug.h>
#include <stdio.h>
/*****************************************************************************/

/* before this function is called:
   the SD-Card must have been unselected (CS high)
   the Clock-In line must have been clocked 80 times (10*8bits out)
   the SD-Card must be selected (CS low) "now" */

/* Definitions for MMC/SDC command */
#define CMD0	(0x40+0)	/* GO_IDLE_STATE */
#define CMD1	(0x40+1)	/* SEND_OP_COND (MMC) */
//#define	ACMD41	(0xC0+41)	/* SEND_OP_COND (SDC) */
#define	ACMD41	(0x40+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(0x40+8)	/* SEND_IF_COND */
#define CMD9	(0x40+9)	/* SEND_CSD */
#define CMD10	(0x40+10)	/* SEND_CID */
#define CMD12	(0x40+12)	/* STOP_TRANSMISSION */
#define CMD13   (0x40+13)
#define ACMD13	(0xC0+13)	/* SD_STATUS (SDC) */
#define CMD16	(0x40+16)	/* SET_BLOCKLEN */
#define CMD17	(0x40+17)	/* READ_SINGLE_BLOCK */
#define CMD18	(0x40+18)	/* READ_MULTIPLE_BLOCK */
#define CMD23	(0x40+23)	/* SET_BLOCK_COUNT (MMC) */
#define	ACMD23	(0xC0+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(0x40+24)	/* WRITE_BLOCK */
#define CMD25	(0x40+25)	/* WRITE_MULTIPLE_BLOCK */
#define CMD55	(0x40+55)	/* APP_CMD */
#define CMD58	(0x40+58)	/* READ_OCR */


static euint8 CardType;

/*****************************************************************************/

static euint8 wait_ready( hwInterface *iface )
{
	euint8  res;
	euint16 retry = LOOP_CNT_50;
	
	/* should retry 50ms */
	do {
		retry--;
		res = if_spiSend( iface, 0xff );
	} while ( retry && ( res == 0xff ) );
	
	return res;
}

static euint8 wait_ready_( hwInterface *iface )
{
	euint8  res;
	euint16 retry = 10;
	while(retry--){
          res = if_spiSend( iface, 0xff );
        }
	
	return res;
}

static char _WaitUntilReady( hwInterface *iface, euint32 sycles )
{
  euint32 LoopRem;
  euint8  aData[2];

  /* Wait for the card to report ready (Note 2) */
  do {
    aData[0]=if_spiSend(iface, 0xFF);
    if (aData[0] == 0xFF) {
      return 0;                          /* O.K., Card is ready */
    }
  } while (sycles-- > 0);
  return 1;                              /* Time out error */
}

static int _CheckR1(hwInterface *iface)
{
  euint8  Data;
  euint8  LoopRem;

  /* Read NCR bits and response token */
  LoopRem = 8;
  do {
    Data=if_spiSend(iface, 0xFF);
    if (Data != 0xff) {
      break;
    }
    if (--LoopRem == 0) {
      break;                                 /* Error: More than 8 bytes NCR (0xff) */
    }
  } while (1);

  return Data;                        /* No error if 0 */
}

void sd_reset(hwInterface *iface)
{
  int i=10;
  if_spiUnselectDevice(iface);
  while(i--) if_spiSend(iface,0xff);
}


unsigned char sd_Command( hwInterface *iface , unsigned char Cmd, euint16 paramx, euint16 paramy)
{
  euint8 r;
  euint8 CmdBuffer[6];

  if ( Cmd == CMD0 ) {
    sd_reset(iface);
  }
  if_spiSelectDevice(iface);

//  if_spiSend(iface,0xff);

  /* build setup command token (48 bit) */
  CmdBuffer[0] = Cmd;
//  CmdBuffer[1] = (euint8)(Arg >> 24) & 0xFF;
//  CmdBuffer[2] = (euint8)(Arg >> 16) & 0xFF;
//  CmdBuffer[3] = (euint8)(Arg >> 8) & 0xFF;
//  CmdBuffer[4] = (euint8) Arg & 0xFF;
  CmdBuffer[1] = (euint8)(paramx >> 8);
  CmdBuffer[2] = (euint8) paramx;
  CmdBuffer[3] = (euint8)(paramy >> 8);
  CmdBuffer[4] = (euint8) paramy;
  CmdBuffer[5] = 0xFF;  /* SPI default mode, no use for CRC */
  if (Cmd == CMD0) {
    CmdBuffer[5] = 0x95;  /* This command needs CRC, even in SPI mode */
  } else if(Cmd == CMD8) {
    CmdBuffer[5] = 0x97;
  }

  /* Make sure the card is ready */
  r = _WaitUntilReady(iface,20);
  if (r) {
    return 0xff;                           /* Time out waiting for ready */
  }

  /* Send command token */
  if_spiWriteBlock(iface, CmdBuffer, 6);
  r = _CheckR1(iface);   /* Receive response */

//  if_spiUnselectDevice(iface);
  return r; /* return received response token */
}


euint8 sd_Command0(hwInterface *iface, euint8 cmd, euint16 paramx, euint16 paramy)
{
  euint8 checksum, res, n;

//printf("<<sd_Command %x\n",cmd);	

//	if (cmd & 0x80) {	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
//		cmd &= 0x7F;
//		res = sd_Command( iface, CMD55, 0, 0 ); /* recursion! */
//		if (res > 1) return res;
//	}
	
	if ( cmd == CMD0 ) {
          sd_reset(iface);
//          if_spiUnselectDevice(iface);
	
//	if ( wait_ready( iface ) != 0xff ) return 0xff;
//	  wait_ready_( iface);
        }
	if_spiSelectDevice(iface);
	
	checksum = 0xFF;
	if ( cmd == CMD0 ) {
		checksum = 0x95;
	}else if ( cmd == CMD8 ) {
		checksum = 0x97;
	}

	if_spiSend(iface, cmd );
	if_spiSend(iface,(euint8) (paramx >> 8)); /* MSB of parameter x */
	if_spiSend(iface,(euint8) (paramx));      /* LSB of parameter x */
	if_spiSend(iface,(euint8) (paramy >> 8)); /* MSB of parameter y */
	if_spiSend(iface,(euint8) (paramy));      /* LSB of parameter y */
	if_spiSend(iface, checksum);

	/* Receive command response */
	if (cmd == CMD12) {
		if_spiSend( iface, 0xff);  /* Skip a stuff byte when stop reading */
        }
	
	n = 50;                        /* Wait for a valid response in timeout of 10 attempts */
	do {
		res = if_spiSend( iface, 0xff );
	} while ((res & 0x80) && --n);
	
	return res;
}

euint8 sd_Command_(hwInterface *iface, euint8 cmd, euint16 paramx, euint16 paramy)
{
  euint8 checksum, res, n;

//printf("<<sd_Command %x\n",cmd);	

	if (cmd & 0x80) {	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		res = sd_Command( iface, CMD55, 0, 0 ); /* recursion! */
		if (res > 1) return res;
	}
	
	if ( cmd == CMD0 ) {
          if_spiUnselectDevice(iface);
	  wait_ready_( iface);
        }
	if_spiSelectDevice(iface);
	
	if_spiSend(iface, cmd );
	if_spiSend(iface,(euint8) (paramx >> 8)); /* MSB of parameter x */
	if_spiSend(iface,(euint8) (paramx));      /* LSB of parameter x */
	if_spiSend(iface,(euint8) (paramy >> 8)); /* MSB of parameter y */
	if_spiSend(iface,(euint8) (paramy));      /* LSB of parameter y */

	checksum = 0xFF;
	if ( cmd == CMD0 ) {
		checksum = 0x95;
	}else if ( cmd == CMD8 ) {
		checksum = 0x97;
	}
	if_spiSend(iface, checksum);

	/* Receive command response */
	if (cmd == CMD12) {
		if_spiSend( iface, 0xff);  /* Skip a stuff byte when stop reading */
	}
	
	n = 20;                        /* Wait for a valid response in timeout of 10 attempts */
	do {
		res = if_spiSend( iface, 0xff );
	} while ((res & 0x80) && --n);
	
	return res;
}
/*****************************************************************************/
euint8 sd_Resp8b(hwInterface *iface)
{
	euint8 i;
	euint8 resp;
	
	/* Respone will come after 1 - 8 pings */
	for(i=0;i<8;i++){
		resp = if_spiSend(iface,0xff);
		if(resp != 0xff)
			return(resp);
	}
		
	return(resp);
}
/*****************************************************************************/
void sd_Resp8bError(hwInterface *iface,euint8 value)
{
	switch(value)
	{
		case 0x40:
			DBG((TXT("Argument out of bounds.\n")));
			break;
		case 0x20:
			DBG((TXT("Address out of bounds.\n")));
			break;
		case 0x10:
			DBG((TXT("Error during erase sequence.\n")));
			break;
		case 0x08:
			DBG((TXT("CRC failed.\n")));
			break;
		case 0x04:
			DBG((TXT("Illegal command.\n")));
			break;
		case 0x02:
			DBG((TXT("Erase reset (see SanDisk docs p5-13).\n")));
			break;
		case 0x01:
			DBG((TXT("Card is initialising.\n")));
			break;
		default:
			DBG((TXT("Unknown error 0x%x (see SanDisk docs p5-13).\n"),value));
			break;
	}
}
/*****************************************************************************/

esint8 sd_State(hwInterface *iface)
{
	eint16 value;
	
	value = sd_Command(iface, CMD13, 0, 0);
	value = (value<<8) | if_spiSend( iface, 0xff);

	switch(value)
	{
		case 0x000:
			return(1);
		case 0x0001:
			DBG((TXT("Card is Locked.\n")));
			break;
		case 0x0002:
			DBG((TXT("WP Erase Skip, Lock/Unlock Cmd Failed.\n")));
			break;
		case 0x0004:
			DBG((TXT("General / Unknown error -- card broken?.\n")));
			break;
		case 0x0008:
			DBG((TXT("Internal card controller error.\n")));
			break;
		case 0x0010:
			DBG((TXT("Card internal ECC was applied, but failed to correct the data.\n")));
			break;
		case 0x0020:
			DBG((TXT("Write protect violation.\n")));
			break;
		case 0x0040:
			DBG((TXT("An invalid selection, sectors for erase.\n")));
			break;
		case 0x0080:
			DBG((TXT("Out of Range, CSD_Overwrite.\n")));
			break;
		default:
			if(value>0x00FF)
				sd_Resp8bError(iface,(euint8) (value>>8));
			else
				DBG((TXT("Unknown error: 0x%x (see SanDisk docs p5-14).++\n"),value));
			break;
	}
	return(-1);
}
/*****************************************************************************/

static euint8 xmit_datablock (
	hwInterface *iface,
	const euint8 *buff,     /* 512 byte data block to be transmitted */
	euint8 token            /* Data/Stop token */
)
{
	euint8  resp;
	euint16 wc;

	if ( wait_ready( iface ) != 0xFF ) {
//printf("xmit error ready\n");
		return 0;
	}

	if_spiSend( iface, token );          /* Xmit data token */
	if ( token != 0xFD ) {               /* Is data token */
		wc = 0;
		do {                             /* Xmit the 512 byte data block to MMC */
			if_spiSend( iface, *buff++);
                        wc++;
		} while (wc<512);
		
		if_spiSend( iface, 0xff);        /* CRC (Dummy) */
		if_spiSend( iface, 0xff);        /* CRC (Dummy) */
		resp = if_spiSend( iface, 0xff); /* Reveive data response */
		if ( (resp & 0x1F) != 0x05 ){       /* If not accepted, return with error */
//printf("xmit error resp %x\n",resp);
			return 0;
                }
	}

	return 1;
}
/*****************************************************************************/
/*
esint8 sd_writeSector(hwInterface *iface,euint32 address, euint8* buf)
{
	euint32 sector;
	euint8  *pbuff;
	euint8  count = 1; // only one block for now to keep interface compatible
	
//	DBG((TXT("Trying to write %u to sector %u.\n"),(void *)&buf,address));
	
	pbuff = buf;
	
	if ( !(CardType & 8) ) {
		sector = address * 512;            // Convert to byte address if needed
	}
	else {
		sector = address;
	}

	if (count == 1) {
		// Single block write
		if ( (sd_Command(iface, CMD24, (euint16) (sector >> 16), (euint16) sector) == 0)
			&& xmit_datablock(iface, pbuff, 0xFE) ) {
			count = 0;
		}
	}
	else {                        // Multiple block write
		if ( CardType & 6 ) {
			sd_Command(iface, ACMD23, 0, count);
		}
		if ( sd_Command(iface, CMD25, (euint16) (sector >> 16), (euint16) sector) == 0 ) {
			do {
				if ( !xmit_datablock( iface, pbuff, 0xFC ) ) {
					break;
				}
				pbuff += 512;
			} while (--count);
			if ( !xmit_datablock( iface, 0, 0xFD ) )  {// STOP_TRAN token
				count = 1;
			}
		}
	}

	return ( count ? (-1) : (0) );
}
*/
/*****************************************************************************/
static euint8 rcvr_datablock (
	hwInterface *iface,
	euint8 *buff,         /* Data buffer to store received data */
	euint16 btr           /* Byte count (must be even number) */
)
{
	euint8 token;
	euint16 retry;


	retry = LOOP_CNT_1000;
	/* should retry 100ms */
	do {
		token = if_spiSend( iface, 0xff );
		retry--;
	} while ((token == 0xFF) && retry);

	
	if(token != 0xFE) {
//printf("rcvr error %x\n",token);
		return 0;                   /* If not valid data token, retutn with error */
	}

	do {                            /* Receive the data block into buffer */
		*buff++=if_spiSend( iface, 0xff );
		*buff++=if_spiSend( iface, 0xff );
	} while (btr -= 2);
	if_spiSend( iface, 0xff);       /* Discard CRC */
	if_spiSend( iface, 0xff);       /* Discard CRC */

	return 1;                       /* Return with success */
}


/*****************************************************************************/
/*
esint8 sd_readSector_(hwInterface *iface,euint32 address, euint8* buf, euint16 len)
{
	euint32 sector;
	euint8  count;
	euint8  *pbuff;

//DBG((TXT("sd_readSector::Trying to read sector %u and store it at %p.\n"),address,&buf[0]));
	
	pbuff = buf;
	count = len/512; // >>9
	
	if ( !(CardType & 8) ) {
		sector = address * 512;                  // Convert to byte address if needed
	}
	else {
		sector = address;
	}

	if (count == 1) {
		// Single block read
		if ( (sd_Command(iface, CMD17, (euint16) (sector >> 16), (euint16) sector) == 0)
			&& rcvr_datablock(iface, pbuff, 512) ) {
			count = 0;
		}
	}
	else {
		// Multiple block read
		if ( sd_Command(iface, CMD18, (euint16) (sector >> 16), (euint16) sector) == 0) {
			do {
				if ( !rcvr_datablock(iface, pbuff, 512) ) {
					break;
				}
				pbuff += 512;
			} while (--count);
			sd_Command( iface, CMD12, 0, 0 );     // STOP_TRANSMISSION
		}
	}
	
	return ( count ? (-1): (0) );
}
*/
/*****************************************************************************/
/* utility code included by Martin Thomas, based on code from Chan */
esint8 sd_getDriveInfo( hwInterface *iface, euint8 infoid, euint32 *result )
{
	
	esint8 res;
	euint8 n, csd[16], resp;
	euint16 csize;
	euint32 retry;
	
	res = -1;
	
	resp = sd_Command(iface, CMDREADCSD, 0, 0);  // CMD9
	retry = LOOP_CNT_1000;
	while ( resp != 0xFE && retry ) {
		resp = sd_Resp8b(iface);
		retry--;
	}
	
	if ( !retry ) {
		return(-1);
	}

//	DBG((TXT("CSD:")));
	for( n=0; n<16; n++) {
		csd[n] = if_spiSend(iface,0xff);
//		DBG((TXT(" %02x"), csd[n]));
	}
//	DBG((TXT("\n")));
	
	switch ( infoid ) {
	
	case SD_INFO_GET_SECTOR_COUNT :               /* Get number of sectors on the disk (DWORD) */
		if ( (csd[0] >> 6) == 1 ) {          /* SDC ver 2.00 */
			csize = csd[9] + ((euint16)csd[8] << 8) + 1;
			*result = (euint32)csize << 10;
		} else {                              /* SDC ver 1.XX or MMC*/
			n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
			csize = (csd[8] >> 6) + ((euint16)csd[7] << 2) + ((euint16)(csd[6] & 3) << 10) + 1;
			*result = (euint32)csize << (n - 9);
		}
		res = 0;
		break;

	case SD_INFO_GET_SECTOR_SIZE :               /* Get R/W sector size (WORD) */
		*result = (euint32)512;
		res = 0;
		break;

	case SD_INFO_GET_BLOCK_SIZE :                /* Get erase block size in unit of sector (DWORD) */
		if (CardType & 4) {
			/* SDC ver 2.00 */
			if ( sd_Command(iface, ACMD13, 0, 0) == 0) {        /* Read SD status */
				if_spiSend( iface, 0xff );
				if (rcvr_datablock(iface, csd, 16)) {           /* Read partial block */
					for (n = 64 - 16; n; n--) {
						if_spiSend( iface, 0xff );              /* Purge trailing data */
					}
					*result = 16UL << (csd[10] >> 4);
					res = 0;
				}
			}
		} else {
			/* SDC ver 1.XX or MMC */
			if ( (sd_Command(iface, CMD9, 0, 0) == 0) && rcvr_datablock( iface, csd, 16)) {    /* Read CSD */
				if (CardType & 2) {
					/* SDC ver 1.XX */
					*result = (((csd[10] & 63) << 1) + ((euint16)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
				} else {
					/* MMC */
					*result = ((euint32)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
				}
				res = 0;
			}
		}
		break;

	case SD_INFO_GET_TYPE :		/* Get card type flags (1 byte) */
		*result = (euint32)CardType;
		res = 0;
		break;

	default:
		res = -2;

	}	/* switch */

	// release_spi();

	return res;
}

/* ****************************************************************************
 calculates size of card from CSD
 (extension by Martin Thomas based on code from Chan)
 */

#if 1
esint8 sd_getDriveSize(hwInterface *iface, euint32* drive_size )
{
	esint8  res;
	euint32 numsec, sizesec;
	
	res = -1;
	if ( sd_getDriveInfo( iface, SD_INFO_GET_SECTOR_COUNT, &numsec) == 0 ) {
		if ( sd_getDriveInfo( iface, SD_INFO_GET_SECTOR_SIZE, &sizesec ) == 0 ) {
			*drive_size = numsec * sizesec;
			res = 0;
			
		}
	}
//	DBG((TXT("numsec %lu sizesec %lu\n"), numsec, sizesec));
	
	return res;
}

#else
// old - based on code from Holger Klabunde - no longer used
esint8 sd_getDriveSize(hwInterface *iface, euint32* drive_size )
{
	euint8 cardresp, i, by;
	euint8 iob[16];
	euint16 c_size, c_size_mult, read_bl_len;
	
	sd_Command(iface, CMDREADCSD, 0, 0);
	
	do {
		cardresp = sd_Resp8b(iface);
	} while ( cardresp != 0xFE );

//	DBG((TXT("CSD:")));
	for( i=0; i<16; i++) {
		iob[i] = if_spiSend(iface,0xff);
//		DBG((TXT(" %02x"), iob[i]));
	}
//	DBG((TXT("\n")));

	if_spiSend(iface,0xff);
	if_spiSend(iface,0xff);
	
	c_size = iob[6] & 0x03; // bits 1..0
	c_size <<= 10;
	c_size += (euint16)iob[7]<<2;
	c_size += iob[8]>>6;

	by= iob[5] & 0x0F;
	read_bl_len = 1;
	read_bl_len <<= by;

	by=iob[9] & 0x03;
	by <<= 1;
	by += iob[10] >> 7;
	
	c_size_mult = 1;
	c_size_mult <<= (2+by);
	
	*drive_size = (euint32)(c_size+1) * (euint32)c_size_mult * (euint32)read_bl_len;
	
	return 0;
}
#endif


/*****************************************************************************/

/*
	Lock/Unlock function contributed by Dario Pennisi sf.net 1612416
	EXPERIMENTAL!
*/

/* <cite>
This changes allow locking/unlocking the MMC/SD card with a password
through a single API:

esint8 sd_lockUnlock(hwInterface *iface,char *pcstr, euint8 len, euint8
flags)

flags is the bitwise or of the following flags:

#define SDPWD_SETPWD 1
#define SDPWD_CLRPWD 2
#define SDPWD_LOCK 4
#define SDPWD_ERASE 8


please note that once you lock a card it won't be readable by almost any
device if it is not previously unlocked or the password is cleared (PC will
for example return bad media). note also that password can be cleared
(along with contents of the card) in case you forget it.

please check SanDisk's document for details
</cite> */


esint8 sd_lockUnlock(hwInterface *iface,char *pcstr, euint8 len, euint8 flags)
{
	euint8 resp;
	euint8 t;
	esint8 ret;

	if (len>8) {
		return -1;
	}
	
	resp = sd_Command(iface, CMDSETBLOCKLEN, 0, 5);

	resp = sd_Command(iface, CMDLOCKUNLOCK, 0, 0);

	if_spiSend(iface,0xfe); /* Start block */
	if_spiSend(iface,flags); /* Send data */
	if_spiSend(iface,len); /* Send data */
	for (t=0;t<len;t++)
	{
		if_spiSend(iface,pcstr[t]); /* Send data */
	}
	if_spiSend(iface,0xff); /* Checksum part 1 */
	if_spiSend(iface,0xff); /* Checksum part 2 */

	if_spiSend(iface,0xff);

	while(if_spiSend(iface,0xff)!=0xff){
		t++;
		/* Removed NOP */
	}
	ret = sd_State(iface);

	resp = sd_Command(iface, CMDSETBLOCKLEN, 0, 512);
	ret |= sd_State(iface);
	
	return ret;
}

esint8 sd_Init(hwInterface *iface)
{
  euint32 retry;
  euint8  n, resp, ty, ocr[7], cmd;
	
  ty = 0;

// Try to send command enter idle (CMD0) up to 100 times */
  retry=100;
  do {
    resp = sd_Command0(iface, CMD0, 0, 0);
  } while( resp!=1 && retry-- );
	
  if( resp != 1 ) {
    if( resp == 0xff ){
	DBG((TXT("No card\n")));
      return -1;
    }else{
      sd_Resp8bError( iface, resp );
      return -2;
    }
  }

//	DBG((TXT("Card identified as ")));
  resp = sd_Command( iface, CMD8, 0, 0x01AA);

  if ( resp == 1 ) {
    for ( n=0; n<4; n++ ) {
      ocr[n] = if_spiSend( iface, 0xff );
    }
    if ( ocr[2] == 0x01 && ocr[3] == 0xAA ) {          /* The card can work at vdd range of 2.7-3.6V */
      retry = LOOP_CNT_1000; /* should retry 1000ms */
      do { // Wait for leaving idle state (ACMD41 with HCS bit)
        resp=sd_Command( iface, CMD55, 0, 0);
        resp=sd_Command( iface, ACMD41, 0x4000,0);
        retry--;
      } while ( retry && (resp != 0) );

      if(retry==0){
	DBG((TXT("ACMD41 timeout\n")));
        return -3;
      }

      resp=sd_Command( iface, CMD58, 0, 0);  // Check CCS bit in the OCR
      if(resp == 0) {
        for ( n = 0; n < 4; n++ )  ocr[n] = if_spiSend( iface, 0xff );
        ty = (ocr[0] & 0x40) ? 12 : 4;
        if ( ty == 12 ) DBG((TXT("SDHC HighCap\n")));
        if ( ty == 4 )  DBG((TXT("SDHC\n")));
      }else{
	DBG((TXT("CMD58 no resp\n")));
        return -4;
      }
    }
  }else {  // SDSC or MMC
    resp=sd_Command( iface, CMD55, 0, 0);
    resp=sd_Command( iface, ACMD41, 0, 0);
//    if ( sd_Command( iface, ACMD41, 0, 0 ) <=1 ) {
    if ( resp <=1 ) {
      ty = 2;
      cmd = ACMD41;	/* SDSC */
      DBG((TXT("SDSC\n")));
    } else {
      ty = 1;
      cmd = CMD1;		/* MMC */
      DBG((TXT("MMC\n")));
      retry = LOOP_CNT_1000; // should retry 1000ms
      do {
// Wait for leaving idle state
        resp = sd_Command( iface, CMD1, 0, 0);
        retry--;
      } while ( retry && (resp != 0) );
    }
			
    retry = LOOP_CNT_1000; // should retry 1000ms
    do {                   // Wait for leaving idle state
      resp = sd_Command( iface, CMD55, 0, 0);
      resp = sd_Command( iface, ACMD41, 0, 0);
//      resp = sd_Command( iface, cmd, 0, 0);
      retry--;
    } while ( retry && (resp != 0) );

// Set R/W block length to 512
    if(ty!=12){
  //    if ( !retry || ( sd_Command( iface, CMD16, 0, 512 ) != 0 ) ) {
       resp = sd_Command( iface, CMD16, 0, 512);
       if ((resp != 0)  ) {
           ty = 0; // failed
      }
    }
  }

  CardType = ty;
  iface->CardType = ty;

  if ( !ty ) {
    DBG((TXT(" ** UNKNOWN - ERROR **\n")));
    return(-3);
  }

  return(0);
}

