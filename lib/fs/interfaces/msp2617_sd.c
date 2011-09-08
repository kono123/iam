#include  "hardware.h"
#include "config.h"
/*****************************************************************************/
#include "sd.h"
#include "debug.h"

/*****************************************************************************/
void if_spiSelectDevice(hwInterface* file)
{
  SD_CS_Activate();
}
/*****************************************************************************/
void if_spiUnselectDevice(hwInterface* file)
{
  SD_CS_Diactivate();
  if_spiSend(file,0xff); /* release DO */
}
/*****************************************************************************/
euint8 if_spiSend(hwInterface *iface, euint8 outgoing)
{
  UCB0TXBUF = outgoing;
  while(UCB0STAT & UCBUSY);
  return UCB0RXBUF;
}

void if_spiReadBlock(hwInterface *iface, euint8 *ptr, euint16 bytes)
{
  ptr--;
  while(bytes--){
    UCB0TXBUF = 0xff;
    ptr++;
    while(UCB0STAT & UCBUSY);
    *ptr=UCB0RXBUF;
  }
}

//void dmemcpy(unsigned int *dist,unsigned int *src,unsigned int words)
void if_spiWriteBlock_(hwInterface *iface, euint8 *ptr, euint16 bytes)
{
  while(DMA1CTL & DMAREQ);
  DMACTL0 = DMA1TSEL_13;
  DMA1SA = (void __data20*)ptr;                  // Start block address
  DMA1DA = (void __data20*)&UCB0TXBUF;                       // Destination block address
  DMA1SZ = bytes;                          // Block size
  DMA1CTL = DMADT_4 + DMASRCINCR_3 + DMASBDB ;
  DMA1CTL |= DMAEN;                         // Enable DMA0
//  DMA0CTL |= DMAREQ;                      // Trigger block transfer
}

void if_spiWriteBlock(hwInterface *iface, euint8 *ptr, euint16 bytes)
{
  while(bytes){
    UCB0TXBUF = *ptr;
    ptr++;
    bytes--;
    while(UCB0STAT & UCBUSY);
//    dummy=UCB0RXBUF;
  }
}

/*****************************************************************************/
void if_spiInit(hwInterface *iface)
{
  euint8 i;

  __disable_interrupt();

  P3OUT |= (1<<DD_CS);
  P3DIR |= (1<<DD_MOSI) | (1<<DD_SCK) | (1<<DD_CS);
  P3DIR &= ~(1<<DD_MISO);

  P3SEL |= 0x0E; //Primary peripheral

  UCB0CTL1 |= UCSWRST;

//  UCB0CTL0 = UCMSB + UCMST + UCSYNC + UCCKPH; //+ UCCKPL+ UCCKPH;
  UCB0CTL0 =  UCMSB + UCMST + UCSYNC + UCCKPL;
  UCB0CTL1 |= 0xC0;

  UCB0BR0 = 0;
  UCB0BR1 = 0;

  UCB0CTL1 &= ~UCSWRST;
  __enable_interrupt();
		
	/* Send 10 spi commands with card not selected */
  for(i=0;i<10;i++)
    if_spiSend(iface,0xff);
  iface->CardType = 0;
}
/*****************************************************************************/
esint8 if_initInterface(hwInterface* file, eint8* opts)
{
  euint32 sc;
//  euint32 _all,_free;

  if_spiInit(file);
	
  if_spiSelectDevice(file);
	
  if(sd_Init(file)<0)	{
//		DBG((TXT("Card failed to init, breaking up...\n")));
    if_spiUnselectDevice(file);
    return(-1);
  }

  if(sd_State(file)<0){
//		DBG((TXT("Card didn't return the ready state, breaking up...\n")));
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
//printf("Card Capacity is %lu Bytes (%lu Sectors)\n", sc, file->sectorCount);
//	DBG((TXT("Init done...\n")));
  return(0);
}
/*****************************************************************************/
esint8 if_readBuf(hwInterface* file,euint32 address,euint8* buf)
{
/*
	esint8 res;
	
	if_spiSelectDevice(file);
	res = sd_readSector(file,address,buf,512);
	if_spiUnselectDevice(file);
  */
  euint8 cardresp;
  euint8 firstblock;
  euint16 fb_timeout=0xffff;
  euint32 place;
//  euint16 btr;
  int t;

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

//  btr=512;

//  while(btr--){
//    *buf++=if_spiSend( file, 0xff );
//  }
  if_spiReadBlock(file,buf,512);

  cardresp=if_spiSend(file, 0xff);
  cardresp=if_spiSend(file, 0xff);

  cardresp=if_spiSend(file, 0xff);
  cardresp=if_spiSend(file, 0xff);
  cardresp=if_spiSend(file, 0xff);
  cardresp=if_spiSend(file, 0xff);

//  while( if_spiSend(file,0xff)!=0xff ) {
//		t++;
//		/* wuff wuff deadlock - TODO: use t for timeout*/
//  }

  if_spiUnselectDevice(file);
	
  return(0);
}

esint8 if_readSector(hwInterface* file,euint32 address,euint8* buf)
{
  euint8 cardresp;
  euint8 firstblock;
  euint16 fb_timeout=0xffff;
  euint32 place;
//  euint16 btr;
  int t;

  if_spiSelectDevice(file);

//printf("read sector 0x%lx\n",address);

  if(file->CardType & 0x08){
    place=address;
//    resp = sd_Command(file, CMDREAD, (euint16) (address >> 16), (euint16) address);
  }else{
    place=address<<9;  // *512
//    resp = sd_Command(file, CMDREAD, (euint16) (address >> 7), (euint16) (address << 9));
  }
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

  if_spiReadBlock(file,buf,512);

  cardresp=if_spiSend(file, 0xff);
  cardresp=if_spiSend(file, 0xff);
/*
  cardresp=if_spiSend(file, 0xff);
  cardresp=if_spiSend(file, 0xff);
  cardresp=if_spiSend(file, 0xff);
  cardresp=if_spiSend(file, 0xff);
*/
//  while( if_spiSend(file,0xff)!=0xff ) {
//		t++;
//		/* wuff wuff deadlock - TODO: use t for timeout*/
//  }

  if_spiUnselectDevice(file);
	
  return(0);
}
/*****************************************************************************/
esint8 if_writeBuf(hwInterface* file,euint32 address,euint8* buf)
{
  /*
  esint8 res;
	
  if_spiSelectDevice(file);
  res = sd_writeSector(file,address, buf);
  if_spiUnselectDevice(file);
	
  return(res);
  */
  euint32 place;
  euint16 t=0;
//	volatile euint32 dummy;
//  euint16 btr;           /* Byte count (must be even number) */
	
  if_spiSelectDevice(file);

  place=512*address;
  sd_Command(file, CMDWRITE, (euint16) (place >> 16), (euint16) place);

  sd_Resp8b(file); // Card response

  if_spiSend(file,0xfe); // Start block

//  btr=512;

//  while(btr--){
//    if_spiSend(file, *buf++);
//  }
  if_spiWriteBlock(file,buf,512);
	
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

esint8 if_writeSector(hwInterface* file,euint32 address,euint8* buf)
{
  euint32 place;
  euint32 t=0;
  euint8 resp;
//	volatile euint32 dummy;
//  euint16 btr;           /* Byte count (must be even number) */

//printf("write sector 0x%x\n",address);
	
  if_spiSelectDevice(file);

  if(file->CardType & 0x08){
    place=address;
//    resp = sd_Command(file, CMDWRITE, (euint16) (address >> 16), (euint16) address);
  }else{
    place=address<<9;  // *512
//    resp = sd_Command(file, CMDWRITE, (euint16) (address >> 7), (euint16) (address << 9));
  }

  resp = sd_Command(file, CMDWRITE, (euint16) (place >> 16), (euint16) place);
  if(resp){
    if_spiSend(file,0xff); // Send dummy
    if_spiSend(file,0xff); // Send dummy
    if_spiUnselectDevice(file);
    return -1;
  }

//  sd_Resp8b(file); // Card response

  if_spiSend(file,0xfe); // Start block

//  if_spiSend(file,0xff); // Send dummy

  if_spiWriteBlock(file,buf,512);
	
  if_spiSend(file,0xff); /* Checksum part 1 */
  if_spiSend(file,0xff); /* Checksum part 2 */

  resp = 0;
  resp = if_spiSend(file,0xff);
//  if((resp & 0x1F) != 0x05) {
  while((resp & 0x1F) != 0x05) {
    if_spiSend(file,0xff); // Send dummy
    if_spiSend(file,0xff); // Send dummy
    if_spiUnselectDevice(file);
    return -2;
  }

  resp = if_spiSend(file,0xff); // Send dummy
  resp = if_spiSend(file,0xff); // Send dummy

  t=0;
//  while( if_spiSend(file,0xff)!=0xff ) {
  while(resp != 0xFF) {
    resp = if_spiSend(file,0xff);
    t++;
    if(t>40000){
      if_spiSend(file,0xff); // Send dummy
      if_spiSend(file,0xff); // Send dummy
      if_spiUnselectDevice(file);
      return -3;
    }
  }

  if_spiUnselectDevice(file);
	
  return 0;
}
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
