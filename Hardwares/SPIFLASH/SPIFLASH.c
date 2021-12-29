#ifndef __SPIFLASH_H__
#define __SPIFLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "SPIFLASH.h"
#include "spi.h"

#define  W25Q80 	0XEF13 	
#define  W25Q16 	0XEF14
#define  W25Q32 	0XEF15
#define  W25Q64 	0XEF16
#define  W25Q128	0XEF17
#define  M25P64_FLASH_ID        0x202017//C22017
#define  M25P40_FLASH_ID        0x202013

/* USER CODE BEGIN Includes */
#define sFLASH_CMD_WRITE          0x02  /*!< Write to Memory instruction */
#define sFLASH_CMD_WRSR           0x01  /*!< Write Status Register instruction */
#define sFLASH_CMD_WREN           0x06  /*!< Write enable instruction */
#define sFLASH_CMD_READ           0x03  /*!< Read from Memory instruction */
#define sFLASH_CMD_RDSR           0x05  /*!< Read Status Register instruction  */
#define sFLASH_CMD_RDID           0x9F  /*!< Read identification */
#define sFLASH_CMD_SE             0x20  /*!< Sector Erase instruction (4k)*/
#define sFLASH_CMD_BE             0xD8  /*!< Block Erase instruction (64k)*/
#define sFLASH_CMD_CE             0xC7  /*!< Chip Erase instruction (Chip Erase)*/
#define sFLASH_WIP_FLAG           0x01  /*!< Write In Progress (WIP) flag */
#define sFLASH_CMD_RDID           0x9F  /*!< Read identification */
#define sFLASH_CMD_DeviceID			    0xAB 
#define sFLASH_CMD_ManufactDeviceID	    0x90 
#define sFLASH_CMD_JedecDeviceID		0x9F 
#define sFLASH_DUMMY_BYTE         0xFF

uint8_t sFlashBuff[4096];

//ƬѡCS�õ�λ
void sFLASH_CS_LOW(void)
{
    HAL_GPIO_WritePin(SPIFLASH_CS_GPIO_Port,SPIFLASH_CS_Pin,GPIO_PIN_RESET);
}	
//ƬѡCS�ø�λ
void sFLASH_CS_HIGH(void)
{
    HAL_GPIO_WritePin(SPIFLASH_CS_GPIO_Port,SPIFLASH_CS_Pin,GPIO_PIN_SET);
}	

//FLASH ����һ���ֽ�
char sFLASH_SendByte(uint8_t byte)
{
//	unsigned char dr;
//  /*!< Loop while DR register in not emplty */
//  //while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
//	while((SPI2->SR & SPI_SR_TXE) == 0);

//  /*!< Send byte through the SPI1 peripheral */
//  //SPI_I2S_SendData(SPI1, byte);
//	SPI2->DR = byte;

//  /*!< Wait to receive a byte */
//  //while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
//	while((SPI2->SR & SPI_SR_RXNE) == 0);

//  /*!< Return the byte read from the SPI bus */
//  //return SPI_I2S_ReceiveData(SPI1);
//	dr = SPI2->DR;
//	return dr;
	uint8_t d_read,d_send = byte;
	if(HAL_SPI_TransmitReceive(&hspi1,&d_send,&d_read,1,0xFFFF) != HAL_OK)
	{
		d_read = sFLASH_DUMMY_BYTE;
	}
	return d_read;
}

//FLASHдʹ��
void sFLASH_WriteEnable(void)
{
  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();
  /*!< Send "Write Enable" instruction */
  sFLASH_SendByte(sFLASH_CMD_WREN);

  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();
}

//FLASH��ȡһ���ֽ�
uint8_t sFLASH_ReadByte(void)
{
  return (sFLASH_SendByte(sFLASH_DUMMY_BYTE));
}
//FLASH�ȴ�д���
void sFLASH_WaitForWriteEnd(void)
{
    uint8_t flashstatus = 0;
    HAL_Delay(1);
    /*!< Select the FLASH: Chip Select low */
    sFLASH_CS_LOW();
    HAL_Delay(1);
    /*!< Send "Read Status Register" instruction */
    sFLASH_SendByte(sFLASH_CMD_RDSR);

    /*!< Loop as long as the memory is busy with a write cycle */
    do
    {
    /*!< Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
    flashstatus = sFLASH_SendByte(sFLASH_DUMMY_BYTE);

    }
    while ((flashstatus & sFLASH_WIP_FLAG) == SET); /* Write in progress */

    /*!< Deselect the FLASH: Chip Select high */
    sFLASH_CS_HIGH();
}

//FLASH����һ������
void sFLASH_EraseSector(uint32_t SectorAddr)
{
  /*!< Send write enable instruction */
	sFLASH_WriteEnable();
  /*!< Sector Erase */
  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();
  /*!< Send Sector Erase instruction */
	sFLASH_SendByte(sFLASH_CMD_SE);
  /*!< Send SectorAddr high nibble address byte */
  sFLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
  /*!< Send SectorAddr medium nibble address byte */
  sFLASH_SendByte((SectorAddr & 0xFF00) >> 8);
  /*!< Send SectorAddr low nibble address byte */
  sFLASH_SendByte(SectorAddr & 0xFF);
  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();

  /*!< Wait the end of Flash writing */
  sFLASH_WaitForWriteEnd();
}
//FLASH��������Ƭ
void sFLASH_EraseChip(void)
{
  /*!< Send write enable instruction */
  sFLASH_WriteEnable();

  /*!< Bulk Erase */
  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();
  /*!< Send Bulk Erase instruction  */
  sFLASH_SendByte(sFLASH_CMD_CE);
  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();

  /*!< Wait the end of Flash writing */
  sFLASH_WaitForWriteEnd();
}
//FLASHдһ��ҳ
void sFLASH_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    sFLASH_WriteEnable();
    sFLASH_CS_LOW();
    sFLASH_SendByte(sFLASH_CMD_WRITE);
    sFLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
    sFLASH_SendByte((WriteAddr & 0xFF00) >> 8);
    sFLASH_SendByte(WriteAddr & 0xFF);

    while (NumByteToWrite--)
    {
        sFLASH_SendByte(*pBuffer);
        pBuffer++;
    }

    sFLASH_CS_HIGH();
    sFLASH_WaitForWriteEnd();
}
//FLASH��ȡ0-65536��
void sFLASH_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
    sFLASH_CS_LOW();
    sFLASH_SendByte(sFLASH_CMD_READ);
    sFLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
    sFLASH_SendByte((ReadAddr& 0xFF00) >> 8);
    sFLASH_SendByte(ReadAddr & 0xFF);

    while (NumByteToRead--) /*!< while there is data to be read */
    {
        *pBuffer = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
        pBuffer++;
    }

    sFLASH_CS_HIGH();
}
//��ȡFLASH ID
uint32_t sFLASH_ReadID(void)
{
/*    uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;
    sFLASH_CS_LOW();
    HAL_Delay(1);
    sFLASH_SendByte(sFLASH_CMD_RDID);
    Temp0 = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
    Temp1 = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
    Temp2 = sFLASH_SendByte(sFLASH_DUMMY_BYTE);

    sFLASH_CS_HIGH();
    Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

    return Temp;*/
	
	uint16_t Temp = 0;	  
	sFLASH_CS_LOW();				    
	sFLASH_SendByte(0x90);//���Ͷ�ȡID����	    
	sFLASH_SendByte(0x00); 	    
	sFLASH_SendByte(0x00); 	    
	sFLASH_SendByte(0x00); 	 			   
	Temp|=sFLASH_SendByte(0xFF)<<8;  
	Temp|=sFLASH_SendByte(0xFF);	 
	sFLASH_CS_HIGH();			    
	return Temp;
}
//��У��дSPI FLASH 
void sFLASH_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 			 		 
	uint16_t pageremain;	   
	pageremain=256-WriteAddr%256;      //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		sFLASH_WritePage(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)
        {
            break;//д�������
        }
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)
            {
                pageremain=256; //һ�ο���д��256���ֽ�
            }
			else
            {
                pageremain=NumByteToWrite; 	  //����256���ֽ�
            }
		}
	}
}
//��������д0-65536���ֽں���
void sFLASH_WriteBuffer(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    
	uint8_t * psFlashBuff;	  
  psFlashBuff=sFlashBuff;	     
 	secpos=WriteAddr/4096;//������ַ  
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   
 	if(NumByteToWrite<=secremain)
    {
        secremain=NumByteToWrite;//������4096���ֽ�
    }
    
	while(1) 
	{	
		sFLASH_ReadBuffer(psFlashBuff,secpos*4096,4096);//������������������       
		sFLASH_EraseSector(secpos*4096);		//�����������
		for(i=0;i<secremain;i++)	   		//У������
		{
			psFlashBuff[i+secoff]=pBuffer[i];	  
		}
		sFLASH_Write_NoCheck(psFlashBuff,secpos*4096,4096);//д���������� 
        
		if(NumByteToWrite==secremain)
        {
            break;//д�������
        }
		else//д��δ����
		{
      secpos++;//������ַ��1
      secoff=0;//ƫ��λ��Ϊ0	 

      pBuffer+=secremain;  				    //ָ��ƫ��
      WriteAddr+=secremain;				    //д��ַƫ��   
      NumByteToWrite-=secremain;			//�ֽ����ݼ�
      if(NumByteToWrite>4096)
      {
        secremain=4096;//��һ����������д����
      }
      else 
      {
        secremain=NumByteToWrite;		//��һ����������д����
      }
		}	 
	}
}

#endif

