//Preserved Header for former author:
/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/


#include "../SD/sd.h"
#include "diskio.h"

static DSTATUS Stat = STA_NOINIT;	/* Disk status */

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */

DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0) */
)
{
	if (drv) return STA_NOINIT;	/* Supports only single drive */
		
	if (!SD_init())	/* Initialization succeded */
	{			
		Stat &= ~STA_NOINIT; /* Clear STA_NOINIT */
	} 

	return Stat;
}

/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0) */
)
{
	if (drv) return STA_NOINIT;		/* Supports only single drive */
	return Stat;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber:(0) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
	if (drv || !count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	if (count == 1)/* Single block read */
	{	
		if(!SD_readSingleBlock(buff, sector)) count = 0;
		
	}
	else/* Multiple block read */
	{							
		if(!SD_readMultipleBlock (buff, sector, count)) count = 0;
	}

	return count ? RES_ERROR : RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write  */
)
{
	if (drv || !count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	if (count == 1)/* Single block write */
	{	
		if(!SD_writeSingleBlock(buff, sector)) count = 0;
		
	}
	else/* Multiple block write */
	{							
		if(!SD_writeMultipleBlock (buff, sector, count)) count = 0;
	}

	return count ? RES_ERROR : RES_OK;
}
#endif /* _READONLY */

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	BYTE n, csd[16];
	DWORD csize;

	if (drv) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	res = RES_ERROR;

	switch (ctrl) {
		case CTRL_SYNC :	/* Flush dirty buffer if present */
			return RES_OK;
			break;

		case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (WORD) */
			if((SD_sendCommand(CMD9, 0) == 0) && receive_DataBlock(csd, 16)) 
			{
				if((csd[0] >> 6) == 1) /* SDC ver 2.00 */
				{	
					csize = csd[9] + ((WORD)csd[8] << 8) + 1;
					*(DWORD*)buff = (DWORD)csize << 10;
				} 
				else /* MMC or SDC ver 1.XX */
				{				
					n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
					csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
					*(DWORD*)buff = (DWORD)csize << (n - 9);
				}
				res = RES_OK;
			}
			break;

		case GET_SECTOR_SIZE :	/* Get sectors on the disk (WORD) */
			*(WORD*)buff = 512;
			res = RES_OK;
			break;

		case GET_BLOCK_SIZE :	/* Get erase block size in unit of sectors (DWORD) */
			if ((SD_sendCommand(CMD9, 0) == 0) && receive_DataBlock(csd, 16)) /* Read CSD */
			{	
				*(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
				res = RES_OK;
			}
			break;

		default:
			res = RES_PARERR;
	}

	return res;
}

