#include  "hardware.h"
#include "config.h"
/*****************************************************************************/
#include "sd.h"
#include "debug.h"
/*****************************************************************************/

/* The GCC's optimizer should inline */

void if_spiSelectDevice(hwInterface* file)
{
  P3OUT &= ~(1<<DD_CS);
}

void if_spiUnselectDevice(hwInterface* file)
{
  if_spiSend(file,0xff); /* release DO */
  P3OUT |= (1<<DD_CS);
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
	
	/* get file->sectorCount (do not use hardcoded = 4 ) */
	if_getDriveSize(file, &sc);
	file->sectorCount = sc/512;
	if( (sc%512) != 0) {
		file->sectorCount--;
	}
	DBG((TXT("Card Capacity is %lu Bytes (%lu Sectors)\n"), sc, file->sectorCount));
	
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

  __disable_interrupt();

  UCB0CTL1 |= UCSWRST;

  UCB0CTL0 = UCMSB + UCMST + UCSYNC; //+ UCCKPL+ UCCKPH;
//  UCB0CTL0 = UCMSB + UCMST + UCSYNC + UCCKPL+ UCCKPH;
  UCB0CTL1 |= 0xC0;

  UCB0BR0 = 2;
  UCB0BR1 = 0;

  P3SEL |= 0x0E; //Primary peripheral

  P3OUT |= (1<<DD_CS);
  P3DIR |= (1<<DD_MOSI) | (1<<DD_SCK) | (1<<DD_CS);


  UCB0CTL1 &= ~UCSWRST;
  __enable_interrupt();
		

	/* Send 10 spi commands with card not selected */
  for(i=0;i<10;i++)
    if_spiSend(iface,0xff);

	/* Select card */
	/* PORTB &= 0xFE; moved to interfaceInit to make clear
	   that card has to be selected before calling sd_init() */
  P3OUT &= ~(1<<DD_CS);
}
/*****************************************************************************/
euint8 if_spiSend(hwInterface *iface, euint8 outgoing)
{
  UCB0TXBUF = outgoing;
  while(UCB0STAT & UCBUSY);
  return UCB0RXBUF;
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
