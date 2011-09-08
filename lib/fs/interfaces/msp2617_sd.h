#ifndef __MSP2617_SD_H_
#define __MSP2617_SD_H_

#define DDR_SPI DDRB
#define DD_MISO 2
#define DD_MOSI 1
#define DD_SCK  3
#define DDR_CS  DDRB
#define DD_CS   0
#define PORT_CS PORTB

//#include <avr/io.h>
//#include <compat/ina90.h>
//#include "../debug.h"
#include "config.h"
#include "types.h"



/*************************************************************\
              hwInterface
               ----------
* FILE* 	imagefile		File emulation of hw interface.
* long		sectorCount		Number of sectors on the file.
\*************************************************************/
struct  hwInterface{
	/*FILE 	*imageFile;*/
	eint32  	sectorCount;
        eint8           CardType;
};
typedef struct hwInterface hwInterface;

esint8 if_initInterface(hwInterface* file,eint8* opts);
esint8 if_readBuf(hwInterface* file,euint32 address,euint8* buf);
esint8 if_writeBuf(hwInterface* file,euint32 address,euint8* buf);
esint8 if_writeSector(hwInterface* file,euint32 address,euint8* buf);
esint8 if_readSector(hwInterface* file,euint32 address,euint8* buf);
esint8 if_setPos(hwInterface* file,euint32 address);

esint8 if_getDriveSize(hwInterface *iface, euint32* drive_size );

void if_spiInit(hwInterface *iface);
euint8 if_spiSend(hwInterface *iface, euint8 outgoing);
void if_spiReadBlock(hwInterface *iface, euint8 *ptr, euint16 bytes);
void if_spiWriteBlock(hwInterface *iface, euint8 *ptr, euint16 bytes);
void if_spiSelectDevice(hwInterface *iface);
void if_spiUnselectDevice(hwInterface *iface);

#endif
