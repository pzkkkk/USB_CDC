#ifndef __SPIFLASH_H__
#define __SPIFLASH_H__

/**Includes************************************************************************************/
#include "stm32f4xx_hal.h"
#include "spi.h"

/**Function declaration************************************************************************/


void sFLASH_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
void sFLASH_EraseSector(uint32_t SectorAddr);
void sFLASH_EraseChip(void);
void sFLASH_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void sFLASH_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void sFLASH_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint32_t sFLASH_ReadID(void);
char sFLASH_SendByte(uint8_t byte);

void FileTest(void);  //函数在user_diskio.c中定义
#endif


