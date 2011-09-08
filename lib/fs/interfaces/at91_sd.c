//#include "interfaces/AT91SAM7S_regs.h"
#include  "hardware.h"
#include "config.h"
#include "interfaces/at91_sd.h"
#include "interfaces/sd.h"
#include "AT91RM9200.H"
#include "LIB_AT91RM9200.H"
#include <stdio.h>
/*****************************************************************************/

/* SPI prescaler lower limit (the smaller the faster) */
#define SPI_SCBR_MIN     2

/* Define the following to enable the SPI's PDC (DMA) */
#define DMA

/* Define the following to enable software peripheral select
   In this mode the select lines are driven by the software
   thru PIO-registers. This is the preferred mode to satisfy the
   demand: "The CS signal must be continuously active for the
   duration of the SPI transaction (command, response and data)."
   (SanDisk SD Card Product Manual, V1.9, 12/2003)
   Since the AT91 SPI interface offers a lot of possibilities
   (CSAAT / delays / LASTXFER) maybe the CS-handling can
   be done "correctly" in hardware. (but: erratas for SPI in
   the datasheet...) */
#define PERIPH_CS_GPIO

/* Define the following to enable fixed peripheral select.
   This enables the SPI _hardware_ fixed peripheral select
   feature. In the current version this must be defined when
   using DMA/PDC.*/
#define PERIPH_CS_FIXED

/* This enables the SPI _hardware_ variable peripheral select.
   PSCDEC-Mode=0 is used
   "The chip selects are directly connected to a peripheral
   device." This mode is not available when using PDC/DMA */
// #define PERIPH_CS_HWVAR

/*
	AT91SAM7S64 SPI Pins:   Function (A/B)
	PA12 - MISO - AT91C_PA12_MISO  (1/0)
	PA13 - MOSI - AT91C_PA13_MOSI  (1/0)
	PA14 - SCK  - AT91C_PA14_SPCK  (1/0)
	
	Chip-Selects (available on different pins)
	PA11 - NPCS0 - AT91C_PA11_NPCS0 (1/0)
	PA31 - NPCS1 - AT91C_PA31_NPCS1 (1/0)
	PA09 - NPCS1 - AT91C_PA9_NPCS1  (1/0)
	...AT91C_PA3_NPCS3	(0/1)
	...AT91C_PA5_NPCS3	(0/1)
	...AT91C_PA10_NPCS2	(0/1)
	...AT91C_PA22_NPCS3	(0/1)
	...AT91C_PA30_NPCS2 (0/1)
*/

/* CS_BITMASK must be defined for any PERIPH_CS-Mode
   - in Mode PERIPH_CS_GPIO this can be any PIOA Pin in Mode
   - in Modes PERIPH_CS_FIXED and PERIPH_CS_VAR this
     must be a NPCS-Pin */
/*here: use NCPS0 @ PA11: */
#define CS_BITMASK          AT91C_PIO_PA25

/* SPI_CSR_NUM must be defined for any PERIPH_CS-Mode.
   The settings in SPI_CSR[SPI_CSR_NUM] will be used by
   this interface */
#define SPI_CSR_NUM         3

/* ASR/BSR and CSR must be defined for
   PERIPH_CS_FIXED- and PERIPH_CS_HWVAR-Mode *
   (used to set the function in the PIO) */
#define NCPS_PDR_BITMSK     CS_BITMASK
#define NCPS_ASR_BITMSK     CS_BITMASK
#define NPCS_BSR_BITMSK     0


/* Dummy-Array to kick SPI-transfers thru PDC for block read.
   If the array is placed in flash (with const) the transfer
   is a little slower but this saves 512 bytes of RAM. Remove the
   const to place the array in RAM and increase transfer rates.
   Test with    const / Flash : 1435750 bytes/sec
   Test without const / RAM   : 1479289 bytes/sec */
/* Define to place the array in .rodata (typicly ROM/FLASH) */
#define DUMMYFF_IN_ROM


/*---- not more user configuration below this line ----*/

#if (SPI_CSR_NUM != 0)
#warning "This interface has only been tested with SPI_CSR_NUM = 0."
#endif

#if defined(DMA) && defined(PERIPH_CS_HWVAR)
#error "DMA not supported for hardware peripheral chip-select in this version."
// since the the element-size of the in- and out-buffers is
// 1 byte (using PCS in TDR would need 32-bit elements, see
// datasheet section 29.6.3.5 Peripheral Selection)
#endif

#if defined(DMA) && !defined(PERIPH_CS_FIXED)
#error "For DMA fixed peripheral chip-select must be used in this version."
// since the PCS value is set in SPI_MR and not in SPI_TDR
// (8bit per DMA-transfer)
#endif

/* PCS_0 for NPCS0, PCS_1 for NPCS1 ... */
#define PCS_0 ((0<<0)|(1<<1)|(1<<2)|(1<<3))
#define PCS_1 ((1<<0)|(0<<1)|(1<<2)|(1<<3))
#define PCS_2 ((1<<0)|(1<<1)|(0<<2)|(1<<3))
#define PCS_3 ((1<<0)|(1<<1)|(1<<2)|(0<<3))
/* TODO: use ## concat */
#if (SPI_CSR_NUM == 0)
#define SPI_MR_PCS       PCS_0
#elif (SPI_CSR_NUM == 1)
#define SPI_MR_PCS       PCS_1
#elif (SPI_CSR_NUM == 2)
#define SPI_MR_PCS       PCS_2
#elif (SPI_CSR_NUM == 3)
#define SPI_MR_PCS       PCS_3
#else
#error "SPI_CSR_NUM invalid"
#endif

/* in variable periph. select PSDEC=0 is used
   so the already defined values for SPC_MR_PCS can be
   reused for the PCS-value in TDR */
#define SPI_TDR_PCS      SPI_MR_PCS

#ifdef DMA

#define MY_SECTOR_SIZE    512

/* This is the block sent by the PDC to read a sector,
  defined with initilized data to avoid RAM-usage */
#if defined(DUMMYFF_IN_ROM)
static const
#else
static
#endif
euint8 DummyFFArray[MY_SECTOR_SIZE] = {	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF };
#else
	/* rien */
#endif /* DMA */


void if_spiSelectDevice(hwInterface *iface)
{
#ifdef PERIPH_CS_GPIO
    AT91F_PIO_ClearOutput(AT91C_BASE_PIOA, CS_BITMASK);
//	AT91C_BASE_PIOA->PIO_CODR = CS_BITMASK; // low
#endif
}

void if_spiUnselectDevice(hwInterface *iface)
{
#ifdef PERIPH_CS_GPIO
    AT91F_PIO_SetOutput(AT91C_BASE_PIOA, CS_BITMASK);
//	AT91C_BASE_PIOA->PIO_SODR = CS_BITMASK; // high
	if_spiSend( iface, 0xff ); // release card's DO
#endif
}

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
	// if_spiSend(file,0xff);
	
	if_spiSetSpeed(SPI_SCBR_MIN);
	// if_spiSetSpeed(100); /* debug - slower */
	
	/* get file->sectorCount (do not use hardcoded = 4 ) */
	if_getDriveSize(file, &sc);
	file->sectorCount = sc/512;
	if( (sc%512) != 0) {
		file->sectorCount--;
	}
	DBG((TXT("Card Capacity is %lu Bytes (%lu Sectors)\n"), sc, file->sectorCount));
	
	return(0);
}

/*****************************************************************************/
/* parts of this function inspired by a spi example found at olimex.com
   not much (nothing?) left from the olimex-code (since too many hardcoded values)
*/
void if_spiInit_(hwInterface *iface)
{
  DBG((TXT("if_spiInit\n")));
  AT91F_PIO_Enable(AT91C_BASE_PIOA, CS_BITMASK);
  AT91F_PIO_CfgOutput(AT91C_BASE_PIOA, CS_BITMASK);
  AT91F_PIO_SetOutput(AT91C_BASE_PIOA, CS_BITMASK);
	
	// enable peripheral clock for SPI ( PID Bit 5 )
//	pPMC->PMC_PCER = ( (euint32) 1 << AT91C_ID_SPI ); // n.b. IDs are just bit-numbers
  AT91F_PMC_EnablePeriphClock(AT91C_BASE_PMC, ((unsigned int)1<<AT91C_ID_SPI));
}

void if_spiInit(hwInterface *iface)
{
	euint8 i;
	
	AT91PS_SPI pSPI     = AT91C_BASE_SPI;
	AT91PS_PIO pPIOA    = AT91C_BASE_PIOA;
	AT91PS_PMC pPMC     = AT91C_BASE_PMC;
#ifdef DMA
	AT91PS_PDC pSPI_PDC = (AT91PS_PDC) AT91C_SPI_RPR;
#endif
	
	// disable PIO from controlling MOSI, MISO, SCK (=hand over to SPI)
///	pPIOA->PIO_PDR = AT91C_PA12_MISO | AT91C_PA13_MOSI | AT91C_PA14_SPCK; //  | NCPS_PDR_BIT;
	// set pin-functions in PIO Controller
///	pPIOA->PIO_ASR = AT91C_PA12_MISO | AT91C_PA13_MOSI | AT91C_PA14_SPCK; /// not here: | NCPS_ASR_BIT;
	
	// set GPIO for chip-select as output high (unselect card)
//	pPIOA->PIO_PER  = CS_BITMASK; // enable GPIO of CS-pin
//	pPIOA->PIO_SODR = CS_BITMASK; // set high
//	pPIOA->PIO_OER  = CS_BITMASK; // output enable
  AT91F_PIO_Enable(AT91C_BASE_PIOA, AT91C_PIO_PA25);
  AT91F_PIO_CfgOutput(AT91C_BASE_PIOA, AT91C_PIO_PA25);
  AT91F_PIO_SetOutput(AT91C_BASE_PIOA, AT91C_PIO_PA25);
	
	// enable peripheral clock for SPI ( PID Bit 5 )
//	pPMC->PMC_PCER = ( (euint32) 1 << AT91C_ID_SPI ); // n.b. IDs are just bit-numbers
//  AT91F_PMC_EnablePeriphClock(AT91C_BASE_PMC, ((unsigned int)1<<AT91C_ID_SPI));
  AT91F_SPI_CfgPMC();
	// SPI enable and reset
	pSPI->SPI_CR = AT91C_SPI_SPIEN | AT91C_SPI_SWRST;
//  AT91F_SPI_Reset(AT91C_BASE_SPI);
//  AT91F_SPI_Enable(AT91C_BASE_SPI);
#if defined(PERIPH_CS_FIXED)
	// SPI mode: master, fixed periph. sel., FDIV=0, fault detection disabled
	pSPI->SPI_MR  = AT91C_SPI_MSTR | AT91C_SPI_PS_FIXED | AT91C_SPI_MODFDIS;
	// set PCS for fixed select
	// pSPI->SPI_MR &= 0xFFF0FFFF; // clear old PCS - redundant (AT91lib)
	pSPI->SPI_MR |= ( (SPI_MR_PCS<<16) & AT91C_SPI_PCS ); // set PCS
//	AT91F_SPI_CfgMode(AT91C_BASE_SPI, AT91C_SPI_MSTR | AT91C_SPI_MODFDIS | AT91C_SPI_PCS);
#else
	// SPI mode: master, variable periph. sel., FDIV=0, fault detection disabled
	// Chip-Select-Decoder Mode disabled = direct connect
	pSPI->SPI_MR  = AT91C_SPI_MSTR | AT91C_SPI_MODFDIS;
#endif
	
	// set chip-select-register
	// 8 bits per transfer, CPOL=1, ClockPhase=0, DLYBCT = 0
	// TODO: Why has CPOL to be active here and non-active on LPC2000?
	//       Take closer look on timing diagrams in datasheets.
	// not working pSPI->SPI_CSR[SPI_CSR_NUM] = AT91C_SPI_CPOL | AT91C_SPI_BITS_8 | AT91C_SPI_NCPHA;
	// not working pSPI->SPI_CSR[SPI_CSR_NUM] = AT91C_SPI_BITS_8 | AT91C_SPI_NCPHA;
	pSPI->SPI_CSR[SPI_CSR_NUM] = AT91C_SPI_CPOL | AT91C_SPI_BITS_8;
	// not working pSPI->SPI_CSR[SPI_CSR_NUM] = AT91C_SPI_BITS_8;
	
	// slow during init
	if_spiSetSpeed(0xFE);

#ifdef DMA
	// init the SPI's PDC-controller:
	// disable TX and RX
	pSPI_PDC->PDC_PTCR = AT91C_PDC_TXTDIS | AT91C_PDC_RXTDIS;
	// init counters and buffer-pointers to 0
	// "next" TX
	pSPI_PDC->PDC_TNPR = 0;
	pSPI_PDC->PDC_TNCR = 0;
	// "next" RX
	pSPI_PDC->PDC_RNPR = 0;
	pSPI_PDC->PDC_RNCR = 0;
	// TX
	pSPI_PDC->PDC_TPR = 0;
	pSPI_PDC->PDC_TCR = 0;
	// RX
	pSPI_PDC->PDC_RPR = 0;
	pSPI_PDC->PDC_RCR = 0;
	// enable PDC TX and RX
	pSPI_PDC->PDC_PTCR = AT91C_PDC_TXTEN | AT91C_PDC_RXTEN;
#endif

	// enable
	pSPI->SPI_CR = AT91C_SPI_SPIEN;

	/* Send 10 spi commands with card not selected */
	for(i=0; i<10; i++) {
		if_spiSend(iface,0xFF);
	}

#if !defined(PERIPH_CS_GPIO)
	/* enable automatic chip-select for used CS-Pin */
	// reset PIO-registers of CS-pin to default
	pPIOA->PIO_ODR  = CS_BITMASK; // input
	pPIOA->PIO_CODR = CS_BITMASK; // clear
	// disable PIO from controlling the CS pin (=hand over to SPI)
	pPIOA->PIO_PDR = CS_BITMASK;
	// set pin-functions in PIO Controller
//	pPIOA->PIO_ASR = NCPS_ASR_BITMSK;
//	pPIOA->PIO_BSR = NPCS_BSR_BITMSK;
#endif
}
/*****************************************************************************/

void if_spiSetSpeed(euint8 speed)
{
	euint32 reg;
	AT91PS_SPI pSPI      = AT91C_BASE_SPI;

	if ( speed < SPI_SCBR_MIN ) speed = SPI_SCBR_MIN;
	if ( speed > 1 ) speed &= 0xFE;

	reg = pSPI->SPI_CSR[SPI_CSR_NUM];
	reg = ( reg & ~(AT91C_SPI_SCBR) ) | ( (euint32)speed << 8 );
	pSPI->SPI_CSR[SPI_CSR_NUM] = reg;
}
/*****************************************************************************/

euint8 if_spiSend(hwInterface *iface, euint8 outgoing)
{
	euint8 incoming;
	
	AT91PS_SPI pSPI      = AT91C_BASE_SPI;
	
	while( !( pSPI->SPI_SR & AT91C_SPI_TDRE ) ); // transfer compl. wait
#if defined(PERIPH_CS_FIXED)
	pSPI->SPI_TDR = (euint16)( outgoing );
#else
	pSPI->SPI_TDR = ( (euint16)(outgoing) | ((euint32)(SPI_TDR_PCS)<<16) );
#endif
	while( !( pSPI->SPI_SR & AT91C_SPI_RDRF ) ); // wait for char
	incoming = (euint8)( pSPI->SPI_RDR );

	return incoming;
}
/*****************************************************************************/

#ifdef DMA

esint8 if_readBuf(hwInterface* file,euint32 address,euint8* buf)
{
	euint8 cardresp;
	euint8 firstblock;
	euint16 fb_timeout=0xffff;
	euint32 place;
	AT91PS_PDC pPDC = (AT91PS_PDC) AT91C_SPI_RPR;
	
	if_spiSelectDevice(file);

	/*DBG((TXT("sd_readSector::Trying to read sector %u and store it at %p.\n"),address,&buf[0]));*/
	place=512*address;
	cardresp = sd_Command( file, CMDREAD, (euint16) (place >> 16), (euint16) place);

	/* Wait for startblock */
	do {
		firstblock=sd_Resp8b( file );
	} while(firstblock==0xff && fb_timeout--);

	if(cardresp!=0x00 || firstblock!=0xfe){
		sd_Resp8bError(file ,firstblock);
		if_spiUnselectDevice(file);
		return(-1);
	}
	
	// SPI PDC RX buffer:
	pPDC->PDC_RPR  = (euint32) buf; // destination address
	pPDC->PDC_RCR  = 512; // number of frames (here: frame=byte)
	pPDC->PDC_RNCR = 0;
	// SPI PDC TX buffer (dummy bytes):
	pPDC->PDC_TPR = (euint32) DummyFFArray; // source address
	pPDC->PDC_TCR = 512; // number of frames (here: frame=byte)
	// wait for receive-buffer full (counter==0):
	while ( pPDC->PDC_RCR ) {
		;
	}
	
	/* Checksum (2 byte) - ignore for now */
	if_spiSend(file, 0xff);
	if_spiSend(file, 0xff);

	if_spiUnselectDevice(file);
	
	return 0;
}
/*****************************************************************************/

esint8 if_writeBuf(hwInterface* file, euint32 address, euint8* buf)
{
	euint32 place;
	euint16 t=0;
	AT91PS_PDC pPDC = (AT91PS_PDC) AT91C_SPI_RPR;
	AT91PS_SPI pSPI = AT91C_BASE_SPI;
	volatile euint32 dummy;
	
	if_spiSelectDevice(file);

	place=512*address;
	sd_Command(file, CMDWRITE, (euint16) (place >> 16), (euint16) place);

	sd_Resp8b(file); /* Card response */

	if_spiSend(file,0xfe); /* Start block */
	
	// No Receive-Puffering
	pPDC->PDC_RCR  = 0;
	pPDC->PDC_RNCR = 0;
	// SPI PDC TX buffer :
	pPDC->PDC_TPR = (euint32)buf; // source address
	pPDC->PDC_TCR = 512; // number of frames
	// wait for transmit complete (counter=0):
	while ( pPDC->PDC_TCR ) {
		;
	}
	dummy = pSPI->SPI_SR;   // read status to clear flags
	dummy = pSPI->SPI_RDR;  // empty receiver-register
	
	
	if_spiSend(file,0xff); /* Checksum part 1 */
	if_spiSend(file,0xff); /* Checksum part 2 */

	if_spiSend(file,0xff);

	while( if_spiSend(file,0xff)!=0xff ) {
		t++;
		/* wuff wuff deadlock - TODO: use t for timeout*/
	}

	if_spiUnselectDevice(file);
	
	return 0;
}
/*****************************************************************************/

#else

esint8 if_readBuf(hwInterface* file,euint32 address,euint8* buf)
{
	esint8 res;
	
	if_spiSelectDevice(file);
	res = sd_readSector(file,address,buf,512);
	if_spiUnselectDevice(file);
	
	return res;
}
/*****************************************************************************/

esint8 if_writeBuf(hwInterface* file, euint32 address, euint8* buf)
{
	esint8 res;
	
	if_spiSelectDevice(file);
	res = sd_writeSector(file, address, buf);
	if_spiUnselectDevice(file);
	
	return res;
}

#endif /* DMA */

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
