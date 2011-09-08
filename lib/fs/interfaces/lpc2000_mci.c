/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : lpc2000_mci.c                                                    *
* Description : This file contains the functions needed to use efs for        *
*               accessing files on an SD-card connected to an LPC2368 or      *
*               LPC2378 through the dedicated SD/MMC interface (it might      *
*               even work on LPC24xx, LPC208xx and LPC3180).                   *
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
*                                                    (c)2007 Juri Haberland   *
\*****************************************************************************/

/* minor modifications from Martin Thomas:
	- volatiles for delay-loop counters
	- format-specifiers %.8X -> %08x
	- helper function -> static
	- FIFO read/write from/to unaligned buffer
	- option for extended debugging-outputs only in this module
*/
 
// #define DEBUG_MCI
#define TEST_MTHOMAS

#ifdef DEBUG_MCI
#define DBGMCI DBG
#else
#define DBGMCI(x__) ;
#endif
 
/*****************************************************************************/
/* #include <LPC23xx.H> */
#include "interfaces/LPC2000_regs.h"
#include "config.h"
#include "extract.h"
#include "interfaces/lpc2000_mci.h"
#include "interfaces/mmc.h"
/*****************************************************************************/

esint8 if_initInterface(hwInterface * card, eint8 * opts)
{
    euint32 card_size;

    if_mci_init();

    if (mci_card_init(card) < 0) {
        DBG((TXT("Card failed to init, giving up.\n")));
        return(EFSL_ERR_FAIL);
    }

    if (mci_get_disk_size(card, &card_size) == 0) {
        card->sectorCount = card_size / DISK_SECTOR_SIZE;
    } else {
        /* getting disk size failed */
        card->sectorCount = 0;
    }
    
	switch (card->type) {
        case CARD_TYPE_MMC:
            DBG((TXT("MMC ")));
			break;
        case CARD_TYPE_SDSC:
			DBG((TXT("SDSC ")));
			break;
        case CARD_TYPE_SDHC:
            DBG((TXT("SDHC ")));
			break;
        default:
            DBG((TXT("Unknown card")));
            break;
    }
	DBG((TXT("card size: %u bytes, sector count: %u\n"), card_size, card->sectorCount));

    DBG((TXT("Init done.\n")));
    return(EFSL_ERR_OK);
}
/*****************************************************************************/

esint8 if_readBuf(hwInterface * card, euint32 block_no, euint8 * buf)
{
    return(mci_read_block(card, block_no, buf));
}
/*****************************************************************************/

esint8 if_writeBuf(hwInterface * card, euint32 block_no, euint8 * buf)
{
    return(mci_write_block(card, block_no, buf));
}
/*****************************************************************************/


/**********************************************************
 * name:       mci_sendcmd()
 **********************************************************
 * sends a MMC command to the card
 ********************************************************** 
 * arguments:  MMC command, argument to command, flags for
 *             command
 * return val: status
 **********************************************************/
static euint32 mci_sendcmd(euint16 cmd, euint32 arg, euint32 flags)
{
    euint32 status;
    volatile euint32 count;

    /* always enable the command path state machine */
    cmd |= MMC_CMD_ENABLE_CPSM;

    /* merge the flags with the command */
    cmd |= flags;

    /* write argument into argument register */
    MCI_ARGUMENT = arg;

    /* send command and (roughly) wait 3x MCLK + 2x PCLK periods */
    MCI_COMMAND = cmd;
    for (count = TIME_OUT_VAL; count > 0; count--);

    /* wait for completion, response, timeout, error... */
    do {
        status = MCI_STATUS;
    } while (status & MCI_STAT_CMD_ACTIVE);

    /* clear all resetable status flags */
    MCI_CLEAR = MCI_STATUS_CLEAR_MASK;

    return(status);
}


/**********************************************************
 * name:       if_mci_init()
 **********************************************************
 * initializes the MCI hardware
 ********************************************************** 
 * arguments:  none
 * return val: none
 **********************************************************/
void if_mci_init(void)
{
    volatile euint32 count;  /* used in for-loops */

    DBGMCI((TXT("init started\n")));

    /* enable the SD/MMC card interface pins */
    PINSEL1 = (PINSEL1 & ~0x00003FC0) | 0x00002A80;
    PINSEL4 = (PINSEL4 & ~0x0FC00000) | 0x0A800000;

    /* enable the power and clock of the SD/MMC card interface */
    PCONP |= 0x10000000;

    /* reset command register and data control register */
    MCI_COMMAND = 0;
    MCI_DATA_CTRL = 0;
    /* clear all status bits */
    MCI_CLEAR = MCI_STATUS_CLEAR_MASK;

    /* in case power was on, turn it off and reset bus mode and Rod to push-pull mode */
    MCI_POWER = MMC_POWER_OFF;
    for (count = 5000; count > 0; count--);

    /* enable power to the SD card, wait for the power to stabilize */
    MCI_POWER |= MMC_POWER_UP;
    while (!(MCI_POWER & MMC_POWER_UP));
    /* wait at least 1us (cclk = 57,6MHz) */
    for (count = TIME_OUT_VAL; count > 0; count--);

    /* set clock to 400kHz, enable MCI bus clock, disable clock if bus is idle, no bypass, no wide bus */
    MCI_CLOCK = MCI_CLK_ENABLE | MCI_CLK_PWRSAVE | MCI_CLK_DIV_LOW_SPEED;
    for (count = 2400; count > 0; count--);

    MCI_POWER |= MMC_POWER_ON;
    for (count = 50000; count > 0; count--);

    DBGMCI((TXT("init done\n")));
}


/**********************************************************
 * name:       mci_card_init()
 **********************************************************
 * initializes the MMC card
 ********************************************************** 
 * arguments:  pointer to interface structure
 * return val: status
 **********************************************************/
esint8 mci_card_init(hwInterface * card)
{
    volatile euint32 count;
	euint32 status;
    euint32 response;
    euint32 cmd41_arg;

    DBGMCI((TXT("card init started\n")));
    card->type = CARD_TYPE_UNKNOWN;

    /* start card identification mode */

    /* reset card */
    /* send CMD0 */
    status = mci_sendcmd(MMC_GO_IDLE_STATE, 0, MMC_CMD_NO_RESPONSE);
    for (count = 2400; count > 0; count--);
    if (status & MCI_STAT_CMD_TIMEOUT) {
        DBG((TXT("Timeout CMD0  status:   0x%08x\n"), status));
    }

    /* set push-pull bus mode for SD cards and disable Rod */
    MCI_POWER &= ~(MMC_BUS_MODE_OPEN_DRAIN | MMC_BUS_MODE_ROD);
    for (count = TIME_OUT_VAL; count > 0; count--);

    /* initialization for SD cards >= v2.00 - *must* be done first! */
    /* send power range (2.7 - 3.6V) to card; may time out if:
       - card cannot operate within the power range or
       - card does not understand CMD8 (MMC and pre-v2.00 SD cards) */
    /* send CMD8 */
    status = mci_sendcmd(SD_SEND_IF_COND, 0x000001AA, MMC_CMD_RESPONSE);
    if (status & MCI_STAT_CMD_TIMEOUT) {
        /* not a SD card >= v2.00, so don't check for high capacity */
        cmd41_arg = MMC_OCR_HIGH_VOLTAGE_RANGE;
		DBGMCI((TXT("not a SD-CARD >= V2.00\n")));
        DBGMCI((TXT("Timeout CMD8  status:   0x%08x\n"), status));
    } else {
        /* no timeout, this must be a SD card >= v2.00, so check for high capacity */
        card->type = CARD_TYPE_SDV2;
        cmd41_arg = MMC_OCR_CARD_HIGH_CAPACITY | MMC_OCR_HIGH_VOLTAGE_RANGE;
    }

    /* another check for the cards power range - this works also for older SD cards */
    /* send power range (2.7 - 3.6V) to card; may time out if:
       - card cannot not operate within the power range or
       - card does not understand ACMD41 (MMC cards) */
    count = TIME_OUT_VAL;
    do {
        /* send CMD55 */
        mci_sendcmd(MMC_APP_CMD, 0, MMC_CMD_RESPONSE);
        /* send ACMD41 */
        status = mci_sendcmd(SD_APP_OP_COND, cmd41_arg, MMC_CMD_RESPONSE);
        response = MCI_RESP0;
    } while (!(status & MCI_STAT_CMD_TIMEOUT) && !(response & MMC_OCR_CARD_NOT_BUSY) && --count);

    if ((status & MCI_STAT_CMD_TIMEOUT) || (count == 0)) {
        /* CMD55/ACMD41 timed out - so it's either:
         - a MMC card
         - a SD card with incompatible power range
         - or no card at all */
		DBG((TXT("Timeout ACMD41 (-> a MMC, SD with incomp. power-range or no card)\n")));
        DBG((TXT("ACMD41 count:    %i\n"), TIME_OUT_VAL - count));
        DBG((TXT("ACMD41 status:   0x%08x\n"), status));
        DBG((TXT("ACMD41 Response: 0x%08x\n"), response));

        /* set open drain bus mode and enable Rod for MMC card initialization */
        MCI_POWER |= MMC_BUS_MODE_OPEN_DRAIN | MMC_BUS_MODE_ROD;
        for (count = TIME_OUT_VAL; count > 0; count--);

        count = TIME_OUT_VAL;
        do {
            /* send CMD1 */
            status = mci_sendcmd(MMC_SEND_OP_COND, MMC_OCR_HIGH_VOLTAGE_RANGE, MMC_CMD_RESPONSE);
            response = MCI_RESP0;
        } while (!(status & MCI_STAT_CMD_TIMEOUT) && !(response & MMC_OCR_CARD_NOT_BUSY) && --count);

        if ((status & MCI_STAT_CMD_TIMEOUT) || (count == 0)) {
            /* no card */
			DBG((TXT(" Timeout CMD1\n")));
            DBG((TXT(" CMD1  count:    %i\n"), TIME_OUT_VAL - count));
            DBG((TXT(" CMD1  status:   0x%08x\n"), status));
            DBG((TXT(" CMD1  Response: 0x%08x\n"), response));
            return(EFSL_ERR_FAIL);
        } else {
            /* it's a MMC card - finally */
            card->type = CARD_TYPE_MMC;
        }
    } else {
        /* it's a SD card */
        if (response & MMC_OCR_CARD_HIGH_CAPACITY) {
            card->type |= CARD_TYPE_SDHC;
        } else {
            card->type |= CARD_TYPE_SDSC;
        }            
    }

    /* send CMD2 */
    status = mci_sendcmd(MMC_ALL_SEND_CID, 0, MMC_CMD_RESPONSE | MMC_CMD_LONGRSP);
    if (status != MCI_STAT_CMD_RESP_END) {
        DBG((TXT("No CMD_RESP_END CMD2  status:   0x%08x\n"), status));
        return(EFSL_ERR_FAIL);
    }

    /* send CMD3 */
    if (card->type & CARD_TYPE_MMC) {
        /* MMC cards need the RCA to be set by the host */
        status = mci_sendcmd(MMC_SET_RELATIVE_ADDR, MMC_CARD_ADDRESS, MMC_CMD_RESPONSE);
        /* expect a short answer of type R1 */
    } else {
        /* it's a SD card, so just query the RCA from the card */
        status = mci_sendcmd(SD_SEND_RELATIVE_ADDR, 0, MMC_CMD_RESPONSE);
        /* expect a short answer of type R6 (including the RCA of the card) */
    }
    if (status == MCI_STAT_CMD_RESP_END) {
        if (card->type & CARD_TYPE_MMC) {
            /* MMC: we assigned the address */
            card->address = MMC_CARD_ADDRESS;
        } else {
            /* SD: we got the address from the card */
            card->address = MCI_RESP0 & 0xFFFF0000;
        }
    } else {
        DBG((TXT("No CMD_RESP_END CMD3  status:   0x%08x\n"), status));
        return(EFSL_ERR_FAIL);
    }

    /* set clock to 14.4MHz, enable MCI bus clock, disable clock if bus is idle, no bypass, no wide bus */
    switch (card->type) {
        case CARD_TYPE_MMC:
            /* fmax is 20 MHz */
            MCI_CLOCK = MCI_CLK_ENABLE | MCI_CLK_PWRSAVE | MCI_CLK_DIV_HIGH_SPEED;
            for (count = TIME_OUT_VAL; count > 0; count--);
            /* set push-pull bus mode and disable Rod */
            MCI_POWER &= ~(MMC_BUS_MODE_OPEN_DRAIN | MMC_BUS_MODE_ROD);
            for (count = TIME_OUT_VAL; count > 0; count--);
            break;
        case CARD_TYPE_SDSC:
        case CARD_TYPE_SDHC:
            /* fmax is 25 MHz */
            MCI_CLOCK = MCI_CLK_ENABLE | MCI_CLK_PWRSAVE | MCI_CLK_DIV_HIGH_SPEED;
            for (count = TIME_OUT_VAL; count > 0; count--);
            break;
        default:
            /* unkown card - we stay at 400kHz */
            break;
    }

    DBGMCI((TXT("card init done\n")));

    /* end of initialization */
    return(EFSL_ERR_OK);
}


/**********************************************************
 * name:       mci_get_disk_size()
 **********************************************************
 * calculates the disk size from special registers on the
 * MMC card
 ********************************************************** 
 * arguments:  pointer to interface structure, pointer to
 *             32bit unsigned int which will be filled with
 *             disk size
 * return val: status: 0 on success
 **********************************************************/
euint8 mci_get_disk_size(hwInterface * card, euint32 * card_size)
{
    euint32 status;
    euint8 c_size_mult;
    euint16 c_size;
    euint16 read_bl_len;
    esint8 retval;

    DBGMCI((TXT("get_disk_size started\n")));

    /* get CSD register from card (CMD9) */
    status = mci_sendcmd(MMC_SEND_CSD, card->address, MMC_CMD_RESPONSE | MMC_CMD_LONGRSP);

    if (status == MCI_STAT_CMD_RESP_END) {
        /* ok, response received */
        /* extract the values from the response */
        read_bl_len = (MCI_RESP1 & 0x000F0000) >> 16;
        c_size = (MCI_RESP1 & 0x000003FF) << 2;
        c_size |= (MCI_RESP2 & 0xC0000000) >> 30;
        c_size_mult = (MCI_RESP2 & 0x00038000) >> 15;
        /* for formula see SD Specification Part 1: Physical Layer V2.00, p. 81 */
        *card_size = (1 << read_bl_len) * (c_size + 1) * (1 << (c_size_mult + 2));
        retval = 0;
    } else {
        /* bad, something went wrong */
        DBG((TXT("No CMD_RESP_END CMD9  status:   0x%08x\n"), status));
        *card_size = 0;
        retval = -1;
    }

    DBGMCI((TXT("get_disk_size done\n")));

    return(retval);
}


/**********************************************************
 * name:       mci_block_read()
 **********************************************************
 * reads the block with the given block number into the
 * given buffer; a block is always 512 bytes in size,
 * therefore the buffer must also be 512 bytes in size
 ********************************************************** 
 * arguments:  pointer to interface structure, block number,
 *             pointer to a 512 bytes buffer
 * return val: status: 0 on success
 **********************************************************/
esint8 mci_read_block(hwInterface * card, euint32 block_no, euint8 * buf)
{
    euint32 status;
    euint32 response;
    volatile euint32 count = 0;
	
    DBGMCI((TXT("read sector: %u\n"), block_no));

    /* select card for further usage (CMD7) */
    status = mci_sendcmd(MMC_SELECT_CARD, card->address, MMC_CMD_RESPONSE);
    if (status != MCI_STAT_CMD_RESP_END) {
		DBG((TXT(" No CMD_RESP_END CMD7\n")));
        DBG((TXT(" CMD7  status:   0x%08x\n"), status));
        DBG((TXT(" CMD7  Response: 0x%08x\n"), MCI_RESP0));
        return(EFSL_ERR_FAIL);
    }

    /* if it is a SD card, turn on 4bit modus */
    /* this needs DMA support, as the processor is not fast enough to */
//    if ((card->type == CARD_TYPE_SDSC) || (card->type == CARD_TYPE_SDHC)) {
//        /* configure MCI host adapter for 4bit modus */
//        MCI_CLOCK |= MCI_CLK_WIDE_BUS;
//        for (count = TIME_OUT_VAL; count > 0; count--);
//        /* send CMD55 */
//        mci_sendcmd(MMC_APP_CMD, card->address, MMC_CMD_RESPONSE);
//        /* send ACMD6 */
//        status = mci_sendcmd(SD_APP_SET_BUS_WIDTH, MMC_BUS_WIDTH_4_BIT, MMC_CMD_RESPONSE);
//        if (status != MCI_STAT_CMD_END) {
//            DBGMCI((TXT("ACMD6  status:   0x%08x\n"), status));
//            DBGMCI((TXT("ACMD6  Response: 0x%08x\n"), MCI_RESP0));
//        }
//    }

    /* prepare data transfer */
    /* set data read timeout */
    MCI_DATA_TMR = 0x00100000;
    /* set amount of data to be received */
    MCI_DATA_LEN = MMC_BLOCK_SIZE;
    /* enable data receiving */
    MCI_DATA_CTRL = MCI_DATA_TRANS_EN | MCI_DATA_TO_HOST | MCI_DATA_BLOCK_MODE | MCI_DATA_BLOCK_SIZE_512;

    /* initiate data read transfer (CMD17) */
    if (card->type == CARD_TYPE_SDHC) {
        /* high capacity SD cards use the block number as address parameter */
        status = mci_sendcmd(MMC_READ_SINGLE_BLOCK, block_no, MMC_CMD_RESPONSE);
    } else {
        /* standard capacity SD cards and MMC cards use the memory address as address parameter */
        status = mci_sendcmd(MMC_READ_SINGLE_BLOCK, block_no * MMC_BLOCK_SIZE, MMC_CMD_RESPONSE);
    }
    if (!(status & MCI_STAT_CMD_RESP_END)) {
		DBG((TXT(" No CMD_RESP_END CMD17 - init read\n")));
        DBG((TXT(" CMD17 status:   0x%08x\n"), status));
        DBG((TXT(" CMD17 Response: 0x%08x\n"), MCI_RESP0));
        return(EFSL_ERR_FAIL);
    }

    /* read the data from the bus and store it in the buffer */
    count = 0;
    do {
        while ((MCI_STATUS & MCI_STAT_RX_DATA_AVLBL) && (count < (MMC_BLOCK_SIZE / sizeof(euint32)))) {
            /* mthomas - alignment: ((euint32 *)buf)[count] = MCI_FIFO; */
			ex_setb32(buf, count*4, MCI_FIFO);
            count++;
        }
    } while (!(MCI_STATUS & (MCI_STAT_DATA_TIMEOUT | MCI_STAT_RX_OVERRUN | MCI_STAT_DATA_END | MCI_STAT_DATA_BLOCK_END)) && (count < (MMC_BLOCK_SIZE / sizeof(euint32))));

    /* did we get the correct number of bytes? */
    if (count != MMC_BLOCK_SIZE / sizeof(euint32)) {
		DBG((TXT("Size mismatch read\n")));
        DBG((TXT("read status:   0x%08x\n"), MCI_STATUS));
        DBG((TXT("read count:    %u\n"), count));
        return(EFSL_ERR_FAIL);
    }
    MCI_CLEAR = MCI_STATUS_CLEAR_MASK;

    /* reset data control register in MCI */
    MCI_DATA_CTRL = 0;
    for (count = TIME_OUT_VAL; count > 0; count--);

    /* get card status (CMD13) */
    status = mci_sendcmd(MMC_SEND_STATUS, card->address, MMC_CMD_RESPONSE);
    response = MCI_RESP0;
    if (response != (MMC_C_S_READY_FOR_DATA | MMC_C_S_STATE_TRAN)) {
		DBG((TXT(" read: only C_S_READY_FOR_DATA |  MMC_C_S_STATE_TRAN expected\n")));
        DBG((TXT(" CMD13 status:   0x%08x\n"), status));
        DBG((TXT(" CMD13 Response: 0x%08x\n"), response));
		DBG((TXT(" trying STOP_TRANSMISSION\n"), response));
        /* something went wrong - try to stop the data transfer (CMD12) */
        mci_sendcmd(MMC_STOP_TRANSMISSION, 0, MMC_CMD_RESPONSE);
        /* get card status again (CMD13) */
        status = mci_sendcmd(MMC_SEND_STATUS, card->address, MMC_CMD_RESPONSE);
        if (!(status & MCI_STAT_CMD_RESP_END)) {
            /* still not good - bail out */
			DBG((TXT(" read: No CMD_RESP_END from STOP_TRANMS\n")));
            DBG((TXT(" CMD13 status:   0x%08x\n"), status));
            DBG((TXT(" CMD13 Response: 0x%08x\n"), MCI_RESP0));
            return(EFSL_ERR_FAIL);
        }
    }

    /* deselect card (CMD7) */
    status = mci_sendcmd(MMC_SELECT_CARD, 0, MMC_CMD_NO_RESPONSE);
    if (!(status & MCI_STAT_CMD_SENT)) {
		DBG((TXT(" read: CMD7 failed\n")));
        DBG((TXT(" CMD7  status:   0x%08x\n"), status));
        DBG((TXT(" CMD7  Response: 0x%08x\n"), MCI_RESP0));
        return(EFSL_ERR_FAIL);
    }

    DBGMCI((TXT("read_sector done\n")));

    return(EFSL_ERR_OK);
}

/**********************************************************
 * name:       mci_block_write()
 **********************************************************
 * writes the block with the given block number from the
 * given buffer; a block is always 512 bytes in size,
 * therefore the buffer must also be 512 bytes in size
 ********************************************************** 
 * arguments:  pointer to interface structure, block number,
 *             pointer to a 512 bytes buffer
 * return val: status: 0 on success
 **********************************************************/
esint8 mci_write_block(hwInterface * card, euint32 block_no, euint8 * buf)
{
    euint32 status;
    euint32 response;
    volatile euint32 count = 0;
	
    DBGMCI((TXT("write sector: %u\n"), block_no));

    /* select card for further usage (CMD7) */
    status = mci_sendcmd(MMC_SELECT_CARD, card->address, MMC_CMD_RESPONSE);
    if (status != MCI_STAT_CMD_RESP_END) {
		DBG((TXT("write: No CMD_RESP_END from SELECT_CARD\n")));
        DBG((TXT("status:   0x%08x\n"), status));
        DBG((TXT("Response: 0x%08x\n"), MCI_RESP0));
        return(EFSL_ERR_FAIL);
    }

    /* if it is a SD card, turn on 4bit modus */
    /* this needs DMA support, as the processor is not fast enough to */
//    if ((card->type == CARD_TYPE_SDSC) || (card->type == CARD_TYPE_SDHC)) {
//        /* configure MCI host adapter for 4bit modus */
//        MCI_CLOCK |= MCI_CLK_WIDE_BUS;
//        for (count = TIME_OUT_VAL; count > 0; count--);
//        /* send CMD55 */
//        mci_sendcmd(MMC_APP_CMD, card->address, MMC_CMD_RESPONSE);
//        /* send ACMD6 */
//        status = mci_sendcmd(SD_APP_SET_BUS_WIDTH, MMC_BUS_WIDTH_4_BIT, MMC_CMD_RESPONSE);
//        if (status != MCI_STAT_CMD_END) {
//            DBGMCI((TXT("ACMD6  status:   0x%08x\n"), status));
//            DBGMCI((TXT("ACMD6  Response: 0x%08x\n"), MCI_RESP0));
//        }
//    }

    /* prepare data transfer */
    /* set data write timeout */
    MCI_DATA_TMR = 0x00100000;
    /* set amount of data to be written */
    MCI_DATA_LEN = MMC_BLOCK_SIZE;
    /* enable data sending */
    MCI_DATA_CTRL = MCI_DATA_TRANS_EN | MCI_DATA_TO_CARD | MCI_DATA_BLOCK_MODE | MCI_DATA_BLOCK_SIZE_512;

    /* initiate data write transfer (CMD24) */
    if (card->type == CARD_TYPE_SDHC) {
        /* high capacity SD cards use the block number as address parameter */
        status = mci_sendcmd(MMC_WRITE_BLOCK, block_no, MMC_CMD_RESPONSE);
    } else {
        /* standard capacity SD cards and MMC cards use the memory address as address parameter */
        status = mci_sendcmd(MMC_WRITE_BLOCK, block_no * MMC_BLOCK_SIZE, MMC_CMD_RESPONSE);
    }
    if (!(status & MCI_STAT_CMD_RESP_END)) {
		DBG((TXT("write: No CMD_RESP_END from CMD24\n")));
        DBG((TXT(" CMD24 status:   0x%08x\n"), status));
        DBG((TXT(" CMD24 Response: 0x%08x\n"), MCI_RESP0));
        return(EFSL_ERR_FAIL);
    }

    /* write the data from the buffer to the bus */
	DBGMCI((TXT("DataTransfer start...")));
    count = 0;
    do {
        while (!(MCI_STATUS & MCI_STAT_TX_FIFO_FULL) && (count < (MMC_BLOCK_SIZE / sizeof(euint32)))) {
            /* mthomas - alignment: MCI_FIFO = ((euint32 *)buf)[count]; */
			MCI_FIFO = ex_getb32( buf, count*4 );
            count++;
        }
    } while (!(MCI_STATUS & (MCI_STAT_DATA_TIMEOUT | MCI_STAT_TX_UNDERRUN | MCI_STAT_DATA_END | MCI_STAT_DATA_BLOCK_END)) && (count < (MMC_BLOCK_SIZE / sizeof(euint32))));
	DBGMCI((TXT("end ")));

#ifdef TEST_MTHOMAS
	if ( MCI_STATUS & MCI_STAT_DATA_TIMEOUT ) {
		DBGMCI((TXT("DATA TIMEOUT ")));
	}
	if ( MCI_STATUS & MCI_STAT_TX_UNDERRUN ) {
		DBGMCI((TXT("TX UNDERRUN ")));
	}
	if ( MCI_STATUS & MCI_STAT_DATA_END ) {
		DBGMCI((TXT("DATA END ")));
	}
	if ( MCI_STATUS & MCI_STAT_DATA_BLOCK_END ) {
		DBGMCI((TXT("DATA BLOCK END ")));
	}
#endif

    /* check if we wrote the correct number of bytes */
    if (count != MMC_BLOCK_SIZE / sizeof(euint32)) {
		DBG((TXT("write: size-mismatch\n")));
        DBG((TXT("write status:   0x%08x\n"), MCI_STATUS));
        DBG((TXT("write count:    %u\n"), count));
        return(EFSL_ERR_FAIL);
    }
    
#ifdef TEST_MTHOMAS
	DBGMCI((TXT("Fifo Data Avail wait start...")));
	count = 0;
    while ( ( MCI_STATUS & MCI_STAT_TX_DATA_AVLBL ) && ( count < TIME_OUT_VAL ) ) {
		count++;
	}
	if ( count == TIME_OUT_VAL ) {
		DBG((TXT("write: Timeout - still FIFO data avail. - but why?\n")));
	}
	DBGMCI((TXT("end ")));
#else
	MCI_CLEAR = MCI_STATUS_CLEAR_MASK;

    /* wait until the FIFO is empty */
	DBGMCI((TXT("Fifo empty wait start...")));
    while (!(MCI_STATUS & MCI_STAT_TX_FIFO_EMPTY)) {
		DBGMCI((TXT(" MCI_STATUS: 0x%08x\n"), MCI_STATUS));
	}
	DBGMCI((TXT("end ")));
#endif
	
    MCI_CLEAR = MCI_STATUS_CLEAR_MASK;
	
    /* wait until the card has processed all data */
	DBGMCI((TXT("WaitProcess start...")));
    count = 0;
    do {
        mci_sendcmd(MMC_SEND_STATUS, card->address, MMC_CMD_RESPONSE);
        response = MCI_RESP0;
        count++;
#ifdef TEST_MTHOMAS
	} while ((response != (MMC_C_S_READY_FOR_DATA | MMC_C_S_STATE_TRAN)) && (count < TIME_OUT_VAL));
	if ( count == TIME_OUT_VAL ) {	
		DBGMCI((TXT("write: timeout waiting for C_S_READY_FOR_DATA | CS_STATE_TRAN\n")));
	}
#else
    } while ((response != (MMC_C_S_READY_FOR_DATA | MMC_C_S_STATE_TRAN)) || (count > TIME_OUT_VAL));
#endif
	DBGMCI((TXT("end ")));

    /* reset data control register */
    MCI_DATA_CTRL = 0;
    for (count = TIME_OUT_VAL; count > 0; count--);

    /* get card status (CMD13) */
    status = mci_sendcmd(MMC_SEND_STATUS, card->address, MMC_CMD_RESPONSE);
    response = MCI_RESP0;
    if (response != (MMC_C_S_READY_FOR_DATA | MMC_C_S_STATE_TRAN)) {
		DBGMCI((TXT(" write: status != MMC_C_S_READY_FOR_DATA | MMC_C_S_STATE_TRAN\n")));
        DBGMCI((TXT(" CMD13 status:   0x%08x\n"), status));
        DBGMCI((TXT(" CMD13 Response: 0x%08x\n"), response));
		DBGMCI((TXT(" trying STOP_TRANSMISSION\n")));
        /* something went wrong - try to stop the data transfer (CMD12) */
        mci_sendcmd(MMC_STOP_TRANSMISSION, 0, MMC_CMD_RESPONSE);
        /* get card status again (CMD13) */
        status = mci_sendcmd(MMC_SEND_STATUS, card->address, MMC_CMD_RESPONSE);
        if (!(status & MCI_STAT_CMD_RESP_END)) {
            /* still not good - bail out */
			DBG((TXT(" write: STOP_TRANSMISSION failed\n")));
            DBG((TXT(" CMD13 status:   0x%08x\n"), status));
            DBG((TXT(" CMD13 Response: 0x%08x\n"), MCI_RESP0));
            return(EFSL_ERR_FAIL);
        }
    }

    /* deselect card (CMD7) */
    status = mci_sendcmd(MMC_SELECT_CARD, 0, MMC_CMD_NO_RESPONSE);
    if (!(status & MCI_STAT_CMD_SENT)) {
		DBG((TXT(" write: DESELECT failed\n")));
        DBG((TXT(" CMD7  status:   0x%08x\n"), status));
        DBG((TXT(" CMD7  Response: 0x%08x\n"), MCI_RESP0));
        return(EFSL_ERR_FAIL);
    }

    DBGMCI((TXT("write_sector done\n")));

    return(EFSL_ERR_OK);
}
