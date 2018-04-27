/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/
#include <string.h>
#include "diskio.h"
#include "stm32f10x.h"
#include "./sdio/sdio_sdcard.h"

#define ATA 0
#define SPI_FLASH 1

#define SD_BLOCKSIZE 512

extern SD_CardInfo SDCardInfo;

DSTATUS disk_status(
	BYTE pdrv)
{
	DSTATUS status = STA_NOINIT;

	switch (pdrv)
	{
	case ATA: /* SD CARD */
		status &= ~STA_NOINIT;
		break;

	case SPI_FLASH: /* SPI Flash */
		break;

	default:
		status = STA_NOINIT;
	}
	return status;
}

DSTATUS disk_initialize(
	BYTE pdrv)
{
	DSTATUS status = STA_NOINIT;
	switch (pdrv)
	{
	case ATA: /* SD CARD */
		if (SD_Init() == SD_OK)
		{
			status &= ~STA_NOINIT;
		}
		else
		{
			status = STA_NOINIT;
		}

		break;

	case SPI_FLASH: /* SPI Flash */
		break;

	default:
		status = STA_NOINIT;
	}
	return status;
}

DRESULT disk_read(
	BYTE pdrv,
	BYTE *buff,
	DWORD sector,
	UINT count)
{
	DRESULT status = RES_PARERR;
	SD_Error SD_state = SD_OK;

	switch (pdrv)
	{
	case ATA: /* SD CARD */
		if ((DWORD)buff & 3)
		{
			DRESULT res = RES_OK;
			DWORD scratch[SD_BLOCKSIZE / 4];

			while (count--)
			{
				res = disk_read(ATA, (void *)scratch, sector++, 1);

				if (res != RES_OK)
				{
					break;
				}
				memcpy(buff, scratch, SD_BLOCKSIZE);
				buff += SD_BLOCKSIZE;
			}
			return res;
		}

		SD_state = SD_ReadMultiBlocks(buff, sector * SD_BLOCKSIZE, SD_BLOCKSIZE, count);
		if (SD_state == SD_OK)
		{
			/* Check if the Transfer is finished */
			SD_state = SD_WaitReadOperation();
			while (SD_GetStatus() != SD_TRANSFER_OK)
				;
		}
		if (SD_state != SD_OK)
			status = RES_PARERR;
		else
			status = RES_OK;
		break;

	case SPI_FLASH:
		break;

	default:
		status = RES_PARERR;
	}
	return status;
}

#if _USE_WRITE
DRESULT disk_write(
	BYTE pdrv,
	const BYTE *buff,
	DWORD sector,
	UINT count)
{
	DRESULT status = RES_PARERR;
	SD_Error SD_state = SD_OK;

	if (!count)
	{
		return RES_PARERR; /* Check parameter */
	}

	switch (pdrv)
	{
	case ATA: /* SD CARD */
		if ((DWORD)buff & 3)
		{
			DRESULT res = RES_OK;
			DWORD scratch[SD_BLOCKSIZE / 4];

			while (count--)
			{
				memcpy(scratch, buff, SD_BLOCKSIZE);
				res = disk_write(ATA, (void *)scratch, sector++, 1);
				if (res != RES_OK)
				{
					break;
				}
				buff += SD_BLOCKSIZE;
			}
			return res;
		}

		SD_state = SD_WriteMultiBlocks((uint8_t *)buff, sector * SD_BLOCKSIZE, SD_BLOCKSIZE, count);
		if (SD_state == SD_OK)
		{
			/* Check if the Transfer is finished */
			SD_state = SD_WaitWriteOperation();

			/* Wait until end of DMA transfer */
			while (SD_GetStatus() != SD_TRANSFER_OK)
				;
		}
		if (SD_state != SD_OK)
			status = RES_PARERR;
		else
			status = RES_OK;
		break;

	case SPI_FLASH:
		break;

	default:
		status = RES_PARERR;
	}
	return status;
}
#endif

#if _USE_IOCTL
DRESULT disk_ioctl(
	BYTE pdrv,
	BYTE cmd,
	void *buff)
{
	DRESULT status = RES_PARERR;
	switch (pdrv)
	{
	case ATA: /* SD CARD */
		switch (cmd)
		{
		// Get R/W sector size (WORD)
		case GET_SECTOR_SIZE:
			*(WORD *)buff = SD_BLOCKSIZE;
			break;
		// Get erase block size in unit of sector (DWORD)
		case GET_BLOCK_SIZE:
			*(DWORD *)buff = 1;
			break;

		case GET_SECTOR_COUNT:
			*(DWORD *)buff = SDCardInfo.CardCapacity / SDCardInfo.CardBlockSize;
			break;
		case CTRL_SYNC:
			break;
		}
		status = RES_OK;
		break;

	case SPI_FLASH:
		break;

	default:
		status = RES_PARERR;
	}
	return status;
}
#endif

__weak DWORD get_fattime(void)
{

	return ((DWORD)(2015 - 1980) << 25) /* Year 2015 */
		   | ((DWORD)1 << 21)			/* Month 1 */
		   | ((DWORD)1 << 16)			/* Mday 1 */
		   | ((DWORD)0 << 11)			/* Hour 0 */
		   | ((DWORD)0 << 5)			/* Min 0 */
		   | ((DWORD)0 >> 1);			/* Sec 0 */
}
