/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : atmega128.c                                                      *
* Description : This file contains the functions needed to use efs for        *
*               accessing files on an SD-card connected to an ATMega128.      *
*                                                                             *
* This program is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU General Public License                 *
* as published by the Free Software Foundation; version 2                     *
* of the License.                                                             *
                                                                              *
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
\*****************************************************************************/

/*****************************************************************************/
#include "atmega128.h"
#include "sd.h"
/*****************************************************************************/

/* The GCC's optimizer should inline */

void if_spiSelectDevice(hwInterface* file)
{
	P3OUT &= ~(1<<DD_CS); /* PORTB &= 0xFE; */
}

void if_spiUnselectDevice(hwInterface* file)
{
	PORT_CS |= (1<<DD_CS); /* PORTB |= 0x01; */
	if_spiSend(file,0xff); /* release DO */
}
/*****************************************************************************/

esint8 if_initInterface(hwInterface* file, eint8* opts)
{
	euint32 sc;

	if_spiInit(file);
	
	if_spiSelectDevice(file);
	
	if(sd_Init(file)<0)	{
		DBG((TXT("Card failed to init, breaking up...\n")));
		if_spiUnselectDevice(file);
		return(-1);
	}
	if(sd_State(file)<0){
		DBG((TXT("Card didn't return the ready state, breaking up...\n")));
		if_spiUnselectDevice(file);
		return(-2);
	}
	
	if_spiUnselectDevice(file);
	
	/* Enable SPI, master, set clock rate to fck/2 */
	SPCR = (1<<SPE) | (1<<MSTR); /* fsck / 4 */
	SPSR = 1; /* fsck / 2 */

#if 1	
	/* get file->sectorCount (do not use hardcoded = 4 ) */
	if_getDriveSize(file, &sc);
	file->sectorCount = sc/512;
	if( (sc%512) != 0) {
		file->sectorCount--;
	}
	DBG((TXT("Card Capacity is %lu Bytes (%lu Sectors)\n"), sc, file->sectorCount));
#else
	file->sectorCount=4; /* FIXME ASAP!! */
#endif
	
	DBG((TXT("Init done...\n")));
	
	return(0);
}
/*****************************************************************************/

esint8 if_readBuf(hwInterface* file,euint32 address,euint8* buf)
{
	esint8 res;
	
	if_spiSelectDevice(file);
	res = sd_readSector(file,address,buf,512);
	if_spiUnselectDevice(file);
	
	return(res);
}
/*****************************************************************************/

esint8 if_writeBuf(hwInterface* file,euint32 address,euint8* buf)
{
	esint8 res;
	
	if_spiSelectDevice(file);
	res = sd_writeSector(file,address, buf);
	if_spiUnselectDevice(file);
	
	return(res);
}
/*****************************************************************************/

esint8 if_setPos(hwInterface* file,euint32 address)
{
	return(0);
}
/*****************************************************************************/

void if_spiInit(hwInterface *iface)
{
	euint8 i;
		
	/* Set as master-out and master-clock -> output */
	DDR_SPI |= (1<<DD_MOSI) | (1<<DD_SCK);
	/* Clear master-in -> intput */
	DDR_SPI &= ~(1<<DD_MISO);
	/* Unselect card */
	PORT_CS |= (1<<DD_CS); /* PORTB |= 0x01; */
	/* Set chip-select pin as output */
	DDR_CS  |= (1<<DD_CS);
	
#if 1
	/* Enable SPI, master, set clock rate to fck/32 during init */
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR1);
	SPSR = 0; /* no fsck/2 */
#else
	/* Enable SPI, master, set clock rate to fck/2 */
	SPCR = (1<<SPE) | (1<<MSTR); /* fsck / 4 */
	SPSR = 1; /* fsck / 2 */
#endif

	/* Send 10 spi commands with card not selected */
	for(i=0;i<10;i++)
		if_spiSend(iface,0xff);

	/* Select card */
	/* PORTB &= 0xFE; moved to interfaceInit to make clear
	   that card has to be selected before calling sd_init() */
}
/*****************************************************************************/

euint8 if_spiSend(hwInterface *iface, euint8 outgoing)
{
	euint8 incoming=0;
	
	/* PORTB &= 0xFE; - by "upper level" functions */
	SPDR = outgoing;
	while(!(SPSR & (1<<SPIF)));
	incoming = SPDR;
	/* PORTB |= 0x01;  - by "upper level" functions */

	return(incoming);
}
/*****************************************************************************/

esint8 if_getDriveSize(hwInterface *iface, euint32* drive_size )
{
	esint8 res;
	
	if_spiSelectDevice(iface);
	res = sd_getDriveSize( iface, drive_size );
	if_spiUnselectDevice(iface);
	
	return res;
}
