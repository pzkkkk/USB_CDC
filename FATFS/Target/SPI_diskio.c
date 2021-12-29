/* USER CODE BEGIN Header */
/**
 ******************************************************************************
  * @file    user_diskio.c
  * @brief   This file includes a diskio driver skeleton to be completed by the user.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
 /* USER CODE END Header */

#ifdef USE_OBSOLETE_USER_CODE_SECTION_0
/*
 * Warning: the user section 0 is no more in use (starting from CubeMx version 4.16.0)
 * To be suppressed in the future.
 * Kept to ensure backward compatibility with previous CubeMx versions when
 * migrating projects.
 * User code previously added there should be copied in the new user sections before
 * the section contents can be deleted.
 */
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */
#endif

/* USER CODE BEGIN DECL */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "ff_gen_drv.h"


#include "spi.h"
#include "w25qxx.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* Disk status */
static volatile DSTATUS Stat = STA_NOINIT;

#define PAGE_SIZE       256
#define SECTOR_SIZE     4096
#define SECTOR_COUNT	  200
#define BLOCK_SIZE	    65536
#define FLASH_PAGES_PER_SECTOR	SECTOR_SIZE/PAGE_SIZE

FATFS fs;
FIL file;						               /* 文件对象 */
FIL *pf = &file;                   //将FIL结构体对象设置为全局变量！！！！！！！！！！！！
FRESULT f_res;                     /* 文件操作结果 */
UINT fnum;            					   /* 文件成功读写数量 */
BYTE ReadBuffer[1024]={0};         /* 读缓冲区 */
BYTE WriteBuffer[]= "PZKKKKK666\n";
BYTE work[4096];

char USER_Path[4];
void mount_disk(void)
{
   f_res = f_mount(&fs, USER_Path, 0);
   return;

}

void format_disk(void)
{
  f_res = f_mkfs(USER_Path, FM_FAT, 4096, work, sizeof(work));
}

 UINT bw;
void create_file(void)
{

  f_res = f_open(pf, "test.txt", FA_OPEN_ALWAYS | FA_WRITE);
  f_res = f_write(&file, WriteBuffer, sizeof(WriteBuffer), &bw);
  f_res = f_close(pf);
}

void read_file(void)
{
  f_res = f_open(&file, "test.txt", FA_READ);
  f_res = f_read(&file, ReadBuffer, sizeof(WriteBuffer), &bw);
  f_res = f_close(&file);
}


FRESULT  fileSystemInit()
{
	FRESULT res = FR_OK;
	res = f_mount(&fs, USER_Path, 1);
	if (res != FR_OK)
	{
		//No Disk file system,format disk !
		res = f_mkfs(USER_Path, FM_FAT, 4096, work, sizeof work);
		if (res == FR_OK)
		{
			res = f_mount(&fs, USER_Path, 1);
			if (res == 0)
			{
				return FR_OK;
			}
			else
				return FR_DISK_ERR;
		}
		else
			return FR_DISK_ERR;
	}
	else
	return FR_OK;
}

void FatfsTest(void)
{
	mount_disk();		    //文件系统注册
  format_disk();		  //格式化文件系统
//	fileSystemInit();
	create_file();		    //建立文件并写入"PZKKKKK666\n"  
	read_file();		      //读取文件放到ReadBuffer中
}

/* USER CODE END DECL */

/* Private function prototypes -----------------------------------------------*/
DSTATUS SpiDisk_initialize (BYTE pdrv);
DSTATUS SpiDisk_status (BYTE pdrv);
DRESULT SpiDisk_read (BYTE pdrv, BYTE *buff, DWORD sector, UINT count);
//#if _USE_WRITE == 1
  DRESULT SpiDisk_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);
//#endif /* _USE_WRITE == 1 */
//#if _USE_IOCTL == 1
  DRESULT SpiDisk_ioctl (BYTE pdrv, BYTE cmd, void *buff);
//#endif /* _USE_IOCTL == 1 */

Diskio_drvTypeDef  SPI_Driver =
{
  SpiDisk_initialize,
  SpiDisk_status,
  SpiDisk_read,
#if  _USE_WRITE == 1
  SpiDisk_write,
#endif  /* _USE_WRITE == 1 */
#if  _USE_IOCTL == 1
  SpiDisk_ioctl,
#endif /* _USE_IOCTL == 1 */
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes a Drive
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS SpiDisk_initialize (
	BYTE pdrv           /* Physical drive nmuber to identify the drive */
)
{
  /* USER CODE BEGIN INIT */
    Stat = STA_NOINIT;
	  if(W25QXX_ReadID() != 0)
		{
			Stat &= ~STA_NOINIT;
		}	
    return Stat;
  /* USER CODE END INIT */
}

/**
  * @brief  Gets Disk Status
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS SpiDisk_status (
	BYTE pdrv       /* Physical drive number to identify the drive */
)
{
  /* USER CODE BEGIN STATUS */
    Stat &= ~STA_NOINIT;
    return Stat;
  /* USER CODE END STATUS */
}

/**
  * @brief  Reads Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT SpiDisk_read (
	BYTE pdrv,      /* Physical drive nmuber to identify the drive */
	BYTE *buff,     /* Data buffer to store read data */
	DWORD sector,   /* Sector address in LBA */
	UINT count      /* Number of sectors to read */
)
{
  /* USER CODE BEGIN READ */
		DRESULT res = RES_ERROR;
		UINT i;
		
		for(i = 0;i < count;i++)
		{
			W25QXX_Read(buff + i * 4096,sector * 4096 + i * 4096,4096 );
		}
		
    return RES_OK;
  /* USER CODE END READ */
}

/**
  * @brief  Writes Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
#if _USE_WRITE == 1
DRESULT SpiDisk_write (
	BYTE pdrv,          /* Physical drive nmuber to identify the drive */
	const BYTE *buff,   /* Data to be written */
	DWORD sector,       /* Sector address in LBA */
	UINT count          /* Number of sectors to write */
)
{
  /* USER CODE BEGIN WRITE */
		DRESULT res = RES_ERROR;
		
		UINT i;
		
		for(i = 0;i < count;i++)
		{
			W25QXX_Write((void *)(buff + i * 4096),sector * 4096 + i * 4096,4096 );
		}
		
		res = RES_OK;
  /* USER CODE HERE */
    return res;
  /* USER CODE END WRITE */
}
#endif /* _USE_WRITE == 1 */

/**
  * @brief  I/O control operation
  * @param  pdrv: Physical drive number (0..)
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
#if _USE_IOCTL == 1
DRESULT SpiDisk_ioctl (
	BYTE pdrv,      /* Physical drive nmuber (0..) */
	BYTE cmd,       /* Control code */
	void *buff      /* Buffer to send/receive control data */
)
{
  /* USER CODE BEGIN IOCTL */
    DRESULT res = RES_OK;

		switch(cmd)
		{
			case CTRL_SYNC :
				break;	
		 
			case CTRL_TRIM:
				break;
				
			case GET_BLOCK_SIZE:
			*(DWORD*)buff = BLOCK_SIZE; 
			break;
				
			case GET_SECTOR_SIZE:
			*(DWORD*)buff = SECTOR_SIZE;
				break;
				
			case GET_SECTOR_COUNT:
			*(DWORD*)buff = SECTOR_COUNT;
			break;
					
			default:
			res = RES_PARERR;
			break;
			}

    return res;
  /* USER CODE END IOCTL */
}
#endif /* _USE_IOCTL == 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
