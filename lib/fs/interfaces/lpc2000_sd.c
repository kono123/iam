/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename    : lpc2000_spi.c                                                 *
* Description : This file contains the functions needed to use efs for        *
*               accessing files on an SD-card connected to an LPC2xxx.        *
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
*                                                    (c)2006 Martin Thomas    *
*                                                    (c)2006 Mike Anton       *
*******************************************************************************/

/*****************************************************************************/
#include "interfaces/LPC2000_regs.h"
#include "config.h"
#include "interfaces/lpc2000_sd.h"
#include "interfaces/sd.h"
/*****************************************************************************/

#ifndef HW_ENDPOINT_LPC2000_SPINUM
#error "HW_ENDPOINT_LPC2000_SPINUM has to be defined in config.h"
#endif

#if ( HW_ENDPOINT_LPC2000_SPINUM == 0 )
// LPC213x ### SPI0 ###

// check config
#ifdef HW_ENDPOINT_LPC2000_SSPAUTOCS
#error "automatic chip-select not supported by SPI0"
#endif
#ifdef HW_ENDPOINT_LPC2000_SSPFIFO
#error "FIFO not supported by SPI0"
#endif

// SP0SPCR  Bit-Definitions
#define CPHA    3
#define CPOL    4
#define MSTR    5
// SP0SPSR  Bit-Definitions
#define SPIF    7

#ifdef HW_ENDPOINT_LPC2000_FASTIO
#define SPI_IODIR   FIODIR0
#define SPI_IOCLR   FIOCLR0
#define SPI_IOSET   FIOSET0
#else
#define SPI_IODIR   IODIR0
#define SPI_IOCLR   IOCLR0
#define SPI_IOSET   IOSET0
#endif

#define SPI_SCK_PIN    4   /* Clock       P0.4  out */
#define SPI_MISO_PIN   5   /* from Card   P0.5  in  */
#define SPI_MOSI_PIN   6   /* to Card     P0.6  out */
#define SPI_SS_PIN     7   /* Card-Select P0.7 - GPIO out */

#define SPI_PINSEL     PINSEL0
#define SPI_SCK_FUNCBIT   8
#define SPI_MISO_FUNCBIT  10
#define SPI_MOSI_FUNCBIT  12
#define SPI_SS_FUNCBIT    14

#define SPI_PRESCALE_REG  S0SPCCR
#define SPI_PRESCALE_MIN  8

#define PERIPH_CS_GPIO /* default for SPI0 */


#elif ( HW_ENDPOINT_LPC2000_SPINUM == 1 )
// LPC213x ### SSP ### ("SPI1")

// SSPCR0  Bit-Definitions
#define CPOL    6
#define CPHA    7
// SSPCR1  Bit-Defintions
#define SSE     1
#define MS      2
#define SCR     8
// SSPSR  Bit-Definitions
#define TNF     1
#define RNE     2
#define BSY     4

#ifdef HW_ENDPOINT_LPC2000_FASTIO
#define SPI_IODIR   FIODIR0
#define SPI_IOCLR   FIOCLR0
#define SPI_IOSET   FIOSET0
#else
#define SPI_IODIR   IODIR0
#define SPI_IOCLR   IOCLR0
#define SPI_IOSET   IOSET0
#endif

#define SPI_SCK_PIN    17   /* Clock       P0.17  out */
#define SPI_MISO_PIN   18   /* from Card   P0.18  in  */
#define SPI_MOSI_PIN   19   /* to Card     P0.19  out */
/* Card-Select P0.20 - GPIO out during startup
   Function 03 during normal operation 
   or always GPIO if HW_ENDPOINT_LPC2000_SSPAUTOCS is undefined */
#define SPI_SS_PIN     20   

#define SPI_PINSEL     PINSEL1
#define SPI_SCK_FUNCBIT   2
#define SPI_MISO_FUNCBIT  4
#define SPI_MOSI_FUNCBIT  6
#define SPI_SS_FUNCBIT    8

#define SPI_PRESCALE_REG  SSPCPSR
// too fast on prototyp wires: #define SPI_PRESCALE_MIN  2
#define SPI_PRESCALE_MIN  4

#ifndef HW_ENDPOINT_LPC2000_SSPAUTOCS
#define PERIPH_CS_GPIO
#endif

#else
#error "Invalid Interface-Number"
#endif

#define SELECT_CARD()      SPI_IOCLR = (1<<SPI_SS_PIN)
#define UNSELECT_CARD()    SPI_IOSET = (1<<SPI_SS_PIN)

void if_spiSelectDevice(hwInterface *iface)
{
#ifdef PERIPH_CS_GPIO
	SELECT_CARD();          // low = assert
#endif
}

void if_spiUnselectDevice(hwInterface *iface)
{
#ifdef PERIPH_CS_GPIO
	UNSELECT_CARD();        // high = de-assert
	if_spiSend( iface, 0xff );  // release card's DO
#endif
}
/*****************************************************************************/ 

esint8 if_initInterface(hwInterface* file, eint8* opts)
{
	euint32 sc;
	
	if_spiInit(file); /* init at low speed */
	
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
	
	/* increase speed after init */
#if ( HW_ENDPOINT_LPC2000_SPINUM == 1 )
	SSPCR0 = ((8-1)<<0) | (0<<CPOL);
#endif
	if_spiSetSpeed(SPI_PRESCALE_MIN);
	// if_spiSetSpeed(100); /* debug - slower */
	
	/* get file->sectorCount (do not use hardcoded = 4 ) */
	if_getDriveSize(file, &sc);
	file->sectorCount = sc/512;
	if( (sc%512) != 0) {
		file->sectorCount--;
	}
	DBG((TXT("Drive Size is %lu Bytes (%lu Sectors)\n"), sc, file->sectorCount));

	DBG((TXT("Init done...\n")));
	return(0);
}
/*****************************************************************************/ 

void if_spiInit(hwInterface *iface)
{
	euint8 i; 

	// setup GPIO
	SPI_IODIR |= (1<<SPI_SCK_PIN)|(1<<SPI_MOSI_PIN)|(1<<SPI_SS_PIN);
	SPI_IODIR &= ~(1<<SPI_MISO_PIN);
	
	// set Chip-Select high - unselect card
	UNSELECT_CARD();

	// reset Pin-Functions
	SPI_PINSEL &= ~( (3<<SPI_SCK_FUNCBIT) | (3<<SPI_MISO_FUNCBIT) |
		(3<<SPI_MOSI_FUNCBIT) | (3<<SPI_SS_FUNCBIT) );

#if ( HW_ENDPOINT_LPC2000_SPINUM == 0 )
	DBG((TXT("spiInit for SPI(0)\n")));
	SPI_PINSEL |= ( (1<<SPI_SCK_FUNCBIT) | (1<<SPI_MISO_FUNCBIT) |
		(1<<SPI_MOSI_FUNCBIT) );
	// enable SPI-Master
	S0SPCR = (1<<MSTR)|(0<<CPOL); // TODO: check CPOL
#endif

#if ( HW_ENDPOINT_LPC2000_SPINUM == 1 )
	DBG((TXT("spiInit for SSP/SPI1\n")));
	// setup Pin-Functions - disable automatic CS disabled during init
	SPI_PINSEL |= ( (2<<SPI_SCK_FUNCBIT) | (2<<SPI_MISO_FUNCBIT) |
		(2<<SPI_MOSI_FUNCBIT) | (0<<SPI_SS_FUNCBIT) );
	// enable SPI-Master
	SSPCR0 = ((8-1)<<0) | (0<<CPOL) | (0x20<<SCR); //  (0xff<<SCR);
	SSPCR1 = (1<<SSE);
#endif
	
	// low speed during init
	if_spiSetSpeed(254); 

	/* Send 10 spi commands with card not selected */
	for( i=0; i<10; i++ ) {
		if_spiSend(iface,0xff);
	}

	// SPI0 does not offer automatic CS for slaves on LPC213x/4x 

#if ( HW_ENDPOINT_LPC2000_SPINUM == 1 ) && !defined(PERIPH_CS_GPIO)
	// enable automatic slave CS for SSP
	SSPCR1 &= ~(1<<SSE); // disable interface
	SPI_PINSEL |= ( (2<<SPI_SCK_FUNCBIT) | (2<<SPI_MISO_FUNCBIT) |
		(2<<SPI_MOSI_FUNCBIT) | (2<<SPI_SS_FUNCBIT) );
	SSPCR1 |= (1<<SSE); // enable interface
#endif

}
/*****************************************************************************/

void if_spiSetSpeed(euint8 speed)
{
	speed &= 0xFE;
	if ( speed < SPI_PRESCALE_MIN  ) {
		speed = SPI_PRESCALE_MIN ;
	}
	SPI_PRESCALE_REG = speed;
}
/*****************************************************************************/

euint8 if_spiSend(hwInterface *iface, euint8 outgoing)
{
	euint8 incoming;

#if ( HW_ENDPOINT_LPC2000_SPINUM == 0 )
	S0SPDR = outgoing;
	while( !(S0SPSR & (1<<SPIF)) ) { ; }
	incoming = S0SPDR;
#endif

#if ( HW_ENDPOINT_LPC2000_SPINUM == 1 )
	while( !(SSPSR & (1<<TNF)) ) { ; }
	SSPDR = outgoing;
	while( !(SSPSR & (1<<RNE)) ) { ; }
	incoming = SSPDR;
#endif

	return(incoming);
}
/*****************************************************************************/

#ifdef HW_ENDPOINT_LPC2000_SSPFIFO

/*****************************************************************************/ 

/* FIFO Utility-Functions (Mike Anton) */
static void if_spiFastTransmit16(euint16 outgoing)
{
	while(!(SSPSR & (1 << TNF))) { ; }  // Wait until there is room in the FIFO
	SSPDR = outgoing;                     // Send the word
}

static void if_spiEmptyFIFO(void)
{
	euint8 temp;

	// Wait until the SSP is idle, and empty the FIFO
	while (SSPSR & ((1 << BSY) | (1 << RNE))) {
		temp = SSPDR;
	}
}

static void if_spiTransfer16(void)
{
	SSPCR0 |= 0x0000000f;          // Set transfer size to 16bits
}


static void if_spiTransfer8(void)
{
	SSPCR0 &= ~0x00000008;       // Set transfer size to 8bits
}

/*****************************************************************************/ 

static void if_Command(euint8 cmd, euint32 param)
{
	if_spiTransfer16();      // Set transfer size to 16bits

	if_spiFastTransmit16(0xff40 | cmd);

	if_spiFastTransmit16((euint16) (param >> 16));     // Parameter x
	if_spiFastTransmit16((euint16) param);             // Parameter y

	if_spiFastTransmit16(0x95ff);  // Checksum (should be only valid for first command (0), and empty response

	if_spiEmptyFIFO();
	if_spiTransfer8();       // Set transfer size to 8bits
}
/*****************************************************************************/

esint8 if_readBuf(hwInterface* file,euint32 address,euint8* buf)
{
	euint8 cardresp;
	euint8 firstblock;
	euint16 fb_timeout=0xffff;
//	euint32 place;
	euint16 c, i;
	
	if_spiSelectDevice(file);
	
	/*DBG((TXT("readSector_LPC2000::Trying to read sector %u and store it at %p.\n"),address,&buf[0]));*/
//	place=512*address;
//	sd_Command( file, CMDREAD, (euint16) (place >> 16), (euint16) place);
	if_Command(CMDREAD, 512 * address);
	
	cardresp=sd_Resp8b(file); /* Card response */ 

	/* Wait for startblock */
	do {
		firstblock=sd_Resp8b(file); 
	} while(firstblock==0xff && fb_timeout--);

	if( cardresp!=0x00 || firstblock!=0xfe ) {
		sd_Resp8bError(file, firstblock);
		if_spiUnselectDevice(file);
		return(-1);
	}
	
	if_spiTransfer16();      // Set transfer size to 16bits

	// Make sure FIFO is empty (this is likely not needed, as there shouldn't be a transfer in progress)
	if_spiEmptyFIFO();
	
	// Presend 8 words (16 bytes) to fill the FIFO
	for (i = 8; i; i--) {
		SSPDR = 0xffff;
	}

	do {
		while (!(SSPSR & (1 << RNE))) {
			;                     // Wait for a word to be received
		}
		c = SSPDR;

		if (i < (514 - 16)) {     // Need to send 514 bytes, as last word is to fetch the checksum
			SSPDR = 0xffff;        // Send the next word
		}

		*buf++ = c >> 8;          // First byte received is high byte
		*buf++ = (euint8) c;      // Second byte received is low byte
		i += 2;
	} while (i < 512);

	/* Checksum (2 byte) - ignore for now */
	// Don't need to send a checksum here, as there was an extra word sent at the start
//	if_spiFastSend(0xffff);
//	if_spiFastSend(0xff);

	if_spiEmptyFIFO();       // Wait for the FIFO to empty before changing transfer size
	if_spiTransfer8();       // Set transfer size to 8bits

	if_spiUnselectDevice(file);

	return 0;
}
/*****************************************************************************/

esint8 if_writeBuf(hwInterface* file, euint32 address, euint8* buf)
{
	euint16 i;
//	euint32 place;
	
	if_spiSelectDevice(file);
	
	/*DBG((TXT("LPC2000 Trying to write %u to sector %u.\n"),(void *)&buf,address));*/
//	place=512*address;
//	sd_Command(file, CMDWRITE, (euint16) (place >> 16), (euint16) place);
	if_Command(CMDWRITE, 512 * address);

	sd_Resp8b(file);         // Card response
	
	if_spiTransfer16();      // Set transfer size to 16bits

	if_spiFastTransmit16(0xfe00 | *buf++); // Start block, and first data byte

	for (i = 0; i < 510; i += 2) {
		if_spiFastTransmit16( (*buf << 8) | *(buf + 1) ); // Send data
		buf += 2;
	}

	if_spiFastTransmit16( (*buf << 8) | 0xff ); // Last data byte, and first part of checksum

	if_spiEmptyFIFO();
	if_spiTransfer8();                      // Set transfer size to 8bits
	
	if_spiSend(file,0xff);                  // Second part of checksum
	
	while( if_spiSend(file,0xff) != 0xff ) {
		i++; // Wait for write to complete - TODO: use i for timeout
	}

	if_spiUnselectDevice(file);
	
	return 0;
}
/*****************************************************************************/

#else 
/* "non-FIFO": */
/*****************************************************************************/ 

esint8 if_readBuf(hwInterface* file,euint32 address,euint8* buf)
{
	esint8 res;
	
	if_spiSelectDevice(file);
	res = sd_readSector( file, address, buf, 512 );
	if_spiUnselectDevice(file);
	
	return res;
}
/*****************************************************************************/

esint8 if_writeBuf(hwInterface* file, euint32 address, euint8* buf)
{
	esint8 res;
	
	if_spiSelectDevice(file);
	res = sd_writeSector( file, address, buf );
	if_spiUnselectDevice(file);
	
	return res;
}
/*****************************************************************************/ 

#endif /* HW_ENDPOINT_LPC2000_SSPFIFO */

/*****************************************************************************/ 

esint8 if_setPos(hwInterface* file,euint32 address)
{
	return(0);
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
